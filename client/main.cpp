/*************************************************************************
    > File Name: main.cpp
    > Author: yeguo
    > Mail: =={======> 
    > Created Time: 2018-06-16
 ************************************************************************/
#include<string.h>
#include<iostream>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdio.h>

#define ERR_EXIT(m,n)\
	do{\
		if(m<0){ \
			perror(n);\
			exit(0);\
		} \
	}while(0)

int f1(){
	int sockFD = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(10001);
	int conFlag = connect(sockFD,(sockaddr*)&sockAddr,sizeof(sockAddr));
	ERR_EXIT(conFlag, "connect error");

	sockaddr_in mySockAddr;
	socklen_t mySockLen = sizeof(mySockAddr);
	int getSockFlag = getsockname(sockFD,(sockaddr*)&mySockAddr, &mySockLen);
	ERR_EXIT(getSockFlag, "getSockFlag error");
	printf("get my socket data host:%s , port:%d\n", inet_ntoa(mySockAddr.sin_addr), ntohs(mySockAddr.sin_port) );
	
	fd_set myset;
	int inFD = fileno(stdin);
	int maxFD = std::max(sockFD,inFD);
	while(0){
		printf("please input \n");
		char buf[1024];
		memset(buf,0,sizeof(buf));
		FD_ZERO(&myset);
		FD_SET(inFD, &myset);
		FD_SET(sockFD, &myset);
		select(maxFD+1, &myset, NULL, NULL, NULL);
		if(FD_ISSET(inFD,&myset)){
			if(fgets(buf, sizeof(buf), stdin)==NULL){
				break;
			}else{
				printf("write : %s\n",buf);
				write(sockFD,buf,sizeof(buf));
			}
		}
		if(FD_ISSET(sockFD,&myset)){
			int readFlag = read(sockFD,buf,sizeof(buf));
			if (readFlag==-1 && errno==EINTR){
				continue;
			}
			printf("read : %s",buf);
		}
	};
	printf("close socket\n");
	close(sockFD);
	return 0;
}


int f2(){
	int sockFD = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(10001);
	int conFlag = connect(sockFD,(sockaddr*)&sockAddr,sizeof(sockAddr));
	ERR_EXIT(conFlag, "connect error");

	sockaddr_in mySockAddr;
	socklen_t mySockLen = sizeof(mySockAddr);
	int getSockFlag = getsockname(sockFD,(sockaddr*)&mySockAddr, &mySockLen);
	ERR_EXIT(getSockFlag, "getSockFlag error");
	printf("get my socket data host:%s , port:%d\n", inet_ntoa(mySockAddr.sin_addr), ntohs(mySockAddr.sin_port) );
	
	char buf[1024];
	memset(buf,0,sizeof(buf));
	strcpy(buf,"hello man!");
	write(sockFD,buf,sizeof(buf));
	close(sockFD);
	printf("client close\n");
	return 0;
}

int main(){
	return f2();
}
