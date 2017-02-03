/**
 * Copyright (c) 2016-2017 Christian Uhsat <christian@uhsat.de>
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
#ifndef LIB_NET_H
#define LIB_NET_H

#include <stddef.h>
#include <stdint.h>

#ifndef ARG_MAX
#define ARG_MAX 255
#endif

#define MAX_TOKEN ARG_MAX

typedef struct {
    const char *name;
    const uint16_t port;
} host_t;

typedef struct {
    char *data;
    size_t size;
} frame_t;

extern int net_auth(const char *token);
extern int net_connect(host_t *addr);
extern int net_listen(host_t *addr);
extern int net_accept();
extern int net_send(frame_t *frame);
extern int net_recv(frame_t *frame);
extern int net_exit();

#endif // LIB_NET_H
