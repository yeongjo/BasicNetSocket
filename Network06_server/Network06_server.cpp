#include "../commonSocket.h"
struct SocketAndLine {
	SOCKET sock;
	size_t line;
};
int ReceiveData(SOCKET s, char* buf, int len, int flags, int line = 0) {
	char *ptr = buf;
	int received;
	for (int left = len; 0 < left; left -= received) {
		received = recv(s, ptr, left, flags);//매번바뀜
		ptr += received;
		if (line) PrintPercent(ptr - buf, len, received, line);
		if (received == SOCKET_ERROR)	return SOCKET_ERROR;// 오류발생시 리턴
		if (received == 0)				return 0;			// 정상적으로 받음
	}
	return -2;
}

DWORD ReceiveClientFile(LPVOID arg) {
	SOCKET clientSock = ((SocketAndLine*)arg)->sock;
	int line = ((SocketAndLine*)arg)->line;
	delete (SocketAndLine*)arg;
	char fileName[256]; ZeroMemory(fileName, 256);
	int totalBytes;
	ReceiveData(clientSock, fileName, 256, 0);
	ReceiveData(clientSock, (char *)&totalBytes, sizeof(totalBytes), 0);

	PrintAtLine(line+1, "받을 파일 이름 : %s", fileName);
	PrintAtLine(line + 2, "받을 파일 크기 : %d 바이트", totalBytes);

	FILE *fp = fopen(fileName, "w+b");	// 빈파일만듬
	char* buf = new char[totalBytes];	// 버퍼할당
	int remain = ReceiveData(clientSock, buf, totalBytes, 0, line+3);
	if (remain == SOCKET_ERROR) {
		PrintAtLine(line+4, "파일읽는데 오류");
		return false;
	}
	fwrite(buf, 1, totalBytes, fp);
	PrintAtLine(line+4, "파일을 성공적으로 받았습니다.");

	fclose(fp);
	closesocket(clientSock);
	return true;
}

void main(int argc, char *argv[]) {
	SOCKET listenSock = CreateSocket();
	InitializeCriticalSection(&cs);
	for (size_t i = 0;; i++) {
		auto sockAndLine = new SocketAndLine{ AccpetClient(listenSock, i*6), i*6};
		auto thread = CreateThread(0, 0, ReceiveClientFile, (LPVOID)sockAndLine, 0, 0);
		if (thread == NULL) closesocket(sockAndLine->sock);
		else CloseHandle(thread);
	}
	DeleteCriticalSection(&cs);
	closesocket(listenSock);
	WSACleanup();
}