#!/usr/bin/env sh
set -o errexit
set -o nounset

HOST=${1:-127.0.0.1}
PORT=${2:-8211}

TOKEN=$(git rev-parse HEAD)

SERVER=~/.test_palantird.pid
CLIENT=~/.test_palantir.pid

SERVER_FLAGS="-a $TOKEN -d"
CLIENT_FLAGS="-a $TOKEN -c --halt"

main() {
    kill -9 $(cat $SERVER) 2> /dev/null || true
    kill -9 $(cat $CLIENT) 2> /dev/null || true

    ./build/palantir $SERVER_FLAGS $HOST $PORT & echo $! > $SERVER
    ./build/palantir $CLIENT_FLAGS $HOST $PORT & echo $! > $CLIENT
}

main "$@"
