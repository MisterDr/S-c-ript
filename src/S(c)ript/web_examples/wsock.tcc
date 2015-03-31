<html lang="en-US">
<head>
</head>
<body>
	<$ 

		//#include <winsock2.h>
		#include <windows.h>
		typedef struct WSAData {
			WORD	wVersion;
			WORD	wHighVersion;
			char	szDescription[257];
			char	szSystemStatus[129];
			unsigned short	iMaxSockets;
			unsigned short	iMaxUdpDg;
			char * 	lpVendorInfo;
		} WSADATA;

		//typedef int       (WSAAPI *zWSAStartup)            (IN WORD wVersionRequested, OUT LPWSADATA lpWSAData);
		typedef void (*FNPTR)(char*);  

		int main() {
			//void *addr;
			
			void (*pFn)(void) = 0; 
			
			long test = _CIacos(12);
			printf("%f\n\r", test);
			
			//MD5Init();
			
			WSADATA out;
			
			//printf("%i\n", sizeof(out));
			
			//HMODULE hWinsock;
			//hWinsock = LoadLibrary("wsock32.dll");
			
			//if (hWinsock)
			//{
				/*pFn = (void (__cdecl *)(char *))GetProcAddress(hWinsock, "WSAStartup");		
				pFn(); 
				FreeLibrary(hWinsock);*/
				//printf("%s\n", "Kitta");
			//}
			
			//addr(void);
			//int err = WSAGetLastError();	

			WSAStartup(MAKEWORD(2,2), &out);
			//INIT_SOCKETS_FOR_WINDOWS;
			printf("%s\n", "Yahoo");
			return 0;
		}
	$>
</body>
</html>