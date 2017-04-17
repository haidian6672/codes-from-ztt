#include <windows.h>  
#include <process.h>  
#include <iostream>  
using namespace std; 
#pragma comment(lib, "Ws2_32.lib")//连接至相应库函数
#define	MAXLINE		4096	/* max text line length */
#define	LISTENQ		1024	/* 2nd argument to listen() */
int main(int argc, char* argv[])
{
	WSADATA wsa;
	char buff[MAXLINE];
	SOCKET servsocket,consocket;
	struct sockaddr_in servaddr,sockname;
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{
		cout<<"套接字初始化失败";
		return 0;
	}
	if((servsocket=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		cout<<"套接字创建失败";
		WSACleanup();
        return 0;
	}
	memset(&servaddr,0,sizeof(servaddr));//用于清空servaddr
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);//inet_addr("10.104.164.175")
	servaddr.sin_port=htons(1986);
	if(bind(servsocket,(LPSOCKADDR)&servaddr,sizeof(servaddr))!=0)
	{
		cout<<"套接字绑定失败";
		return 0;
	}


	if(listen(servsocket,LISTENQ)!=0)
	{
		cout<<"监听失败";
        return 0;
	}
	while(true)
	{
		consocket=accept(servsocket,(LPSOCKADDR)NULL,NULL);
		
		int size=sizeof(sockname);

	    if(getpeername(consocket,(LPSOCKADDR)&sockname,&size)==-1)
		{
		    cout<<"名字获取失败";
		}
		cout<<ntohs(sockname.sin_port)<<'\n'<<inet_ntoa(sockname.sin_addr)<<'\n'<<"连接已建立,请输入:";
		cin>>buff;
		send(consocket, buff, strlen(buff),0);
		closesocket(consocket);
	}
	closesocket(servsocket);
	WSACleanup();
	return 0;
}
