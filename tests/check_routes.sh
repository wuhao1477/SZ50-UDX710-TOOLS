#!/bin/sh
set -eu

# 认证与基础 API
for route in /api/auth/login /api/apn/config /api/plugins /api/rathole/status \
  /api/ipv6-proxy/status /api/netif/list /api/usb/mode /api/info /api/health \
  /api/version; do
  grep -q "\"$route\"" src/handlers/http_server.c
done

# 版本号必须由构建注入（禁止再写死与发布 tag 不一致的常量）
grep -q 'UDX710_TOOLS_VERSION' src/Makefile
grep -q 'UDX710_TOOLS_VERSION' src/include/system/update.h
grep -q 'VERSION:' .github/workflows/build.yml

# SZ50 设备适配 API
for route in /api/capabilities /api/led/status /api/led/control /api/wifi/status \
  /api/wifi/config /api/wifi/clients /api/factory-reset; do
  grep -q "\"$route\"" src/handlers/http_server.c
done

# ADB 管理 API
for route in /api/adb/status /api/adb/wireless /api/adb/usb /api/adb/restart; do
  grep -q "\"$route\"" src/handlers/http_server.c
done

# 通知管理 API
for route in /api/notifications/webhook /api/notifications/webhook/test \
  /api/notifications/logs /api/notifications/rules /api/notifications/rules/*; do
  grep -q "\"$route\"" src/handlers/http_server.c
done

# 遥测上传 API
for route in /api/telemetry/config /api/telemetry/status /api/telemetry/test; do
  grep -q "\"$route\"" src/handlers/http_server.c
done

# 内置卡运营商 API
for route in /api/builtin-cards /api/builtin-cards/realname \
  /api/builtin-cards/switch /api/goform/proxy; do
  grep -q "\"$route\"" src/handlers/http_server.c
done

grep -q 'system/builtin_card_rules.c' src/Makefile
grep -q 'system/goform_client.c' src/Makefile
grep -q 'system/goform_proxy.c' src/Makefile
grep -q 'system/builtin_cards.c' src/Makefile
grep -q 'system/wifi_clients.c' src/Makefile
grep -q 'system/telemetry.c' src/Makefile

test -f web/src/components/BuiltinCardManager.vue
test -f web/src/components/DeviceManager.vue
test -f tools/start.sh
test -f tools/rj45_bridge_watch.sh
grep -q 'tools/start.sh' .github/workflows/build.yml
grep -q 'tools/rj45_bridge_watch.sh' .github/workflows/build.yml
