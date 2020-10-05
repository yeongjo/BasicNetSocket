#include "../commonSocket.h"
void main(int argc, char *argv[]) {
	SOCKET sock = CreateSocket(inet_addr("127.0.0.1"));

	char filename[256]; ZeroMemory(filename, 256);
	sprintf(filename, argv[1]);
	FILE *fp = fopen(filename, "rb"); 
	fseek(fp, 0, SEEK_END);
	int totalbytes = ftell(fp); rewind(fp);

	send(sock, filename, 256, 0);							// 파일이름
	send(sock, (char *)&totalbytes, sizeof(totalbytes), 0); // 보낼파일크기

	char *buf = new char[totalbytes];
	fread(buf, 1, totalbytes, fp);
	int sendbytes = 0;

	for (int remain = totalbytes; 0 < remain; remain = remain - BUFSIZE) {
		int bufSize = BUFSIZE;		// 기본 4096만큼 보내기
		if (remain - BUFSIZE < 0)	// 남아있는 버퍼가 적으면
			bufSize = remain;		// 남아있는 만큼만 보내기

		send(sock, buf, bufSize, 0);
		buf += bufSize;
		sendbytes += bufSize;
		PrintPercent(sendbytes, totalbytes, bufSize);
	}
	printf("\n총 %d 바이트 파일 전송을 완료\n", sendbytes);

	fclose(fp);
	closesocket(sock);
	WSACleanup();
}
