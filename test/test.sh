#!/usr/bin/env sh
set -o errexit
set -o nounset

HOST=${1:-127.0.0.1}
PORT=${2:-8211}

SERVER=~/.ci_palantird.pid
CLIENT=~/.ci_palantir.pid

SERVER_FLAGS=-d
CLIENT_FLAGS=-c "--halt"

main() {
    kill -9 $(cat $SERVER) 2> /dev/null || true
    kill -9 $(cat $CLIENT) 2> /dev/null || true

    ./palantir $SERVER_FLAGS $HOST $PORT & echo $! > $SERVER
    ./palantir $CLIENT_FLAGS $HOST $PORT & echo $! > $CLIENT
}

main "$@"
