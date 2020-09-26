#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdio.h>

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	LocalFree(lpMsgBuf);
	exit(1);
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	MessageBox(NULL, "윈속 초기화 성공!", "알림", MB_OK);

	//SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	//if (tcp_sock == INVALID_SOCKET) err_quit("socket()");
	//MessageBox(NULL, "TCP 소켓 생성 성공!", "알림", MB_OK);

	printf("wVersion : %x.%x\n", wsa.wVersion & 0xff, (wsa.wVersion >> 8) & 0xff);
	printf("wHighVersion : %x.%x\n", wsa.wVersion & 0xff, (wsa.wVersion >> 8) & 0xff);
	printf("szDescription : %s\n", wsa.szDescription);
	printf("szSystemStatus : %s\n", wsa.szSystemStatus);

	//closesocket(tcp_sock);

	WSACleanup();

	system("pause");

	return 0;
}