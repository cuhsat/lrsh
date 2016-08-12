#!/usr/bin/env sh
set -o errexit
set -o nounset

HOST=127.0.0.1
PORT=8211

main() {
    killall palantir || true
    ./palantir -d $HOST $PORT && \
    echo "-- halt" | ./palantir $HOST $PORT
}

main "$@"
