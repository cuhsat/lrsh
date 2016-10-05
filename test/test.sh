#!/usr/bin/env sh
set -o errexit
set -o nounset

HOST=${1:-127.0.0.1}
PORT=${2:-8211}

main() {
    echo "Testing on $HOST:$PORT ..."
    killall palantir 2> /dev/null || true
    ./palantir -d $HOST $PORT && \
    echo "-- halt" | ./palantir $HOST $PORT
}

main "$@"
