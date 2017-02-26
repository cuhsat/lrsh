#!/usr/bin/env bash
set -o errexit
set -o nounset

HOST=${1:-127.0.0.1}
PORT=${2:-8211}

TOKEN=$(git rev-parse HEAD)

SERVER_FLAGS="-a $TOKEN -d"
CLIENT_FLAGS="-a $TOKEN"

main() {
    killall palantir 2> /dev/null || true

    ./palantir $SERVER_FLAGS $HOST $PORT
    ./palantir $CLIENT_FLAGS $HOST $PORT <<< '--halt'
}

main "$@"
