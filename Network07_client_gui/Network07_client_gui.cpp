#include <sdkddkver.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <tchar.h>

#include <CommCtrl.h>

#include "../commonSocket.h"



int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	INITCOMMONCONTROLSEX iccex;
	iccex.dwSize = sizeof(iccex);
	iccex.dwICC = ICC_PROGRESS_CLASS;

	if (!InitCommonControlsEx(&iccex))
		return 1;

	// Setup window class attributes.
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpszClassName = TEXT("PROGRESSBARSAMPLE");
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;

	// Register window and ensure registration success.
	if (!RegisterClassEx(&wcex))
		return 1;

	// Create the window.
	HWND hWnd = ::CreateWindowEx(0,	TEXT("PROGRESSBARSAMPLE"),TEXT("ProgressBar samples"), WS_OVERLAPPEDWINDOW,	0,0,640,480,NULL,NULL,hInstance,NULL);

	// Validate window.
	if (!hWnd)
		return 1;


	HWND hDefaultProgressCtrl,
		hSmoothProgressCtrl,
		hVerticalProgressCtrl;


	// Create default progress bar.
	hDefaultProgressCtrl = ::CreateWindowEx(0,PROGRESS_CLASS,TEXT(""),WS_CHILD | WS_VISIBLE,20,20,450,30,hWnd,(HMENU)ID_DEFAULTPROGRESSCTRL,hInstance,NULL);

	::SendMessage(hDefaultProgressCtrl, PBM_SETPOS, (WPARAM)(INT)80, 0);

	// Display the window.
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	// client code begin
	SOCKET sock = CreateSocket(inet_addr("127.0.0.1"));

	char filename[256]; ZeroMemory(filename, 256);
	sprintf(filename, "b.mkv");
	FILE* fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	int totalbytes = ftell(fp); rewind(fp);

	send(sock, filename, 256, 0);							// 파일이름
	send(sock, (char*)&totalbytes, sizeof(totalbytes), 0); // 보낼파일크기

	char* buf = new char[totalbytes];
	fread(buf, 1, totalbytes, fp);
	int sendbytes = 0;

	for (int remain = totalbytes; 0 < remain; remain -= BUFSIZE) {
		int bufSize = BUFSIZE;		// 기본 4096만큼 보내기
		if (remain - BUFSIZE < 0)	// 남아있는 버퍼가 적으면
			bufSize = remain;		// 남아있는 만큼만 보내기
		send(sock, buf, bufSize, 0);

		buf += bufSize;
		sendbytes += bufSize;
		PrintPercent(sendbytes, totalbytes, bufSize);
		::SendMessage(hDefaultProgressCtrl, PBM_SETPOS, (WPARAM)(INT)sendbytes/ (float)totalbytes*100, 0);
	}
	//printf("\n총 %d 바이트 파일 전송을 완료\n", sendbytes);

	fclose(fp);
	closesocket(sock);
	WSACleanup();

	MSG msg;
	while (::GetMessage(&msg, hWnd, 0, 0) > 0)
		::DispatchMessage(&msg);

	::UnregisterClass(wcex.lpszClassName, hInstance);

	return (int)msg.wParam;
}