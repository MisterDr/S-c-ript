//TODO: Insert mime types as necessary

#define TCC_HEADER_HTML printf("%s","HTTP/1.0 200 OK\nContent-Type:text/html;charset=iso-8859-1\n\n");
#define TCC_HEADER_PLAIN printf("%s","HTTP/1.0 200 OK\nContent-Type:text/plain;charset=us-ascii\n\n");
#define TCC_HEADER_JSON printf("%s","HTTP/1.0 200 OK\nContent-Type:application/json;charset=iso-8859-1\n\n");