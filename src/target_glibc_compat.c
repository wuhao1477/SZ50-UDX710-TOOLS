/* Compatibility shims for building with a newer host glibc against the
 * device's glibc 2.27 runtime.  The cross linker otherwise selects newer
 * stat ABI entry points even though the host headers expose newer defaults.
 *
 * 警告：只允许放 stat/lstat 这类普通转发。
 * 绝对不要再在这里定义 pthread_create/pthread_join/pthread_detach 或
 * __libc_start_main 的本地包装（曾经在 4f8d094 引入过）——那会让
 * 交叉编译出的 aarch64 二进制在设备上启动即卡死（CPU 100% 忙循环，
 * 连 main() 都进不去），已在真机上复现并定位。回归检查见
 * tests/check_routes.sh。
 */

#include <sys/stat.h>

extern int __xstat(int version, const char *path, struct stat *buf);
extern int __lxstat(int version, const char *path, struct stat *buf);

int stat(const char *path, struct stat *buf) {
  return __xstat(0, path, buf);
}

int lstat(const char *path, struct stat *buf) {
  return __lxstat(0, path, buf);
}
