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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define FRAME_MAX 1024

static char *buffer;
static int list = 0;
static int sock = 0;

/*
 * CRC32 bitwise
 */ 
static uint32_t checksum(const char *data, size_t size) {
    uint32_t crc = 0;

    while (size--) {
        crc ^= *data++;

        for (uint8_t i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
    }

    return crc;
}

extern int net_start() {
    if ((buffer = (char *)calloc(4, sizeof(char))) == NULL) {
        return -1;
    }

    return 0;
}

extern int net_connect(const char *host, uint16_t port) {
    if (sock && close(sock) < 0) {
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    return 0;
}

extern int net_listen(const char *host, uint16_t port) {
    if ((list = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        return -1;
    }

    if (bind(list, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    if (listen(list, 1) < 0) {
        return -1;
    }

    return 0;
}

extern int net_accept() {
    if (sock && close(sock) < 0) {
        return -1;
    }

    if ((sock = accept(list, (struct sockaddr*)NULL, NULL)) < 0) {
        return -1;
    }

    return 0;
}

extern int net_send(const char *data, size_t size) {
    uint32_t crc = checksum(data, size);

    if (send(sock, (const char *)&size, 4, 0) < 0) {
        return -1;
    }

    if (send(sock, (const char *)&crc, 4, 0) < 0) {
        return -1;
    }

    if (send(sock, data, size, 0) < 0) {
        return -1;
    }

    return 0;
}

extern int net_recv(char **data, size_t *size) {
    uint32_t crc;
    char frame[FRAME_MAX];
    int frame_size, frame_last;

    if (recv(sock, (char *)size, 4, 0) < 0) {
        return -1;
    }

    frame_last = *size;

    if ((buffer = (char *)realloc(buffer, *size)) == NULL) {
        return -1;
    }

    if (recv(sock, (char *)&crc, 4, 0) < 0) {
        return -1;
    }

    do {
        frame_size = recv(sock, frame, MIN(FRAME_MAX, frame_last), 0);

        if (frame_size < 0) {
            return -1;
        }

        memcpy(buffer + (*size - frame_last), frame, frame_size);
    } while (frame_size && (frame_last -= frame_size));

    if (crc != checksum(buffer, *size)) {
        return -1;
    }

    *data = buffer;

    return 0;
}

extern int net_close() {
    free(buffer);

    if (list && close(list) < 0) {
        return -1;
    }

    if (sock && close(sock) < 0) {
        return -1;
    }

    return 0;
}
