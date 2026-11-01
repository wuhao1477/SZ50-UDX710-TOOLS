CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror
CPPFLAGS = -I../src/include/system

.PHONY: test clean device_profile_test builtin_card_rules_test goform_json_test \
        goform_async_test wifi_clients_test notification_test sms_parser_test \
        telemetry_codec_test ipv6_listener_check sz50_symbols_check

test: device_profile_test builtin_card_rules_test goform_json_test goform_async_test \
      wifi_clients_test notification_test sms_parser_test telemetry_codec_test \
      ipv6_listener_check sz50_symbols_check
	./device_profile_test
	./builtin_card_rules_test
	./goform_json_test
	./goform_async_test
	./wifi_clients_test
	./notification_test
	./sms_parser_test
	./telemetry_codec_test
	sh check_ipv6_listener.sh
	sh check_sz50_symbols.sh

ipv6_listener_check:
	sh check_ipv6_listener.sh

sz50_symbols_check:
	sh check_sz50_symbols.sh

device_profile_test: device_profile_test.c ../src/system/device_profile.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

builtin_card_rules_test: builtin_card_rules_test.c ../src/system/builtin_card_rules.c ../src/system/strict_json.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

goform_json_test: goform_json_test.c ../src/system/goform_client.c ../src/system/builtin_card_rules.c ../src/system/strict_json.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

goform_async_test: goform_async_test.c ../src/system/goform_async.c ../src/mongoose.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L -I../src -I../src/include/system -pthread -o $@ $^

wifi_clients_test: wifi_clients_test.c ../src/system/wifi_clients.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

notification_test: notification_test.c telemetry_link_stub.c ../src/system/notification_rules.c ../src/system/exec_utils.c
	$(CC) $(CFLAGS) -Wno-unused-parameter $(CPPFLAGS) -o $@ $^

sms_parser_test: sms_parser_test.c ../src/system/sms_parser.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

telemetry_codec_test: telemetry_codec_test.c ../src/system/telemetry_codec.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

clean:
	rm -f device_profile_test builtin_card_rules_test goform_json_test goform_async_test \
	      wifi_clients_test notification_test sms_parser_test telemetry_codec_test
