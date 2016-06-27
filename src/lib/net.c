/*
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

static char *buffer = NULL;
static int s_socket = 0;
static int c_socket = 0;

/*
 * Bitwise CRC32
 */ 
static uint32_t crc32(const char *data, size_t size) {
    uint32_t crc = 0;

    while (size--) {
        crc ^= *data++;

        for (uint8_t i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
    }

    return crc;
}

/*
 * Net connect
 */
extern int net_connect(host_t host) {
    if (c_socket && close(c_socket) < 0) {
        return -1;
    }

    if ((c_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    struct hostent *he;
    struct sockaddr_in addr;

    if ((he = gethostbyname2(host.name, AF_INET)) == NULL) {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    memcpy(&addr.sin_addr, he->h_addr, he->h_length);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(host.port);

    if (inet_pton(AF_INET, host.name, &(addr.sin_addr)) <= 0) {
        return -1;
    }

    if (connect(c_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    return 0;
}

/*
 * Net listen
 */
extern int net_listen(host_t host) {
    if ((s_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    struct hostent *he;
    struct sockaddr_in addr;

    if ((he = gethostbyname2(host.name, AF_INET)) == NULL) {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    memcpy(&addr.sin_addr, he->h_addr, he->h_length);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(host.port);

    if (inet_pton(AF_INET, host.name, &(addr.sin_addr)) <= 0) {
        return -1;
    }

    if (bind(s_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    if (listen(s_socket, 1) < 0) {
        return -1;
    }

    return 0;
}

/*
 * Net accept
 */
extern int net_accept() {
    if (c_socket && close(c_socket) < 0) {
        return -1;
    }

    if ((c_socket = accept(s_socket, NULL, NULL)) < 0) {
        return -1;
    }

    return 0;
}

/*
 * Net send
 */
extern int net_send(frame_t frame) {
    uint32_t checksum = crc32(frame.data, frame.size), size = frame.size;

    #if (defined(DEBUG) && (DEBUG == 1))
    printf(">> %.*s\n", (int)(frame.size), frame.data);
    #endif

    if (write(c_socket, (const char *)&checksum, 4) < 4) {
        return -1;
    }

    if (write(c_socket, (const char *)&size, 4) < 4) {
        return -1;
    }

    if (write(c_socket, frame.data, frame.size) < frame.size) {
        return -1;
    }

    return 0;
}

/*
 * Net recv
 */
extern int net_recv(frame_t *frame) {
    uint32_t checksum, size;

    if (read(c_socket, (char *)&checksum, 4) < 4) {
        return -1;
    }

    if (read(c_socket, (char *)&size, 4) < 4) {
        return -1;
    }

    if ((buffer = (char *)realloc(buffer, size)) == NULL) {
        return -1;
    }

    if (read(c_socket, buffer, size) < size) {
        return -1;
    }

    if (checksum != crc32(buffer, size)) {
        return -1;
    }

    frame->data = buffer;
    frame->size = size;

    #if (defined(DEBUG) && (DEBUG == 1))
    printf("<< %.*s\n", (int)(frame->size), frame->data);
    #endif

    return 0;
}

/*
 * Net exit
 */
extern int net_exit() {
    free(buffer);

    if (s_socket && close(s_socket) < 0) {
        return -1;
    }

    if (c_socket && close(c_socket) < 0) {
        return -1;
    }

    return 0;
}
