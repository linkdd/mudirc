#pragma once

#include <preludec/defs.h>
#include <preludec/mem/str.h>

#include <preludec/net/conn.h>


void netlib_init  (void);
void netlib_deinit(void);

RESULT(conn, str) netlib_create_tcp_client(const char *host, const char *port);
