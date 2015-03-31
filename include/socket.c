/* net.c */

/*    Copyright 2009-2011 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/* Derivative of Implementation for generic version of net.h */

/*
	Changed by Djenad Razic	
*/

#include "env.h"
#include <errno.h>
#include <string.h>

#ifndef NI_MAXSERV
# define NI_MAXSERV 32
#endif

int write_socket( conn_info *conn, const void *buf, int len ) {
    const char *cbuf = buf;
#ifdef _WIN32
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif

    while ( len ) {
        int sent = send( conn->sock, cbuf, len, flags );
        if ( sent == -1 ) {
            if (errno == EPIPE) 
                conn->connected = 0;
            conn->err = IO_ERROR;
            return ERROR;
        }
        cbuf += sent;
        len -= sent;
    }

    return OK;
}

int read_socket( conn_info *conn, void *buf, int len ) {
    char *cbuf = buf;
    while ( len ) {		
        int sent = recv( conn->sock, cbuf, len, 0 );
		
		printf("receiving len %i\n", sent);
		
        if ( sent == 0 || sent == -1 ) {
            conn->err = IO_ERROR;
            return ERROR;
        }
        cbuf += sent;
        len -= sent;
    }

    return OK;
}

sdata* read_socket_all( conn_info *conn, int *len) {
    char* recvbuf = (char*)malloc(DEFAULT_BUFLEN);
	char* buf = NULL; 
	char* retbuf;
	int size = 0;
	static int sent = 0;
    do {
        int sent = recv( conn->sock, recvbuf, DEFAULT_BUFLEN, 0 );
		size += sent;

		buf = (char*)realloc(buf, size);
		buf = memcpy(buf += (size-sent), recvbuf, sent);
		
    } while (sent > 0);
	
	sdata* ret = (sdata*)malloc(sizeof(sdata));
	
	ret->size = size;
	ret->data = buf;
		
    return ret;
}

/* This is a no-op in the generic implementation. */
int set_socket_op_timeout( conn_info *conn, int millis ) {
    return OK;
}

#ifdef USE_GETADDRINFO
int socket_connect( conn_info *conn, const char *host, int port ) {
    char port_str[NI_MAXSERV];
    int status;

    struct addrinfo ai_hints;
    struct addrinfo *ai_list = NULL;
    struct addrinfo *ai_ptr = NULL;

    conn->sock = 0;
    conn->connected = 0;

    bson_sprintf( port_str, "%d", port );

    memset( &ai_hints, 0, sizeof( ai_hints ) );
#ifdef AI_ADDRCONFIG
    ai_hints.ai_flags = AI_ADDRCONFIG;
#endif
    ai_hints.ai_family = AF_UNSPEC;
    ai_hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo( host, port_str, &ai_hints, &ai_list );
    if ( status != 0 ) {
        bson_errprintf( "getaddrinfo failed: %s", gai_strerror( status ) );
        conn->err = CONN_ADDR_FAIL;
        return ERROR;
    }

    for ( ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next ) {
        conn->sock = socket( ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol );
        if ( conn->sock < 0 ) {
            conn->sock = 0;
            continue;
        }

        status = connect( conn->sock, ai_ptr->ai_addr, ai_ptr->ai_addrlen );
        if ( status != 0 ) {
            close_socket( conn->sock );
            conn->sock = 0;
            continue;
        }

        if ( ai_ptr->ai_protocol == IPPROTO_TCP ) {
            int flag = 1;

            setsockopt( conn->sock, IPPROTO_TCP, TCP_NODELAY,
                        ( void * ) &flag, sizeof( flag ) );
            if ( conn->op_timeout_ms > 0 )
                set_socket_op_timeout( conn, conn->op_timeout_ms );
        }

        conn->connected = 1;
        break;
    }

    freeaddrinfo( ai_list );

    if ( ! conn->connected ) {
        conn->err = CONN_FAIL;
        return ERROR;
    }

    return OK;
}
#else
int socket_connect( conn_info *conn, const char *host, int port ) {
    struct sockaddr_in sa;
    socklen_t addressSize;
    int flag = 1;

    if ( ( conn->sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
        conn->sock = 0;
        conn->err = CONN_NO_SOCKET;
        return ERROR;
    }

    memset( sa.sin_zero , 0 , sizeof( sa.sin_zero ) );
    sa.sin_family = AF_INET;
    sa.sin_port = htons( port );
    sa.sin_addr.s_addr = inet_addr( host );
    addressSize = sizeof( sa );

    if ( connect( conn->sock, ( struct sockaddr * )&sa, addressSize ) == -1 ) {
        close_socket( conn->sock );
        conn->connected = 0;
        conn->sock = 0;
        conn->err = CONN_FAIL;
        return ERROR;
    }

    setsockopt( conn->sock, IPPROTO_TCP, TCP_NODELAY, ( char * ) &flag, sizeof( flag ) );

    if( conn->op_timeout_ms > 0 )
        set_socket_op_timeout( conn, conn->op_timeout_ms );

    conn->connected = 1;

    return OK;
}

#endif

TCC_EXPORT int sock_init() {

#if defined(_WIN32)
    WSADATA wsaData;
    WORD wVers;
#elif defined(SIGPIPE)
    struct sigaction act;
#endif

    static int called_once;
    static int retval;
    if (called_once) return retval;
    called_once = 1;

#if defined(_WIN32)
    wVers = MAKEWORD(1, 1);
    retval = (WSAStartup(wVers, &wsaData) == 0);
#elif defined(MACINTOSH)
    GUSISetup(GUSIwithInternetSockets);
    retval = 1;
#elif defined(SIGPIPE)
    retval = 1;
    if (sigaction(SIGPIPE, (struct sigaction *)NULL, &act) < 0)
        retval = 0;
    else if (act.sa_handler == SIG_DFL) {
        act.sa_handler = SIG_IGN;
        if (sigaction(SIGPIPE, &act, (struct sigaction *)NULL) < 0)
            retval = 0;
    }
#endif
    return retval;
}
