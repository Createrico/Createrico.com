#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 60422
#define BUFFER_SIZE 1000000

using namespace std;

WSADATA wsaData;
int client;
struct sockaddr_in serverAddr;
char buffer[BUFFER_SIZE];
char Ip[30];

void HideCursor(int num)
{
	CONSOLE_CURSOR_INFO cursor_info = {1, num};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void Boot()
{
	fstream IP("Data\\IP", ios::in);
	IP >> Ip;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
        perror("WSAStartup failed");
        exit(0);
    }

    // Create socket
    if ((client = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
        perror("Socket creation failed");
        exit(0);
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(Ip);
    serverAddr.sin_port = htons(PORT);
}

void Connect()
{
	// Connect to the server
    if (connect(client, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
        perror("Connection failed");
        closesocket(client);
        return;
    }
    printf("Connected to the server\n");
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

void recvfilename()
{
	char namelong[10000] = {'\0'}; long long lnamelong;
	waitM(2);
	recv(client, namelong, sizeof(namelong), 0);
	lnamelong = atoi(namelong);
	char name[lnamelong] = {'\0'};
	waitM(3);
	recv(client, name, sizeof(name), 0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	cout << endl << "Files:" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	int num = 1;
	cout << num << ". ";
	for (int i = 0; i < lnamelong; i++)
	{
		if (name[i] == '|') 
		{
			if (i < lnamelong - 2) num ++, cout << endl << num << ". ";
			else
			{
				cout << endl << "-----";
				break;
			}
		}
		else cout << name[i];
	}
	cout << endl << endl;
}

int main()
{
	SetConsoleTitleA("ToFile");
	int Mode;
	cout << "Upload[1] or Download[2]: ";
	cin >> Mode;
	Boot();
	Connect();
	if (Mode == 1)
	{
		//发送模式 
		Sleep(50);
		send(client, "A", 1, 0);
		while (1)
		{
			system("cls");
			cout << "Mode: Upload" << endl;
			//接受所有文件名 
			waitM(1);
			recvfilename();
			//选择上传的文件和发送文件名
			cout << endl << "Drag in File: ";
			string filepath = "";
			while (1)
			{
				getline(cin, filepath);
				if (!filepath.empty()) break;
			}
			const char* zs = filepath.c_str(); char Fp[filepath.length() + 1] = {'\0'}; strcpy(Fp, zs);
			char filepathlong[100] = {'\0'}; itoa(filepath.length(), filepathlong, 10);
			waitY(4);
			send(client, filepathlong, filepath.length(), 0);
			waitY(5);
			send(client, Fp, filepath.length(), 0);
			//发送文件长度 
			double lalllong = 0; int ialllong = 0; char alllong[1000000] = {'\0'};
			char lo[1000000] = {'\0'};
			int lon;
			FILE* fil = fopen(Fp, "rb");
			cout << "Please wait" << endl;
			cout << "In preparation";
			SetConsoleTitleA("In preparation--ToFile");
			waitY(6);
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
			//发送文件 
			FILE* file = fopen(Fp, "rb");
			size_t bytesRead;
		    long long all = 0;
		    HideCursor(0);
			waitY(7);
		    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
			{
				cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bUploaded: ";
		        ssize_t bytesSent = send(client, buffer, bytesRead, 0);
		        all += bytesRead;
		        printf("%0.2f", all / lalllong * 100);
		        cout << "%";
		    }
		    cout << endl;
		    cout << "Upload" << endl;
		    SetConsoleTitleA("Upload--ToFile");
		    _sleep(300);
		    HideCursor(1);
		    waitM(8);
	    }
	}
	if (Mode == 2)
	{
		//发送模式 
		Sleep(50);
		send(client, "B", 1, 0);
		while (1)
		{
			system("cls");
			cout << "Mode: Download" << endl;
			//接受所有文件名 
			waitM(1);
			recvfilename();
			//发送下载文件名
			cout << "Download File (code): ";
			string filename;
			cin >> filename;
			bool haveE = false;
			for (int i = 0; i < filename.length(); i++)
			{
				if ((int)filename[i] < 48 || (int)filename[i] > 57)
				{
					haveE = true;
					break;
				}
			}
			char dz[11000] = {'\0'};
			if (haveE)
			{
				waitY(10);
				send(client, "E", 1, 0);
				char filenamelong[10000] = {'\0'}; itoa(filename.length(), filenamelong, 10);
				const char* zs = filename.c_str();
				char Fp[filename.length() + 1];
				strcpy(Fp, zs);
				memcpy(dz, Fp, strlen(Fp));
				waitY(4);
				send(client, filenamelong, filename.length(), 0);
				waitY(5);
				send(client, Fp, filename.length(), 0);
				//文件是否存在？
				char ans[1] = {'\0'};
				waitM(6);
				recv(client, ans, 1, 0);
				if (ans[0] == 'N')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);					
					cout << "No file" << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					_sleep(200);
					continue;
				}
			}
			else
			{
				waitY(10);
				send(client, "N", 1, 0);
				char filenamelong[10000] = {'\0'}; itoa(filename.length(), filenamelong, 10);
				const char* zs = filename.c_str();
				char Fp[filename.length() + 1];
				strcpy(Fp, zs);
				waitY(11);
				send(client, filenamelong, filename.length(), 0);
				waitY(12);
				send(client, Fp, filename.length(), 0);
				char data[1] = {'\0'};
				waitM(13);
				recv(client, data, 1, 0);
				if (data[0] == 'N')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);					
					cout << "No file code" << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					_sleep(200);
					continue;
				}
				else
				{
					char findnamelong[10000] = {'\0'};
					waitM(14);
					recv(client, findnamelong, sizeof(findnamelong), 0);
					long long lfindnamelong = atoi(findnamelong);
					char findname[lfindnamelong + 1] = {'\0'};
					waitM(15);
					recv(client, findname, lfindnamelong, 0);
					cout << "Find file: " << findname << endl;
					memcpy(dz, findname, strlen(findname));
					//_sleep(500);
				}
			}
			
			//接收下载文件大小 
			cout << "Please wait" << endl;
			cout << "In preparation";
			SetConsoleTitleA("In preparation--ToFile");	
			char recvlong[1000000]; long long lrecvlong = 0;
			waitM(7);
			while (1)
			{
				int a = recv(client, recvlong, sizeof(recvlong), 0);
				if (a <= 0)
				{
					cout << "No Internet" << endl;
					exit(0);
				}
				lrecvlong += atoi(recvlong);
				if (recvlong[0] == 'S') break;
			}
			cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
			double drecvlong = lrecvlong;
			//接收文件 
			double receivedlong = 0;
			FILE* file = fopen(dz, "wb");
			memset (buffer, '\0', sizeof buffer);
			cout << "file size: " << lrecvlong << endl;
			HideCursor(0);
			waitM (8);
			while (1)
			{
				cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bDownloaded:";
				if (receivedlong >= lrecvlong)//接收完整后退出 
				{
			        break;
			    }
			    long long bytesRead = recv(client, buffer, sizeof(buffer), 0);
			    if (bytesRead <= 0)
			    {
			    	cout << "No Internet" << endl; 
			       	exit(0);
				}
			    int fin = fwrite(buffer, 1, bytesRead, file);
			    receivedlong += bytesRead;
			    printf("%0.2f", receivedlong / drecvlong * 100);
			    cout << "%";
			}
			fclose(file);
			cout << "Download" << endl; 
			SetConsoleTitleA("Download--ToFile");
			_sleep(300);
			HideCursor(1);
			waitY(9);
		}
	}
}
