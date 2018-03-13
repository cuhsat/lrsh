/**
 * Copyright (c) 2016-2018 Christian Uhsat <christian@uhsat.de>
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "net.h"
#include "crc.h"

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef POSIX
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#ifdef WINNT
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifdef POSIX
typedef int socket_t;
#endif

#ifdef WINNT
typedef SOCKET socket_t;
#endif

static uint32_t auth = 0;
static char *buffer = NULL;
static socket_t server = 0;
static socket_t client = 0;

/**
 * Fill address
 * @param host the host
 * @param addr the address
 * @return success
 */
static int address(host_t *host, struct sockaddr_in *addr) {
    struct hostent *he;

    if ((he = gethostbyname(host->name)) == NULL) {
        return -1;
    }

    memset(addr, 0, sizeof(*addr));
    memcpy(&(addr->sin_addr), he->h_addr_list[0], (size_t)he->h_length);

    addr->sin_family = AF_INET;
    addr->sin_port = htons(host->port);

    if ((addr->sin_addr.s_addr = inet_addr(host->name)) == -1) {
        return -1;
    }

    return 0;
}

/**
 * Terminate socket
 * @param fd the socket
 * @return success
 */
static int terminate(socket_t fd) {
    if (fd > 0) {
        socklen_t len = sizeof(errno);

        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)&errno, &len) < 0) {
            return -1;
        }

#ifdef POSIX

        if (close(fd) < 0) {
            return -1;
        }

#endif // POSIX

#ifdef WINNT

        if (closesocket(fd) != 0) {
            return -1;
        }

#endif // WINNT
    }

    return 0;
}

/**
 * Net authentication
 * @param token the token
 * @return success
 */
extern int net_auth(const char *token) {
    size_t size = strlen(token);

    if (size > 0) {
        auth = crc32(token, size, 0);
    } else {
        auth = 0;
    }

    return 0;
}

/**
 * Net connect
 * @param host the host address
 * @return success
 */
extern int net_connect(host_t *host) {
    if (terminate(client) < 0) {
        return -1;
    }

    if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    struct sockaddr_in addr;

    if (address(host, &addr) < 0) {
        return -1;
    }

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    return 0;
}

/**
 * Net listen
 * @param host the host address
 * @return success
 */
extern int net_listen(host_t *host) {
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    struct sockaddr_in addr;

    if (address(host, &addr) < 0) {
        return -1;
    }

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
        return -1;
    }

    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    if (listen(server, 1) < 0) {
        return -1;
    }

    return 0;
}

/**
 * Net accept
 * @return success
 */
extern int net_accept() {
    if (terminate(client) < 0) {
        return -1;
    }

    if ((client = accept(server, NULL, NULL)) < 0) {
        return -1;
    }

    return 0;
}

/**
 * Net send
 * @param frame the frame address
 * @return success
 */
extern int net_send(frame_t *frame) {
    uint32_t checksum = crc32(frame->data, frame->size, auth);

#if (defined(DEBUG) && (DEBUG == 1))

    fprintf(stderr, ">> %.*s\n", (int)(frame->size), frame->data);

#endif

    if (send(client, (const char *)&checksum, 4, 0) < 4) {
        return -1;
    }

    if (send(client, (const char *)&frame->size, 4, 0) < 4) {
        return -1;
    }

    if (send(client, frame->data, frame->size, 0) < frame->size) {
        return -1;
    }

    return 0;
}

/**
 * Net recv
 * @param frame the frame address
 * @return success
 */
extern int net_recv(frame_t *frame) {
    uint32_t checksum, size;

    if (recv(client, (char *)&checksum, 4, 0) < 4) {
        return -1;
    }

    if (recv(client, (char *)&size, 4, 0) < 4) {
        return -1;
    }

    if ((buffer = (char *)realloc(buffer, size)) == NULL) {
        return -1;
    }

    if (recv(client, buffer, size, 0) < size) {
        return -1;
    }

    if (checksum != crc32(buffer, size, auth)) {
        errno = ((auth > 0) ? EACCES : EILSEQ);
        return -1;
    }

    frame->data = buffer;
    frame->size = size;

#if (defined(DEBUG) && (DEBUG == 1))

    fprintf(stderr, "<< %.*s\n", (int)(frame->size), frame->data);

#endif

    return 0;
}

/**
 * Net exit
 * @return success
 */
extern int net_exit() {
    free(buffer);

    if (terminate(server) < 0) {
        return -1;
    }

    if (terminate(client) < 0) {
        return -1;
    }

    return 0;
}
