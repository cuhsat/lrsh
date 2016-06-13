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
static int ss = 0;
static int cs = 0;

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
extern int net_connect(const char *host, uint16_t port) {
    if (cs && close(cs) < 0) {
        return -1;
    }

    if ((cs = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    struct hostent *he;
    struct sockaddr_in addr;

    if ((he = gethostbyname2(host, AF_INET)) == NULL) {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    memcpy(&addr.sin_addr, he->h_addr, he->h_length);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &(addr.sin_addr)) <= 0) {
        return -1;
    }

    if (connect(cs, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    return 0;
}

/*
 * Net listen
 */
extern int net_listen(const char *host, uint16_t port) {
    if ((ss = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    struct hostent *he;
    struct sockaddr_in addr;

    if ((he = gethostbyname2(host, AF_INET)) == NULL) {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    memcpy(&addr.sin_addr, he->h_addr, he->h_length);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &(addr.sin_addr)) <= 0) {
        return -1;
    }

    if (bind(ss, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    if (listen(ss, 1) < 0) {
        return -1;
    }

    return 0;
}

/*
 * Net accept
 */
extern int net_accept() {
    if (cs && close(cs) < 0) {
        return -1;
    }

    if ((cs = accept(ss, NULL, NULL)) < 0) {
        return -1;
    }

    return 0;
}

/*
 * Net send
 */
extern int net_send(const char *data, size_t size) {
    uint32_t checksum = crc32(data, size);

    #if (defined(DEBUG) && (DEBUG == 1))
    printf(">> %.*s\n", (int)size, data);
    #endif

    if (write(cs, (const char *)&checksum, 4) < 4) {
        return -1;
    }

    if (write(cs, (const char *)&size, 4) < 4) {
        return -1;
    }

    if (write(cs, data, size) < size) {
        return -1;
    }

    return 0;
}

/*
 * Net recv
 */
extern int net_recv(char **data, size_t *size) {
    uint32_t checksum;

    if (read(cs, (char *)&checksum, 4) < 4) {
        return -1;
    }

    if (read(cs, (char *)size, 4) < 4) {
        return -1;
    }

    if ((buffer = (char *)realloc(buffer, *size)) == NULL) {
        return -1;
    }

    if (read(cs, buffer, *size) < *size) {
        return -1;
    }

    if (checksum != crc32(buffer, *size)) {
        return -1;
    }

    *data = buffer;

    #if (defined(DEBUG) && (DEBUG == 1))
    printf("<< %.*s\n", (int)*size, *data);
    #endif

    return 0;
}

/*
 * Net exit
 */
extern int net_exit() {
    free(buffer);

    if (ss && close(ss) < 0) {
        return -1;
    }

    if (cs && close(cs) < 0) {
        return -1;
    }

    return 0;
}
