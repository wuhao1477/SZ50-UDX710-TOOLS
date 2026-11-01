#include "goform_proxy.h"

#include "builtin_cards.h"
#include "goform_client.h"
#include "http_utils.h"

#include <stdlib.h>
#include <string.h>

#define GOFORM_PROXY_TEXT_MAX 256

typedef struct {
  char operation[32];
  char operator_id[16];
  char ssid[GOFORM_PROXY_TEXT_MAX];
  char password[GOFORM_PROXY_TEXT_MAX];
} GoformProxyAsyncArg;

static int proxy_get(const char *operation, char *response, size_t size) {
  if (strcmp(operation, "getDevInfo") == 0) {
    return goform_get_device_info(response, size);
  }
  if (strcmp(operation, "getAllDeviceInfo") == 0) {
    return goform_get_all_device_info(response, size);
  }
  if (strcmp(operation, "getSimInfo") == 0) {
    return goform_get_sim_info(response, size);
  }
  if (strcmp(operation, "getDevicePkgInfo") == 0) {
    return goform_get_package_info(response, size);
  }
  return -1;
}

static int proxy_get_operation_allowed(const char *operation) {
  return strcmp(operation, "getDevInfo") == 0 ||
         strcmp(operation, "getAllDeviceInfo") == 0 ||
         strcmp(operation, "getSimInfo") == 0 ||
         strcmp(operation, "getDevicePkgInfo") == 0;
}

static int goform_proxy_worker(void *arg, char *response,
                               size_t response_size) {
  GoformProxyAsyncArg *task = (GoformProxyAsyncArg *)arg;
  int priority;

  if (!task) return -1;
  if (proxy_get_operation_allowed(task->operation)) {
    return proxy_get(task->operation, response, response_size);
  }
  if (strcmp(task->operation, "setapinfo") == 0) {
    return goform_set_wifi_info(task->ssid, task->password, response,
                                response_size);
  }
  return builtin_card_switch_operator(task->operator_id, response,
                                      response_size, &priority);
}

static void complete_goform_proxy(struct mg_connection *c, int result,
                                  const char *response, void *arg) {
  GoformProxyAsyncArg *task = (GoformProxyAsyncArg *)arg;

  if (result == 0) {
    HTTP_JSON(c, 200, response);
  } else if (task && strcmp(task->operation, "setapinfo") == 0) {
    HTTP_ERROR(c, 502, "WiFi 配置写入失败");
  } else if (task && strcmp(task->operation, "setPriorityMnc") == 0 &&
             result == -2) {
    HTTP_ERROR(c, 409, "当前设备不支持内置卡运营商切换");
  } else if (task && strcmp(task->operation, "setPriorityMnc") == 0 &&
             result == -3) {
    HTTP_ERROR(c, 400, "内置卡不存在、未就绪或运营商参数无效");
  } else if (task && strcmp(task->operation, "setPriorityMnc") == 0 &&
             result == -4) {
    HTTP_ERROR(c, 502, "实名状态查询失败");
  } else if (task && strcmp(task->operation, "setPriorityMnc") == 0 &&
             result == -5) {
    HTTP_ERROR(c, 403, "该内置卡尚未实名，禁止切换");
  } else if (task && strcmp(task->operation, "setPriorityMnc") == 0) {
    HTTP_ERROR(c, 502, "设备运营商切换失败");
  } else {
    HTTP_ERROR(c, 502, "goform 查询失败");
  }
}

/* WiFi 配置写入。密码只透传到设备本地 goform，不会回显或落盘。 */
static void handle_goform_setapinfo(struct mg_connection *c,
                                    struct mg_http_message *hm) {
  char *ssid = mg_json_get_str(hm->body, "$.ssid");
  char *password = mg_json_get_str(hm->body, "$.password");
  GoformProxyAsyncArg *task = NULL;

  if (ssid && password && ssid[0] && password[0] &&
      strlen(ssid) < GOFORM_PROXY_TEXT_MAX &&
      strlen(password) < GOFORM_PROXY_TEXT_MAX) {
    task = (GoformProxyAsyncArg *)calloc(1, sizeof(*task));
    if (task) {
      snprintf(task->operation, sizeof(task->operation), "setapinfo");
      snprintf(task->ssid, sizeof(task->ssid), "%s", ssid);
      snprintf(task->password, sizeof(task->password), "%s", password);
    }
  }
  free(ssid);
  free(password);

  if (!task || goform_async_submit(c, goform_proxy_worker, task, free,
                                   complete_goform_proxy) != 0) {
    free(task);
    HTTP_ERROR(c, 503, "goform 服务暂不可用");
  }
}

void handle_goform_proxy(struct mg_connection *c, struct mg_http_message *hm) {
  char *operation;
  char *operator_id = NULL;
  GoformProxyAsyncArg *task;

  HTTP_CHECK_POST(c, hm);
  operation = mg_json_get_str(hm->body, "$.operation");
  if (!operation) {
    HTTP_ERROR(c, 400, "operation 参数无效");
    return;
  }
  if (!goform_proxy_operation_allowed(operation)) {
    free(operation);
    HTTP_ERROR(c, 400, "goform 操作未允许");
    return;
  }
  if (strcmp(operation, "setapinfo") == 0) {
    free(operation);
    handle_goform_setapinfo(c, hm);
    return;
  }

  if (proxy_get_operation_allowed(operation)) {
    task = (GoformProxyAsyncArg *)calloc(1, sizeof(*task));
    if (task && strlen(operation) < sizeof(task->operation)) {
      snprintf(task->operation, sizeof(task->operation), "%s", operation);
    } else {
      free(task);
      task = NULL;
    }
    free(operation);
    if (!task || goform_async_submit(c, goform_proxy_worker, task, free,
                                     complete_goform_proxy) != 0) {
      free(task);
      HTTP_ERROR(c, 503, "goform 服务暂不可用");
      return;
    }
    return;
  }

  /* The write operation is intentionally bound to operatorId.  A raw
   * priorityMnc is never accepted, so this endpoint cannot bypass the
   * real-name check in builtin_card_switch_operator(). */
  if (strcmp(operation, "setPriorityMnc") != 0) {
    free(operation);
    HTTP_ERROR(c, 400, "goform 操作未允许");
    return;
  }
  operator_id = mg_json_get_str(hm->body, "$.operatorId");
  free(operation);
  if (!operator_id) {
    HTTP_ERROR(c, 400, "setPriorityMnc 必须提供 operatorId");
    return;
  }
  task = (GoformProxyAsyncArg *)calloc(1, sizeof(*task));
  if (task && strlen(operator_id) < sizeof(task->operator_id)) {
    snprintf(task->operation, sizeof(task->operation), "setPriorityMnc");
    snprintf(task->operator_id, sizeof(task->operator_id), "%s", operator_id);
  } else {
    free(task);
    task = NULL;
  }
  free(operator_id);
  if (!task || goform_async_submit(c, goform_proxy_worker, task, free,
                                   complete_goform_proxy) != 0) {
    free(task);
    HTTP_ERROR(c, 503, "goform 服务暂不可用");
  }
}
