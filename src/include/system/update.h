/**
 * @file update.h
 * @brief OTA更新系统
 */

#ifndef UPDATE_H
#define UPDATE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 当前工具版本
 * 构建时由 src/Makefile 注入 -DUDX710_TOOLS_VERSION=...（CI 取 git tag），
 * 下面的兜底值只在未注入时使用，避免版本号与发布 tag 不一致。 */
#ifndef UDX710_TOOLS_VERSION
#define UDX710_TOOLS_VERSION "v2.4.9"
#endif

/* 构建提交号（构建时注入，便于确认设备上跑的是哪一次提交） */
#ifndef UDX710_TOOLS_COMMIT
#define UDX710_TOOLS_COMMIT "unknown"
#endif

/* 当前固件版本 */
#define FIRMWARE_VERSION UDX710_TOOLS_VERSION

/* 更新文件路径 */
#define UPDATE_TMP_DIR "/tmp"
#define UPDATE_ZIP_PATH "/tmp/update.zip"
#define UPDATE_EXTRACT_DIR "/tmp/update"
#define UPDATE_INSTALL_SCRIPT "/tmp/update/install.sh"

/* 版本检查URL（编译时嵌入，指向 SZ50-UDX710-TOOLS 仓库根目录的 version.json） */
#define UPDATE_CHECK_URL \
  "https://raw.githubusercontent.com/wuhao1477/SZ50-UDX710-TOOLS/main/version.json"

/* 安装脚本签名配置文件 */
#define UPDATE_CONFIG_FILE "/tmp/update/configuration.json"

/* 版本信息结构 */
typedef struct {
  char version[32];
  char url[512];
  char changelog[1024];
  size_t size;
  int required;
} update_info_t;

/**
 * @brief 获取当前版本
 * @return 版本字符串
 */
const char *update_get_version(void);

/**
 * @brief 获取构建提交号
 * @return 提交号字符串（未注入时为 "unknown"）
 */
const char *update_get_build_commit(void);

/**
 * @brief 获取嵌入的版本检查URL
 * @return URL字符串
 */
const char *update_get_embedded_url(void);

/**
 * @brief 从URL下载更新包
 * @param url 下载链接
 * @return 0成功, -1失败
 */
int update_download(const char *url);

/**
 * @brief 解压更新包
 * @return 0成功, -1失败
 */
int update_extract(void);

/**
 * @brief 执行安装脚本
 * @param output 输出缓冲区
 * @param size 缓冲区大小
 * @return 0成功, -1失败
 */
int update_install(char *output, size_t size);

/**
 * @brief 清理更新临时文件
 */
void update_cleanup(void);

/**
 * @brief 检查远程版本
 * @param check_url 版本检查URL
 * @param info 版本信息输出
 * @return 0成功, -1失败
 */
int update_check_version(const char *check_url, update_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* UPDATE_H */
