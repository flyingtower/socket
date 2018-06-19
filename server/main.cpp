/*************************************************************************
    > File Name: main.cpp
    > Author: yeguo
    > Mail: =={======> 
    > Created Time: 2018-06-16
 ************************************************************************/
#include<sys/wait.h>
#include<iostream>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include<signal.h>
#define ERR_EXIT(m,n)\
	do{\
		if(m<0){ \
			perror(n);\
			exit(0);\
		} \
	}while(0)

#define ERR_P(m,n)\
	do{\
		if(m<0){ \
			perror(n);\
		} \
	}while(0)

int childFd = 0;

void doService(){
	ERR_EXIT(childFd,"childFd < 0");
	printf("begin service\n");
	fd_set moniSet;
	char buf[1024];
	int inFD = fileno(stdin);
	int maxFD = std::max(childFd,inFD);
	while(1){
		printf("please input\n");
		FD_ZERO(&moniSet);
		FD_SET(childFd,&moniSet);
		FD_SET(inFD,&moniSet);
		select(maxFD+1,&moniSet,NULL,NULL,NULL);
		if(FD_ISSET(childFd,&moniSet)){
			memset(buf,0,sizeof(buf));
			int readCnt = read(childFd, buf, sizeof(buf));
			if(readCnt<0){
				perror("read error : ");
			}else if(readCnt == 0){
				printf("peer send FIN\n");
				break;
			}else{
				printf("rece sock : %s\n",buf);
				write(childFd,buf,sizeof(buf));
			}
		}
		if(FD_ISSET(inFD,&moniSet)){
			memset(buf,0,sizeof(buf));
			char* inp = fgets(buf,sizeof(buf),stdin);
			if(inp==NULL){
				break;
			}
			printf("rece stdin : %s\n",inp);
		}
	};
	printf("finish service\n");
	close(childFd);
	exit(0);
}

void handleChild(int sig){
	printf("get child exit signal %d\n",sig);
	//while(waitpid(-1,NULL,WNOHANG|WUNTRACED)>0);
	wait(NULL);
	printf("handle finished\n");
}

int f1(){
	signal(SIGCHLD, handleChild);
	int sockFD = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	sockAddr.sin_port = htons(10001);
	int on = 1;
	setsockopt(sockFD,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	int bindFlag  = bind(sockFD,(sockaddr*)&sockAddr,sizeof(sockAddr));
	ERR_EXIT(bindFlag, "bind error");
	int listenFlag = listen(sockFD,SOMAXCONN);
	ERR_EXIT(listenFlag, "listen error: ");
	
	while(1){
		sockaddr_in tmpSockAddr;
		socklen_t tmpL = sizeof(tmpSockAddr);
		memset(&tmpSockAddr,0,sizeof(tmpSockAddr));
		int fd = accept(sockFD, (sockaddr*)&tmpSockAddr, &tmpL);
		if(fd>0){
			int pid = fork();
			if(pid==-1){
				perror("create service error");
			}else if (pid==0){
				childFd = fd;
				close(sockFD);	
				doService();
			}
			close(fd);
		}else{
			perror("accept error");
			break;
		}
	};
	printf("server exit\n");
	close(sockFD);
	return 0;
}

void pipeHandler(int sig){
	printf("get pipe sig %d\n",sig);
}

int f2(){
	signal(SIGPIPE, pipeHandler);
	int sockFD = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	sockAddr.sin_port = htons(10001);
	int on = 1;
	setsockopt(sockFD,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	int bindFlag  = bind(sockFD,(sockaddr*)&sockAddr,sizeof(sockAddr));
	ERR_EXIT(bindFlag, "bind error");
	int listenFlag = listen(sockFD,SOMAXCONN);
	ERR_EXIT(listenFlag, "listen error: ");
	
	sockaddr_in tmpSockAddr;
	socklen_t tmpL = sizeof(tmpSockAddr);
	memset(&tmpSockAddr,0,sizeof(tmpSockAddr));
	int fd = accept(sockFD, (sockaddr*)&tmpSockAddr, &tmpL);
	if(fd<=0){
		perror("accept error");
	}else{
		char buf[1024];
		memset(buf,0,sizeof(buf));
		int flag = read(fd,buf,sizeof(buf));
		printf("flag:%d read:%s\n",flag,buf);
		memset(buf,0,sizeof(buf));
		flag = read(fd,buf,sizeof(buf));
		printf("flag:%d read:%s\n",flag,buf);
		memset(buf,0,sizeof(buf));
		flag = read(fd,buf,sizeof(buf));
		printf("flag:%d read:%s\n",flag,buf);
		memset(buf,0,sizeof(buf));
		flag = read(fd,buf,sizeof(buf));
		printf("flag:%d read:%s\n",flag,buf);
		timeval a;
		sleep(3);
		flag = write(fd,buf,sizeof(buf));
		printf("flag:%d write:%s\n",flag,buf);
		flag = write(fd,buf,sizeof(buf));
		printf("flag:%d write:%s\n",flag,buf);
		flag = read(fd,buf,sizeof(buf));
		printf("flag:%d read:%s\n",flag,buf);
	}
	printf("server exit\n");
	close(sockFD);
	return 0;
}

int main(){
	return f2();
}
