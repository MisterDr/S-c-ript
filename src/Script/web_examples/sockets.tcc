<html lang="en-US">
<head>
</head>
<body>
	<$ 

		#include <bstrlib.h>
		#include <bstrlib.c>
		#include <socket.h>
		#include <socket.c>
		#include <windows.h>

		int main() {
			conn_info* conn = (conn_info*)malloc(sizeof(conn_info));
			sock_init();
			if (socket_connect(conn, "127.0.0.1", 80) == OK)
			{
				if (conn->connected == 1)
				{
					int* size;
					bstring b,req;
					req = bfromcstr("De Nada\r\n");
					//req = bfromcstr("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\nUser-Agent: Kita\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Encoding: gzip,deflate,sdch\r\nAccept-Language: en-US,en;q=0.8\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n\r\n");
					//req = bfromcstr("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.64 Safari/537.11\r\n");
					
					printf("\nConnected\nTrying to write some data\n");
					write_socket(conn, req->data, req->slen);
					
					printf("Trying to read some data\n\n");
					sdata* bufdata = read_socket_all(conn, size);
					
					//printf("Buffer %s\n", buf);
					
					b = bfromcstralloc(bufdata->size, bufdata->data);
					
					printf("Size of the data: %i\n\n", bufdata->size);
					printf("%s\n", b->data);
					
					bdestroy(b);
					free(bufdata->data);
					free(bufdata);
				}
				else
					printf("Not connected\n");		
			}
			
			
			free(conn);
		}
	$>
</body>
</html>