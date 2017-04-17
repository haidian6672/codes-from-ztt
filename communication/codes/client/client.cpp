#include <windows.h>  
#include <process.h>  
#include <iostream>  
using namespace std; 
#pragma comment(lib, "Ws2_32.lib")
#define	MAXLINE		4096	/* max text line length */
int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET cltsocket;
	int n;
	char recvline[MAXLINE+1];
	struct sockaddr_in servaddr;
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{
		cout<<"套接字初始化失败";
		return 0;
	}
	if((cltsocket=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		cout<<"套接字创建失败";
		WSACleanup();
		return 0;
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	servaddr.sin_port=htons(1986);
	if((connect(cltsocket,(LPSOCKADDR)&servaddr,sizeof(servaddr)))!=0)
	{
		cout<<"连接失败";
		WSACleanup();
		return 0;
	}
	while ( (n = recv(cltsocket, recvline, MAXLINE,0)) > 0) //接收服务器应答
	{
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF)
			printf("fputs error");
	}
	if (n < 0)
		printf("recv error");
	closesocket(cltsocket);
    	WSACleanup();     		//终止socket的使用

	return 0;


}

