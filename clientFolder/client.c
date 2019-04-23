// Write CPP code here
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

#define MAX 80
#define SA struct sockaddr

typedef struct structObj{
    char *ip;
    char *port;
}serverStruct;

void func(int sockfd, char *message)
{
	char buff[MAX];
	// bzero(buff, sizeof(buff));
	// printf("Enter the string : ");
	// n = 0;
	// while ((buff[n++] = getchar()) != '\n')
	// 	;
    // buff[n-1] = '\0';
	write(sockfd, message, (strlen(message)+1)*sizeof(char));
	//bzero(buff, sizeof(buff));
	// read(sockfd, buff, sizeof(buff));
	// printf("From Server : %s", buff);
	// if ((strncmp(buff, "exit", 4)) == 0) {
	// 	printf("Client Exit...\n");
	// 	break;
	// }

}

char *messageConcat(char *str1, char *str2){
    int length = strlen(str1) + 1 + strlen(str2) + 1;
    char *val = (char*)malloc(sizeof(char)*length);

    strcpy(val, str1);
    val[strlen(str1)] = ':';

    int count = strlen(str1)+1;
    int i = 0;
    for(i = 0; i< strlen(str2); i++){
        val[count] = str2[i];
        count++;
    }
    val[count] = '\0';
		printf("%d len out concat\t", length);
    return val;
}

char *fileReader(char *fpath){
    int fd = open(fpath, O_RDONLY);
    char *fileStr = NULL;
    if(fd != -1){
        off_t currentPos = lseek(fd, (size_t)0, SEEK_CUR);
        int size = lseek(fd, 0, SEEK_END);
        lseek(fd, currentPos, SEEK_SET);

        fileStr = (char*)malloc(sizeof(char)*size);
        read(fd, fileStr, size);
    }
    else{
        printf("Configure file could not be opened\n");
        exit(0);
    }
    close(fd);
    return fileStr;
}

int fileWriter(char *fpath, char *string, int writeLen,char config){

    if(config == '1'){
        int fd = open(fpath, O_CREAT | O_RDWR , 0666);
        if (fd != -1) {
        //printf("%s\n", str);
            write(fd, string, (strlen(string))*sizeof(char));
        }
        close(fd);
    }
    return 0;
}

void configure(char *_ip, char *_port){
    //printf("%s\n", _port);
    int writeLen = strlen(_ip) + 1 + strlen(_port);
    char *str = (char*)malloc(sizeof(char) * writeLen);
    strcpy(str, _ip);
    str[strlen(_ip)] = '\n';
    int count = strlen(_ip)+1;
    int i = 0;
    for(i = 0; i < strlen(_port); i++){
        str[count] = _port[i];
        count++;
    }

    //str[writeLen-1] = '\0';
    fileWriter("./.configure", str, writeLen,'1');

}


void serverConnect(serverStruct *server, char *msg){
    int sockfd = -10; //int connfd;
	struct sockaddr_in servaddr;//, cli;

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));



	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(server->ip);
    int port = atoi(server->port);
	servaddr.sin_port = htons(port);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd, msg);

	// close the socket
	close(sockfd);
}


serverStruct *ServerStringReader(char *str){
    serverStruct *server = (serverStruct*) malloc(sizeof(serverStruct));
    char *ptr = strchr(str, '\n');
    if(ptr) {
       int index = ptr - str;
       int i = 0;
       char *ip = (char*)malloc(sizeof(char)*(index+1));

       int portLen = (strlen(str)-index);
       //printf("portlen %d\t",portLen );
       char *port = (char*)malloc(sizeof(char)* portLen);
       for(i = 0; i< index; i++){
           ip[i] = str[i];
       }
       ip[i] = '\0';
       i++;
       int count = 0;
       for(; i< strlen(str); i++){
           port[count] = str[i];
           count++;
       }

       port[count] = '\0';
       server->ip = ip;
       server->port = port;
       //printf("%s\n%s\n",ip, port);
    }
    return server;
}


void checkout(char *fileName){
    char *serverInfo = fileReader("./.configure");
    //printf("%s\n", serverInfo);
    serverStruct *server = ServerStringReader(serverInfo);
    char *msg = messageConcat("checkout", fileName);
		printf("%ld len out of checkout\n", strlen(msg));
    //printf("message: %s\n",msg );
    serverConnect(server, msg);
    //printf("%s \t %s \n", server->ip, server->port);


    free(serverInfo);
    free(server->ip);
    free(server->port);
    free(server);

}

void clientCreate(char *fileName){
  char *serverInfo = fileReader("./.configure");
  //printf("%s\n", serverInfo);
  serverStruct *server = ServerStringReader(serverInfo);
  char *msg = messageConcat("create", fileName);
  printf("%ld len out of create\n", strlen(msg));
  //printf("message: %s\n",msg );
  serverConnect(server, msg);
  //printf("%s \t %s \n", server->ip, server->port);


  free(serverInfo);
  free(server->ip);
  free(server->port);
  free(server);
}

void clientDestroy(char *fileName){
  char *serverInfo = fileReader("./.configure");
  //printf("%s\n", serverInfo);
  serverStruct *server = ServerStringReader(serverInfo);
  char *msg = messageConcat("destroy", fileName);
  printf("%ld len out of destrtoy\n", strlen(msg));
  //printf("message: %s\n",msg );
  serverConnect(server, msg);
  //printf("%s \t %s \n", server->ip, server->port);


  free(serverInfo);
  free(server->ip);
  free(server->port);
  free(server);
}

int main(int argc, char **argv)
{
    if(argc > 1){
        if(strcmp(argv[1], "configure") == 0){
            char *ip = argv[2];
            char *port = argv[3];
            configure(ip, port);
        }
        else if(strcmp(argv[1], "checkout") == 0){
            //printf("Hi");
            char *checkoutName = argv[2];
            checkout(checkoutName);
        }
        else if(strcmp(argv[1], "create") == 0){
            //printf("Hi");
            char *createName = argv[2];
            clientCreate(createName);
        }
        else if(strcmp(argv[1], "destroy") == 0){
            //printf("Hi");
            char *destroyName = argv[2];
            clientDestroy(destroyName);
        }
    }

}
