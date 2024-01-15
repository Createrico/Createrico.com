#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <iostream>
#include <dirent.h>
#include <conio.h>
#include <cstdio>
#include <io.h>
 
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

#pragma comment(lib, "ws2_32.lib")

#define PORT 60422
#define BUFFER_SIZE 1000000

using namespace std;

WSADATA wsaData;
int server, client;
struct sockaddr_in serverAddr, clientAddr;
int addrLen = sizeof(clientAddr);
char buffer[BUFFER_SIZE];
SYSTEMTIME st;

int gt(string mode)
{
	GetLocalTime(& st);
	if (mode == "year") return st.wYear;
	if (mode == "month") return st.wMonth;
	if (mode == "day") return st.wDay;
	if (mode == "hour") return st.wHour;
	if (mode == "minute") return st.wMinute;
	if (mode == "second") return st.wSecond;
}

void agt(string mode, string color)
{
	if (color == "R") SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	else if (color == "G") SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	else if (color == "B") SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	cout << "● ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	if (mode == "ahms")
	{
		cout << "[" << gt("hour") << ":" << gt("minute") << ":" << gt("second") << "] ";
	}
}

void Boot()
{
	 SetConsoleTitleA("Listening--ToFile_Server");
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    server = socket(AF_INET, SOCK_STREAM, 0);
	
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(server, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    listen(server, 5);
	agt("ahms", "G"); cout << "Server is listening on port " << PORT << endl;
}

void Accept()
{
	// Accept a connection from a client
	client = accept(server, (struct sockaddr*)&clientAddr, &addrLen);
    SetConsoleTitleA("Connected--ToFile_Server");
	agt("ahms", "G");
	cout << "[Accepted] ";
	printf("Accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
}

void waitY(int i)
{
	//cout << i << " " << 0 << endl;
	char c[1]; recv(client, c, 1, 0);
	//cout << i << " " << 1 << endl;
	send(client, "a", 1, 0);
	//cout << i << " " << 2 << endl;
}

void waitM(int i)
{
	//cout << i << " " << 0 << endl;
	send(client, "a", 1, 0);
	//cout << i << " " << 1 << endl;
	char c[1]; recv(client, c, 1, 0);
	//cout << i << " " << 2 << endl;
}

string findfile(int num)
{
	const char* directoryPath = "File";
    DIR* dir = opendir(directoryPath);
	int which = 0;
    if (dir != NULL)
	{
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
		{
            if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
			{
                which ++;
                if (which == num)
				{
					string name = entry->d_name;
					return name;
				}
            }
        }
        closedir(dir);
    }
    return "Nofile";
}

void sendfilename()
{
	string name = "";
	const char* directoryPath = "File";
    DIR* dir = opendir(directoryPath);

    if (dir != NULL)
	{
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
		{
            if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
			{
                string filename = entry->d_name;
                name += filename + "|";
            }
        }
        closedir(dir);
        if (name.empty()) name += "|";
        char namelong[10000] = {'\0'}; itoa(name.length(), namelong, 10); 
        waitY(2);
		send(client, namelong, strlen(namelong), 0);
		const char* zs = name.c_str(); char Fn[name.length() + 1]; strcpy(Fn, zs);
        waitY(3);
		send(client, Fn, name.length(), 0);
        agt("ahms", "W"); cout << "File name sent." << endl;
    }
	else agt("ahms", "R"), cout << "Unable to open the directory." << endl;
}

int main()
{
	Boot();
	HWND Server = GetForegroundWindow();
	//ShowWindow(Server, 0);
	while (1)
	{
		Accept();
		char Mode[1];
		_sleep(50);
		int Isclient = recv(client, Mode, 1, 0);
		if (Isclient < 0) break;
		bool issafe = true;
		bool issize = true;
		while (1)
		{
			if (Mode[0] == 'A')
			{
				//发送所有文件名
				waitY(1);
				sendfilename();
				//接收上传文件名
				char filenamelong[10000] = {'\0'}; long long lfilenamelong;
				waitM(4);
				if (int a = recv(client, filenamelong, sizeof(filenamelong), 0) <= 0) break;
				lfilenamelong = atoi(filenamelong);
				char filename[lfilenamelong + 1] = {'\0'};
				waitM(5);
				if (int a = recv(client, filename, lfilenamelong, 0) <= 0) break;
				string Name = "";
				for (int i = strlen(filename); i >= 0; i--)
			    {
			    	if (filename[i] == '\\' || filename[i] == '/')
			    	{
			    		for (int j = i + 1; j <= strlen(filename); j++) Name += filename[j];
			    		break;
					}
				}
				string Fname = "File\\" + Name;
			    const char* zs = Fname.c_str();
			    char Fp[Fname.length() + 1];
				strcpy(Fp, zs);
				//接收上传文件大小 
				char recvlong[1000000]; long long lrecvlong = 0;
				waitM(6);
				while (1)
				{
					int a = recv(client, recvlong, sizeof(recvlong), 0);
					if (a <= 0)
					{
						issize = false;
						break;
					}
					lrecvlong += atoi(recvlong);
					if (recvlong[0] == 'S') break;
				}
				if (!issize)
				{
					agt("ahms", "R"); cout << "Size-interrupt" << endl;
					break;
				}
				//接收文件 
				long long receivedlong = 0;
				FILE* file = fopen(Fp, "wb");
				memset (buffer, '\0', sizeof buffer);
				agt("ahms", "W"); cout << "file size: " << lrecvlong << endl;
				waitM(7);
			    while (1)
				{
					cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
					if (receivedlong >= lrecvlong)//接收完整后退出 
					{
			            break;
			        }
			        long long bytesRead = recv(client, buffer, sizeof(buffer), 0);
			        if (bytesRead <= 0)
			        {
			        	issafe = false;
			        	break;
					}
			        
			        size_t fin = fwrite(buffer, 1, bytesRead, file);
			        receivedlong += bytesRead;
			        //cout << receivedlong << " " << lrecvlong;
			    }
				fclose(file);
				if (!issafe)
				{
					remove(Fp);
					agt("ahms", "R"); cout << "[File-interrupt] Delete " << Fp << endl;
					break;
				}
				agt("ahms", "W"); cout << "File received and saved as " << Fp << endl;
				waitY(8);
			}
			if (Mode[0] == 'B')
			{
				//发送所有文件名
				waitY(1);
				sendfilename();
				//接收下载文件名
				char EN[1] = {'\0'};
				waitM(10);
				if (recv(client, EN, 1, 0) <= 0) break;
				char dz[11000] = {'\0'};
				if (EN[0] == 'E')
				{
					char filenamelong[10000] = {'\0'}; long long lfilenamelong;
					waitM(4);
					if (int a = recv(client, filenamelong, sizeof(filenamelong), 0) <= 0) break;
					lfilenamelong = atoi(filenamelong);
					char filename[lfilenamelong + 1] = {'\0'};
					waitM(5);
					if (int a = recv(client, filename, lfilenamelong, 0) <= 0) break;
					string sfilename = filename;
					string ssfilename = "File\\" + sfilename;
					const char* zs = ssfilename.c_str();
				    char Fp[ssfilename.length() + 1];
					strcpy(Fp, zs);
					memcpy(dz, Fp, strlen(Fp));
					//检测文件存在？ 
					FILE* file = fopen(dz, "r");
					waitY(6);
					if (!file)
					{
						send(client, "N", 1, 0);
						agt("ahms", "R"); cout << "No file as " << dz << endl;
						fclose(file);
						continue;
					}
					else send(client, "Y", 1, 0);
					fclose(file);
				}
				else
				{
					char filenamelong[10000] = {'\0'}; long long lfilenamelong;
					waitM(11);
					if (int a = recv(client, filenamelong, sizeof(filenamelong), 0) <= 0) break;
					lfilenamelong = atoi(filenamelong);
					char filename[lfilenamelong + 1] = {'\0'};
					waitM(12);
					if (int a = recv(client, filename, lfilenamelong, 0) <= 0) break;
					long long filenamel = atoi(filename);
					string findname = findfile(filenamel);
					waitY(13);
					if (findname == "Nofile")
					{
						send(client, "N", 1, 0);
						agt("ahms", "R"); cout << "No file as code: " << filenamel << endl;
						continue;
					}
					else send(client, "Y", 1, 0), agt("ahms", "W"), cout << "Find code file: " << findname << endl;
					const char* zs = findname.c_str();
				    char Fp[findname.length() + 1];
					strcpy(Fp, zs);
					char findnamelong[10000] = {'\0'};
					itoa(findname.length(), findnamelong, 10);
					waitY(14);
					send(client, findnamelong, strlen(findnamelong), 0);
					waitY(15);
					send(client, Fp, findname.length(), 0);
					string ssfindname = "File\\" + findname;
					const char* ds = ssfindname.c_str();
				    char Up[ssfindname.length() + 1];
					strcpy(Up, ds);
					memcpy(dz, Up, strlen(Up));
				}
				
				//发送文件长度 
				double lalllong = 0; int ialllong = 0; char alllong[1000000] = {'\0'};
				char lo[1000000] = {'\0'};
				int lon;
				FILE* fil = fopen(dz, "rb");
				waitY(7);
				while ((lon = fread(lo, 1, sizeof(lo), fil)) > 0)
				{
					ialllong = lon;
					lalllong += ialllong;
					itoa(ialllong, alllong, 10);
					Sleep(5);
					send(client, alllong, ialllong, 0);
				}
				fclose(fil);
				_sleep(500);
				send(client, "S", 1, 0);
				agt("ahms", "W"); cout << "Download file length sent: " << lalllong << endl;
				//发送文件 
				FILE *file = fopen(dz, "rb");
				size_t bytesRead;
			    long long all = 0;
				waitY(8);
			    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
				{
			        ssize_t bytesSent = send(client, buffer, bytesRead, 0);
			        all += bytesRead;
			    }
			    agt("ahms", "W"); cout << "File sent" << endl;
			    fclose(file);
			    waitM(9);
			}
		}
		agt("ahms", "R"); cout << "Disconnected." << endl;
		SetConsoleTitleA("Disconnected--ToFile_Server");
	}
} 
