#include "../commonSocket.h"

using namespace std;

bool GetDomainName(char *name, SOCKADDR *addr = NULL) {
	SOCKADDR_IN6 sockAddr6 = addr?*(SOCKADDR_IN6*)addr:SOCKADDR_IN6();
	SOCKADDR_IN sockAddr = addr ? *(SOCKADDR_IN*)addr : SOCKADDR_IN();
	CHAR szBuffer1[512], szBuffer2[512]; //도메인 max 256
	ZeroMemory(szBuffer1, sizeof(szBuffer1));
	ZeroMemory(szBuffer2, sizeof(szBuffer2));
	/* IP 주소를 도메인으로 변환합니다. IP 주소를 담고 있는 sockaddr_in * 구조체, 도메인이 반환될 char * 배열, 서비스가 반환될 char * 배열이 사용됩니다. */
	SOCKADDR *sock = addr;
	socklen_t sockSize = -1;
	if (addr == NULL) {
		ZeroMemory(&sockAddr, sizeof(sockAddr));
		ZeroMemory(&sockAddr6, sizeof(sockAddr6));
		hostent* ent = NULL;
		if (strchr(name, ':')) {
			char addr6_str[40];
			sockAddr6.sin6_family = AF_INET6;
			inet_pton(AF_INET6, name, &addr6_str);
			memcpy(&sockAddr6.sin6_addr.u, (IN6_ADDR*)addr6_str, sizeof(IN6_ADDR));
			sockAddr6.sin6_port = htons(433);
			sock = (SOCKADDR *)&sockAddr6;
			sockSize = sizeof(SOCKADDR_IN6);

			ent = gethostbyaddr((char*)&(sockAddr6.sin6_addr.u), sizeof(IN6_ADDR), AF_INET6);
		}
		else {
			sockAddr.sin_family = AF_INET;
			sockAddr.sin_addr.s_addr = inet_addr(name);
			sockAddr.sin_port = htons(443);
			sock = (SOCKADDR *)&sockAddr;
			sockSize = sizeof(SOCKADDR_IN);

			ent = gethostbyaddr((char*)&(sockAddr.sin_addr.s_addr), sizeof(IN_ADDR), AF_INET);
		}
		if (ent) {
			int n1 = 0;
			while (ent->h_aliases[n1] != NULL) {
				printf("  별명[%d]   | %s \n", n1, (ent->h_aliases[n1]));
				n1++;
			}
		}
		else {
			err_display("gethostbyaddr()");
		}
	}
	else {
		sockSize = addr->sa_family == AF_INET ? sizeof(SOCKADDR_IN) : sizeof(SOCKADDR_IN6);
	}
	if (int error = GetNameInfo((const SOCKADDR *)sock, sockSize, szBuffer1, sizeof(szBuffer1), szBuffer2, sizeof(szBuffer2), 0)) {
		//cout << gai_strerror(error) << endl;
		err_display("getaddrinfo()");
		return false;
	}

	

	printf("Node Name is %s Service is %s\n", szBuffer1, szBuffer2);
	return true;
	//==============================================
	//HOSTENT *ptr = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	//if (ptr == NULL) {
	//	err_display("gethostbyaddr()");
	//	return false;
	//}
	//if (ptr->h_addrtype != AF_INET)
	//	return false;
	//strncpy(name, ptr->h_name, namelen);
	//return true;
}

bool GetIPAddr(char* name) {
	addrinfo hints, *result;
	char buf[80] = { 0x00, };
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	//hints.ai_flags = AI_DNS_ONLY;
	//hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_family = AF_INET6;
	if (name != NULL) {
		name = name[0] == '0' ? NULL : name;
	}
	if (int error = getaddrinfo(name, "80", &hints, &result)) {
		cout << gai_strerror(error) << endl;
		err_display("getaddrinfo()");
		return false;
	}

	hostent* ent = gethostbyname(name);
	if (ent) {
		int n1 = 0;
		while (ent->h_aliases[n1] != NULL) {
			printf("  별명[%d]   | %s \n", n1, (ent->h_aliases[n1]));
			n1++;
		}
	}
	else {
		err_display("gethostbyname()");
	}

	for (addrinfo *rp = result; rp != NULL; rp = rp->ai_next)
	{
		if (rp->ai_family == AF_INET)
		{
			auto sin = (sockaddr_in*)rp->ai_addr;
			inet_ntop(rp->ai_family, &sin->sin_addr, buf, sizeof(buf));
			printf("ipv4: %s\n", buf);
		}
		else if (rp->ai_family == AF_INET6)
		{
			auto sin6 = (sockaddr_in6*)rp->ai_addr;
			inet_ntop(rp->ai_family, &sin6->sin6_addr, buf, sizeof(buf));
			printf("ipv6: %s\n", buf);
		}
		GetDomainName(NULL, rp->ai_addr);
	}
	freeaddrinfo(result);
	return true;
}



int main(int argc, char **argv)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	char* ch = argv[1];
	if (argc == 1) {
		ch = new char[256];
		cin >> ch;
	}
	// 맨앞글자가 숫자면 IP주소
	if (49<=ch[0]&&ch[0]<=57) {
		GetDomainName(ch);
	}
	else {
		GetIPAddr(ch);
	}
	WSACleanup();
}
