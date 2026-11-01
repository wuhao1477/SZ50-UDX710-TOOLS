#!/bin/sh

set -eu

BASE_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
SERVER="$BASE_DIR/server"
WATCHER="$BASE_DIR/rj45_bridge_watch.sh"
SERVER_PID="/tmp/udx710-tools.pid"
WATCHER_PID="/tmp/udx710-rj45-daemon.pid"

if [ ! -x "$SERVER" ]; then
    echo "server not found or not executable: $SERVER" >&2
    exit 1
fi

if ! command -v start-stop-daemon >/dev/null 2>&1; then
    echo "start-stop-daemon is required on the target device" >&2
    exit 1
fi

cd "$BASE_DIR"

# pid 文件里的进程是否就是"当前这个" server 二进制。
# 覆盖安装后旧进程的 /proc/<pid>/exe 会变成 "<path> (deleted)"，
# 这种残留进程必须清掉，否则会误判为"已在运行"而跳过启动新版本。
server_pid_matches_binary() {
    pid=$1
    exe=$(readlink "/proc/$pid/exe" 2>/dev/null || true)
    [ "$exe" = "$SERVER" ]
}

server_running=0
if [ -r "$SERVER_PID" ]; then
    pid=$(cat "$SERVER_PID" 2>/dev/null || true)
    if [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null; then
        if server_pid_matches_binary "$pid"; then
            server_running=1
        else
            exe=$(readlink "/proc/$pid/exe" 2>/dev/null || echo unknown)
            echo "清理残留 server 进程 $pid (exe=$exe)" >&2
            kill "$pid" 2>/dev/null || true
            sleep 1
            kill -9 "$pid" 2>/dev/null || true
            rm -f "$SERVER_PID"
        fi
    fi
fi
if [ "$server_running" -eq 0 ]; then
    start-stop-daemon -S -b -m -p "$SERVER_PID" -x "$SERVER" -- 6677
fi

if [ -x "$WATCHER" ]; then
    start-stop-daemon -S -b -m -p "$WATCHER_PID" -a /bin/sh -- \
        -c "exec '$WATCHER'"
fi

echo "UDX710 tools started on port 6677"
