#include <windows.h>
#include <winsock.h>

#include<iostream>

using namespace std;
#pragma comment (lib,"ws2_32")						//加载库函数

#define GetDriver	0x01
#define GetDirInfo	0x02
#define ExecFile	0x03
#define GetFile		0x04
#define PutFile		0x05
#define DelFile		0x06
#define DelDir		0x07
#define CreateDir	0x08
#define FileInfo	0x09
#define GetScreen	0x10

#define  BUF_LEN	1024
#define CHUNK_SIZE (64*1024)	


typedef struct					//命令结构
{
	int			ID;				//命令ID
	BYTE		lparam[BUF_LEN*2]; 

}COMMAND;

typedef struct					//文件结构
{
	char		FileName[MAX_PATH];
	int			FileLen;
	char		Time[50];
	BOOL		IsDir;
	BOOL		Error;
	HICON		hIcon;			//图标句柄

}FILEINFO;

DWORD WINAPI CLisen(LPVOID lparam);
DWORD	GetDriverProc	(COMMAND command,SOCKET client);
DWORD	GetDirInfoProc	(COMMAND command,SOCKET client);
DWORD	ExecFileProc	(COMMAND command,SOCKET client);
DWORD	DelFileProc		(COMMAND command,SOCKET client);
DWORD	FileInfoProc	(COMMAND command,SOCKET client);
DWORD   CreateDirProc	(COMMAND command,SOCKET client);
DWORD   DelDirProc		(COMMAND command,SOCKET client);
DWORD   GetFileProc		(COMMAND command,SOCKET client);
DWORD   PutFileProc		(COMMAND command,SOCKET client);


SOCKET server;




int main()
{
	WSADATA wsadata;
	
	int ID;
	struct sockaddr_in servaddr;
	if(WSAStartup(MAKEWORD(2,2),&wsadata)!=0)
	{
		printf("套接字初始化失败");
		return 0;
	}
	if((server=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("套接字创建失败");
		WSACleanup();
		return 0;
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	servaddr.sin_port=htons(5555);
	if((connect(server,(LPSOCKADDR)&servaddr,sizeof(servaddr)))!=0)
	{
		printf("连接失败");
		WSACleanup();
		return 0;
	}

	COMMAND command;
	memset((char*)&command,0,sizeof(command));
	cout<<"(GetDriver:1 GetDirInfo:2 ExecFile:3 GetFile:4 PutFile:5 DelFile:6 DelDir:7 CreateDir:8 FileInfo:9 GetScreen:10)\n请输入要实现的功能ID号码:";
	cin>>ID;
	command.ID=ID;
	send(server,(char*)&command,sizeof(command),0);	

	CreateThread(NULL,NULL,CLisen,(LPVOID)server,NULL,NULL);


	closesocket(server);
   	WSACleanup();     		//终止socket的使用
	return 0;


}


DWORD WINAPI CLisen(LPVOID lparam)
{


	COMMAND command;
	memset((char*)&command,0,sizeof(command));
	while(true)
		{
			memset((char*)&command,0,sizeof(command));
			if(recv(server,(char*)&command,sizeof(command),0)==0)
			{
				break;
			}else
			{
				switch(command.ID)
				{
				case GetDriver:
					cout<<"!!";

					 break;
				case GetDirInfo:

					 break;
				case DelFile:

					 break;
				case ExecFile:

					 break;
				case FileInfo:

					 break;
				case DelDir:

					 break;
				case CreateDir:
		
					 break;
				case GetFile:
				
					 break;
				case PutFile:
				
					 break;
				case GetScreen:
			
					 break;

				}
			}


	}
		

	
	closesocket(server);
	WSACleanup();

	return 0;
}
