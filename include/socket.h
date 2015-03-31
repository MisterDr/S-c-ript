/** @file net.h */

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

/* Header for generic net.h */
#ifndef NET_H_
#define NET_H_

#ifdef __GNUC__
    #define TCC_INLINE static __inline__
    #define TCC_EXPORT
#else
    #define TCC_INLINE static
    #ifdef TCC_STATIC_BUILD
        #define TCC_EXPORT
    #elif defined(TCC_DLL_BUILD)
        #define TCC_EXPORT __declspec(dllexport)
    #else
        #define TCC_EXPORT __declspec(dllimport)
    #endif
#endif

#ifdef __cplusplus
#define TCC_EXTERN_C_START extern "C" {
#define TCC_EXTERN_C_END }
#else
#define TCC_EXTERN_C_START
#define TCC_EXTERN_C_END
#endif

#define OK 0
#define ERROR -1

#define DEFAULT_BUFLEN 512


typedef enum conn_error_t {
    CONN_SUCCESS = 0,  /**< Connection success! */
    CONN_NO_SOCKET,    /**< Could not create a socket. */
    CONN_FAIL,         /**< An error occured while calling connect(). */
    CONN_ADDR_FAIL,    /**< An error occured while calling getaddrinfo(). */
    CONN_NOT_MASTER,   /**< Warning: connected to a non-master node (read-only). */
    CONN_BAD_SET_NAME, /**< Given rs name doesn't match this replica set. */
    CONN_NO_PRIMARY,   /**< Can't find primary in replica set. Connection closed. */

    IO_ERROR,          /**< An error occurred while reading or writing on the socket. */
    READ_SIZE_ERROR,   /**< The response is not the expected length. */
} conn_error_t;

typedef struct sdata
{
	char* data;
	int size;
} sdata;

typedef struct host_port {
    char host[255];
    int port;
    struct host_port *next;
} host_port;

typedef struct conn_info {
    host_port *primary;  /**< Primary connection info. */
    int sock;                  /**< Socket file descriptor. */
    int flags;                 /**< Flags on this connection object. */
    int conn_timeout_ms;       /**< Connection timeout in milliseconds. */
    int op_timeout_ms;         /**< Read and write timeout in milliseconds. */
    int connected;     /**< Connection status. */

    conn_error_t err;         /**< Most recent driver error code. */
    char *errstr;              /**< String version of most recent driver error code. */
    int lasterrcode;           /**< getlasterror given by the server on calls. */
    char *lasterrstr;          /**< getlasterror string generated by server. */
} conn_info;

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <ws2tcpip.h>  // send,recv,socklen_t etc
        #include <wspiapi.h>   // addrinfo
    #else
        #include <windows.h>
        #include <winsock.h>
        typedef int socklen_t;
    #endif
#define close_socket(sock) ( closesocket(sock) )
#else
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#define close_socket(sock) ( close(sock) )
#endif

#ifndef _WIN32
#include <unistd.h>
#endif

#if defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE) || _POSIX_C_SOURCE >= 1
#define USE_GETADDRINFO
#endif

TCC_EXTERN_C_START

/* This is a no-op in the generic implementation. */
int set_socket_op_timeout( conn_info *conn, int millis );
int read_socket( conn_info *conn, void *buf, int len );
int write_socket( conn_info *conn, const void *buf, int len );
int socket_connect( conn_info *conn, const char *host, int port );

/* Initialize the socket services */
TCC_EXPORT int sock_init();
TCC_EXTERN_C_END
#endif
