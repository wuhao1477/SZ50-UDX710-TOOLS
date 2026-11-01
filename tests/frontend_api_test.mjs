import assert from 'node:assert/strict'
import * as api from '../web/src/composables/useApi.js'

// 基础与内置卡
assert.equal(typeof api.getCapabilities, 'function')
assert.equal(typeof api.getBuiltinCards, 'function')
assert.equal(typeof api.checkBuiltinCardRealName, 'function')
assert.equal(typeof api.switchBuiltinCard, 'function')
assert.equal(typeof api.goformProxy, 'function')
assert.equal(typeof api.fetchHealth, 'function')

// 遥测上传
assert.equal(typeof api.getTelemetryConfig, 'function')
assert.equal(typeof api.saveTelemetryConfig, 'function')
assert.equal(typeof api.getTelemetryStatus, 'function')
assert.equal(typeof api.testTelemetry, 'function')

// WiFi 与接入设备
assert.equal(typeof api.getWifiStatus, 'function')
assert.equal(typeof api.setWifiConfig, 'function')
assert.equal(typeof api.getWifiClients, 'function')
assert.equal(typeof api.getWifiBlacklist, 'function')
assert.equal(typeof api.getWifiWhitelist, 'function')
assert.equal(typeof api.lookupMacVendor, 'function')

// ADB 与通知
assert.equal(typeof api.getAdbStatus, 'function')
assert.equal(typeof api.setAdbWireless, 'function')
assert.equal(typeof api.setAdbUsb, 'function')
assert.equal(typeof api.restartAdb, 'function')
assert.equal(typeof api.getNotificationWebhook, 'function')
assert.equal(typeof api.saveNotificationWebhook, 'function')
assert.equal(typeof api.testNotificationWebhook, 'function')
assert.equal(typeof api.getNotificationRules, 'function')
assert.equal(typeof api.addNotificationRule, 'function')
assert.equal(typeof api.updateNotificationRule, 'function')
assert.equal(typeof api.deleteNotificationRule, 'function')
assert.equal(typeof api.getNotificationLogs, 'function')
