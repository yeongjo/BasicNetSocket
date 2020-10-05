#include "../commonSocket.h"
int DataReceive(SOCKET s, char* buf, int len, int flags, bool isPrint = false) {
	char *ptr = buf;
	int received;
	for (int left = len; 0 < left; left -= received, ptr += received) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)	return SOCKET_ERROR;
		if (received == 0)				return len - left;
		if (isPrint) PrintPercent(ptr - buf, len, received);
	}
	return -2;
}

void main(int argc, char *argv[]) {
	SOCKET listenSock = CreateSocket();
	while (1) {
		SOCKET clientSock = AccpetClient(listenSock);
		char fileName[256]; ZeroMemory(fileName, 256);
		int totalBytes;
		DataReceive(clientSock, fileName, 256, 0);
		DataReceive(clientSock, (char *)&totalBytes, sizeof(totalBytes), 0);

		printf("받을 파일 이름 : %s\n", fileName);
		printf("받을 파일 크기 : %d 바이트\n", totalBytes);

		FILE *fp = fopen(fileName, "w+b");	// 빈파일만듬
		char* buf = new char[totalBytes];	// 버퍼할당
		int remain = DataReceive(clientSock, buf, totalBytes, 0, true);
		if (remain == SOCKET_ERROR) {
			printf("\n파일읽는데 오류\n");
			continue;
		}
		fwrite(buf, 1, remain, fp);
		printf("\n파일을 성공적으로 받았습니다.\n");

		fclose(fp);
		closesocket(clientSock);
	}
	closesocket(listenSock);
	WSACleanup();
}