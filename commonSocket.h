#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cctype>

#define SERVERPORT 9000
#define BUFSIZE    4096

using namespace std;

// 소켓 함수 오류 출력 후 종료
void err_quit(const char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 첨파라미터 bind, connect 사용
// addr: 주소 inet_addr("127.0.0.1")
SOCKET CreateSocket(ULONG addr = htonl(INADDR_ANY)) {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKADDR_IN servaddr;
	ZeroMemory(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9000);
	servaddr.sin_addr.s_addr = addr;

	// TCP : SOCK_STREAM
	// UDP : SOCK_DGRAM
	auto s = socket(AF_INET, SOCK_STREAM, 0);
	if (addr == htonl(INADDR_ANY)) {
		bind(s, (SOCKADDR *)&servaddr, sizeof(servaddr)); // 접속을 기다리는 소켓
		printf("서버 생성 : IP = %s, Port = %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
		listen(s, SOMAXCONN);
	} else {
		connect(s, (SOCKADDR *)&servaddr, sizeof(servaddr)); // bind 함수의 역할도 함께 수행
		printf("서버 접속 : IP = %s, Port = %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
	}

	return s;
}

CRITICAL_SECTION cs;

void PrintAtLine(short line, const char* chr, ...) {
	EnterCriticalSection(&cs);
	va_list ap;
	char buf[128];
	va_start(ap, chr);
	vsprintf(buf, chr, ap);
	va_end(ap);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, line });
	fprintf(stdout, "%d: ", line);
	fprintf(stdout, "%s", buf);
	LeaveCriticalSection(&cs);
}

void PrintPercent(float current, float total, int bufSize) {
	printf("\r%.1f 퍼센트 \t bufsize: %d", (current / (float)total * 100), bufSize);
}

void PrintPercent(float current, float total, int bufSize, int line) {
	PrintAtLine(line, "%.1f 퍼센트 \t bufsize: %d", (current / (float)total * 100), bufSize);
}



SOCKET AccpetClient(SOCKET listen_sock, int printLine = -1) {
	SOCKADDR_IN clientaddr; int addrlen = sizeof(clientaddr);
	SOCKET client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);

	if(printLine == -1)
		printf("클라이언트 접속 : IP = %s, Port = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	else
		PrintAtLine(printLine, "클라이언트 접속 : IP = %s, Port = %d", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	return client_sock;

}

#pragma comment(lib, "comctl32.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_DEFAULTPROGRESSCTRL	401
#define ID_SMOOTHPROGRESSCTRL	402
#define ID_VERTICALPROGRESSCTRL	403

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}