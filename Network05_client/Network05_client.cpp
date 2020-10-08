#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>

using namespace std;
#define BUFSIZE 4096
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char *argv[]) {

	// 윈속 객체 선언 및 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// 클라이언트 소켓 초기화
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	// 클라이언트 소켓을 서버에 연결
	SOCKADDR_IN servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9000);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// 서버에 연결 시도
	connect(sock, (SOCKADDR *)&servaddr, sizeof(servaddr));
	printf("서버에 접속 성공 : IP = %s, Port = %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

	char filename[256];
	ZeroMemory(filename, 256);
	sprintf(filename, argv[1]);

	FILE *fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	send(sock, filename, 256, 0);

	int totalbytes = ftell(fp);
	send(sock, (char *)&totalbytes, sizeof(totalbytes), 0);
	rewind(fp);

	char *buf = new char[totalbytes];
	int numtotal = 0;

	int numread = (int)fread(buf, 1, totalbytes, fp);
	//numread랑 totalbytes랑 같음
	//cout << numread << " " << totalbytes << endl;
	char *bufOff = buf;
	int remainBytes = numread;
	while (remainBytes > 0) {
		remainBytes = numread - BUFSIZE;
		int bufSize = BUFSIZE;
		if (remainBytes < 0)
			bufSize = remainBytes;
		cout << "bufsize: " << bufSize << endl;

		if (numread > 0) {
			// 나눠서 send해야할듯
			send(sock, bufOff, bufSize, 0); // 길이제한이 4096인가봄
			bufOff += bufSize;
			numtotal += bufSize;
			printf("\r%d 퍼센트", (int)(numtotal / (float)totalbytes * 100));
		}
	}
	printf("\n총 %d 바이트 파일 전송을 완료했습니다.\n", numtotal);
	fclose(fp);

	closesocket(sock);
	WSACleanup();
	return 0;
}
