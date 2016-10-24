/**
 * Copyright (c) 2016 Christian Uhsat <christian@uhsat.de>
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

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

static uint32_t auth = 0;
static char *buffer = NULL;
static int server = 0;
static int client = 0;

/**
 * CRC32 (bitwise)
 * @param data the data address
 * @param size the data size
 * @param auth the authentication
 * @return crc sum
 */
static uint32_t crc32(const char *data, size_t size, uint32_t auth) {
    uint32_t sum = auth;

    while (size--) {
        sum ^= *data++;

        for (uint8_t i = 0; i < 8; i++) {
            sum = (sum >> 1) ^ ((sum & 1) ? 0xEDB88320 : 0);
        }
    }

    return sum;
}

/**
 * Fill address
 * @param host the host address
 * @param addr the address
 * @return success
 */
static int address(host_t *host, struct sockaddr_in *addr) {
    struct hostent *he;

    if ((he = gethostbyname2(host->name, AF_INET)) == NULL) {
        return -1;
    }

    memset(addr, 0, sizeof(*addr));
    memcpy(&(addr->sin_addr), he->h_addr, he->h_length);

    addr->sin_family = AF_INET;
    addr->sin_port = htons(host->port);

    if (inet_pton(AF_INET, host->name, &(addr->sin_addr)) <= 0) {
        return -1;
    }

    return 0;
}

/**
 * Terminate socket
 * @param fd the socket
 * @return success
 */
static int terminate(int fd) {
    if (fd > 0) {
        socklen_t len = sizeof(errno);

        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &errno, &len) < 0) {
            return -1;
        }

        if ((shutdown(fd, SHUT_RDWR) < 0) && (errno != ENOTCONN)) {
            return -1;
        }

        if (close(fd) < 0) {
            return -1;
        }
    }

    return 0;
}

/**
 * Net authentication
 * @param token the token
 * @return success
 */
extern int net_auth(const char *token) {
    if (strlen(token) > 0) {
        auth = crc32(token, strnlen(token, MAX_TOKEN), 0);
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

    if (write(client, (const char *)&checksum, 4) < 4) {
        return -1;
    }

    if (write(client, (const char *)&frame->size, 4) < 4) {
        return -1;
    }

    if (write(client, frame->data, frame->size) < frame->size) {
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

    if (read(client, (char *)&checksum, 4) < 4) {
        return -1;
    }

    if (read(client, (char *)&size, 4) < 4) {
        return -1;
    }

    if ((buffer = (char *)realloc(buffer, size)) == NULL) {
        return -1;
    }

    if (read(client, buffer, size) < size) {
        return -1;
    }

    if (checksum != crc32(buffer, size, auth)) {
        errno = ((auth > 0) ? EACCES : EBADMSG);
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
