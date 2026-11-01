#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "goform_client.h"

int run_command(char *output, size_t size, const char *cmd, ...) {
  (void)output;
  (void)size;
  (void)cmd;
  return -1;
}

static void test_device_info_fields(void) {
  const char *json =
      "{\"data\":{\"oemname\":\"SZ\",\"devtype\":\"542\","
      "\"devsubtype\":\"SRB876\",\"device\":\"123456\","
      "\"usemnc\":11}}";
  char oem[16];
  int priority;

  assert(goform_json_string(json, "oemname", oem, sizeof(oem)) == 0);
  assert(strcmp(oem, "SZ") == 0);
  assert(goform_json_int(json, "usemnc", &priority) == 0);
  assert(priority == 11);
}

static void test_device_status_fields(void) {
  const char *json =
      "{\"data\":{\"battery\":100,\"voltage\":4350,"
      "\"current\":null,\"charging\":1,\"oemname\":\"SZ\"}}";
  int value;

  assert(goform_json_int(json, "battery", &value) == 0 && value == 100);
  assert(goform_json_int(json, "voltage", &value) == 0 && value == 4350);
  assert(goform_json_int(json, "current", &value) != 0);
}

static void test_json_parser_rejects_missing_values(void) {
  char value[16];
  int number;

  assert(goform_json_string("{\"oemname\":null}", "oemname", value,
                           sizeof(value)) != 0);
  assert(goform_json_int("{\"usemnc\":null}", "usemnc", &number) != 0);
  assert(goform_json_int("{\"usemnc\":1.5}", "usemnc", &number) != 0);
  assert(goform_json_string("{\"data\":{\"name\":\"ok\"}}", "name",
                            value, sizeof(value)) == 0);
  assert(strcmp(value, "ok") == 0);
  assert(goform_json_string("{\"data\":{\"name\":\"unterminated}}",
                            "name", value, sizeof(value)) != 0);
}

static void test_wifi_query_builder(void) {
  char query[256];

  assert(goform_build_wifi_query("wifi name", "p@ssword!", query,
                                 sizeof(query)) == 0);
  assert(strcmp(query,
                "goformId=setapinfo&ap_ssid=wifi%20name&"
                "ap_ssidpwd=p%40ssword%21&admin=admin&pwd=admin") == 0);
  assert(goform_build_wifi_query("", "pwd", query, sizeof(query)) != 0);
  assert(goform_build_wifi_query("ssid", "", query, sizeof(query)) != 0);
}

static void test_proxy_allowlist(void) {
  assert(goform_proxy_operation_allowed("getDevInfo") == 1);
  assert(goform_proxy_operation_allowed("getDevicePkgInfo") == 1);
  assert(goform_proxy_operation_allowed("setapinfo") == 1);
  assert(goform_proxy_operation_allowed("setPriorityMnc") == 1);
  assert(goform_proxy_operation_allowed("cmd") == 0);
  assert(goform_proxy_operation_allowed("http://example.com") == 0);
}

int main(void) {
  test_device_info_fields();
  test_device_status_fields();
  test_json_parser_rejects_missing_values();
  test_wifi_query_builder();
  test_proxy_allowlist();
  return 0;
}
