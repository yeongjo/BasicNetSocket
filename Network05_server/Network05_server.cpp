// 헤더 파일 추가
#include <stdlib.h>
#include <stdio.h>
#include <WinSock2.h>

// 버퍼 크기 정의
#define BUFSIZE 4096

// 경고 제어
#pragma warning(disable:4996)

// 윈속 사용을 위한 라이브러리 링킹
#pragma comment(lib, "Ws2_32.lib")

// 데이터 수신 함수
int dataReceive(SOCKET s, char* buf, int len, int flags) {

	// 통신에 필요한 변수 선언
	int received;
	char *ptr = buf;
	int left = len;

	// 남아있는 데이터가 있는 경우 반복적으로 시행
	while (left > 0) {
		received = recv(s, ptr, left, flags);

		// 더이상 데이터를 받아오지 못하는 경우
		if (received == 0)
			break;

		// 받아온 데이터가 존재하는 경우
		left -= received;
		ptr += received;

		printf("%d 퍼센트\n", (int)((ptr-buf) / (float)len * 100));
	}

	// 더 받을 수 있는 데이터 길이를 반환(데이터를 한 번이라도 받았으면 0이 아닌 수를 반환하게 됨)
	return (len - left);
}

// 메인 함수
int main(int argc, char *argv[]) {

	// 데이터 크기를 담을 변수
	int retval;

	// 윈속 객체 선언 및 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// 연결용 소켓을 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	// 바인딩 처리 변수 초기화
	SOCKADDR_IN servaddr;
	ZeroMemory(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 바인딩
	bind(listen_sock, (SOCKADDR *)&servaddr, sizeof(servaddr));

	// 연결 함수 시작
	listen(listen_sock, SOMAXCONN);

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char* buf;

	// 반복적으로 클라이언트로부터 소켓 요청을 받음
	while (1) {

		// 클라이언트 접속을 받음
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);

		// 접속한 클라이언트 정보 출력
		printf("클라이언트 접속 : IP = %s, Port = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 파일 이름 받기
		char filename[256];
		ZeroMemory(filename, 256);
		dataReceive(client_sock, filename, 256, 0);
		printf("받을 파일 이름 : %s\n", filename);

		// 파일 크기 받기
		int totalbytes;
		dataReceive(client_sock, (char *)&totalbytes, sizeof(totalbytes), 0);
		buf = new char[totalbytes];
		printf("받을 파일 크기 : %d 바이트\n", totalbytes);

		// 파일 열기
		FILE *fp = fopen(filename, "wb");

		// 파일 데이터 받기
		int numtotal = 0;
		while (1) {
			retval = dataReceive(client_sock, buf, totalbytes, 0);

			// 더 받을 데이터가 없을 때
			if (retval == 0) {
				break;
			}

			else {
				fwrite(buf, 1, retval, fp);

				// 받은 데이터 크기만큼 변수에 더해줌
				numtotal += retval;
			}
		}

		fclose(fp);

		// 전송 결과
		if (numtotal == totalbytes)
			printf("파일을 성공적으로 받았습니다.\n");
		else
			printf("파일을 제대로 받지 못했습니다.\n");

		// 해당 클라이언트 소켓을 폐기
		closesocket(client_sock);
	}

	// 서버 소켓을 폐기
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}