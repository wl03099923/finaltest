#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#include<iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
using namespace std;
int __cdecl main(void) 
{
	
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
	char sendbuf[DEFAULT_BUFLEN]; // added by eric
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
	string dic[100]={"apple","banana","cat","dog","egg","fish","house","water"};
	string dic2[100]={"蘋果","香蕉","貓咪","狗","蛋","魚","房子","水"};
	for(int k=8;k<100;k++){dic[k]="";dic2[k]="";}
	//string dc;
	int num=8;
	string add="+add";
	string del="-del";
	bool addic=false,addic2=false,addic3=false,delic=false,delic2=false;
    
    // Initialize Winsock
	while(true){
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
	bool ecx=false;
    do {
		
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
			
//            printf("Bytes received: %d\n", iResult);    // modified by eric
			printf("client: %s\n", recvbuf);
			if(strcmp(recvbuf,add.c_str())==0)addic=true;
			if(strcmp(recvbuf,del.c_str())==0)delic=true;
			if(addic==true){strcpy(sendbuf,"請新增單字");addic=false;addic2=true;}
			else if(addic2==true){string dc(recvbuf);dic[num]=dc;addic2=false;addic3=true;strcpy(sendbuf,"請新增單字的意思");}
			else if(addic3==true){string dc(recvbuf);dic2[num++]=dc;addic3=false;strcpy(sendbuf,"單字新增完畢");}
			else if(delic==true){strcpy(sendbuf,"請輸入要刪除的單字");delic=false;delic2=true;}
			else if(delic2==true){
				bool cut=false;delic2==false;
				for(int k=0;k<num;k++){
					if(strcmp(recvbuf,dic[k].c_str())==0){dic[k]="";dic2[k]="";cut=true;}
					if(cut==true){dic[k]=dic[k+1];dic2[k]=dic2[k+1];}
				}
				num--;
				if(cut==true){cut=false;strcpy(sendbuf,"該單字已被刪除");}
				else {strcpy(sendbuf,"查無此單字");}
			}
			else for(int k=0;k<num;k++){
					if(strcmp(recvbuf,dic[k].c_str())==0){strcpy(sendbuf,dic2[k].c_str());break;}
					else strcpy(sendbuf,"沒有這個單字的意思");
			}
			
			if(lstrlen(recvbuf)==0){strcpy(sendbuf,"");ecx=true;}
			printf("server: \n");

        // Echo the buffer back to the sender   // disabled by eric
			//gets(sendbuf);
			
         //   iSendResult = send( ClientSocket, recvbuf, iResult, 0 );
			iSendResult = send( ClientSocket, sendbuf, lstrlen(sendbuf)+1, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
				return 1;
            }
//            printf("Bytes sent: %d\n", iSendResult);  // Commented by eric
		}
        else if (iResult == 0);
          //  printf("Connection closing...\n");
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
		}

    } while (iResult > 0);
	
    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
	
    // cleanup
	closesocket(ClientSocket);
    WSACleanup();
	if(ecx==true)break;
	}
	cout<<"The server is closed"<<endl;
	system("pause");
    return 0;
}