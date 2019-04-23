#include <netdb.h>

#include <dirent.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "helperFunctions.h"

#define MAX 10
#define SA struct sockaddr

// Function designed for chat between client and server.

int functionDeterminer(char *str){
    char *functionString = NULL;
		int type = -1;
    char *ptr = strchr(str, ':');
    if(ptr) {
       int index = ptr - str;
       int i = 0;
       functionString = (char*)malloc(sizeof(char)*(index+1));

       //printf("portlen %d\t",portLen );
       for(i = 0; i< index; i++){
           functionString[i] = str[i];
       }
       functionString[i] = '\0';
   }

	 if(strcmp(functionString, "checkout") == 0){
		 type = 1;
	 }else if(strcmp(functionString, "update") == 0){
		  type = 2;
	 }else if(strcmp(functionString, "upgrade") == 0){
		  type = 3;
	 }else if(strcmp(functionString, "commit") == 0){
		  type = 4;
	 }else if(strcmp(functionString, "push") == 0){
		  type = 5;
	 }else if(strcmp(functionString, "create") == 0){
		  type = 6;
	 }else if(strcmp(functionString, "destroy") == 0){
		  type = 7;
	 }else if(strcmp(functionString, "currentVersion") == 0){
		  type = 10;}
	else if(strcmp(functionString, "rollback") == 0){
 		  type = 11;
	 }else if(strcmp(functionString, "rollback") == 0){
		  type = 12;
	 }

	 free(functionString);
   return type;
}

void commitFunction(char *str){

}

void createFunction(char *str){

	struct stat stats;
	if (stat( str, &stats) == -1) {
    int result = mkdir(str, 0700);
    char *maniFilePath = concat("./",str, '\0');
    char *fp = concat(maniFilePath, ".manifest", '/');
    char *maniContent = "version:0\n";
    int manifestWrite = fileWriter(fp, maniContent, strlen(maniContent), '1');
    if(result != -1 && manifestWrite != -1){
      printf("Created successfully\n");

    }

	}
	else{
		printf("File present error\n");
	}
}

void destroyFunction(char *str){
  struct stat stats;
  if (stat( str, &stats) != -1){
    int rv = remove(str);
    printf("Removed");
  }
}

void func(int sockfd)
{
	char buff[MAX];
	int n;
  int size = 0;
  char *readString = (char *)malloc(sizeof(char)*MAX);
	// infinite loop for chat
	bzero(buff, MAX);

	// read the message from client and copy it in buffer
	int readSize = -1;
	while( (readSize = read(sockfd, buff, sizeof(buff))) > 0){
        printf("%s\n", buff);
        size += readSize;
        if(size != MAX){
            readString = realloc(readString, size);
            strcat(readString, buff);
        }else{
						strcat(readString, buff);
        }
        bzero(buff, MAX);
    }
    //printf("\n%s\n", readString);
		int functionType = functionDeterminer(readString);
		printf("%d\n", functionType);
		switch(functionType){
			case 1:
				commitFunction(readString);
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:{
        char *fname = subString(readString, ':');
				createFunction(fname);
				break;
      }
			case 7:{
        char * fname = subString(readString, ':');
        destroyFunction(fname);
				break;
      }
			case 10:
				break;
			case 11:
				break;
			case 12:
				break;
		}

    free(readString);
	// print buffer which contains the client contents
	// printf("From client: %s\t To client : ", buff);
	// bzero(buff, MAX);
	// n = 0;
	// // copy server message in the buffer
	// while ((buff[n++] = getchar()) != '\n')
	// 	;
    //
	// // and send that buffer to client
	// write(sockfd, buff, sizeof(buff));
    //
	// // if msg contains "Exit" then server exit and chat ended.
	// if (strncmp("exit", buff, 4) == 0) {
	// 	printf("Server Exit...\n");
	// 	break;
	// }

}


void compress(){

}


// Driver function
int main(int argc, char **argv)
{

    destroyFunction("ProjectZero");
    // if(argc<2){
    //     printf("Please enter port number\n");
    //     return 0;
    // }
    //
    // char *_port = argv[1];
    // int portNo = 0;
    // if(strlen(_port)>=4){
    //     portNo = atoi(_port);
    // }
    // else{
    //     printf("Please enter a port number greater than 8k\n");
    //     return 0;
    // }
    // int sockfd, connfd;
    // socklen_t len;
    // struct sockaddr_in servaddr, cli;
    //
    // // socket create and verification
    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd == -1) {
    // 	printf("socket creation failed...\n");
    // 	exit(0);
    // }
    // else
    // 	printf("Socket successfully created..\n");
    // bzero(&servaddr, sizeof(servaddr));
    //
    // // assign IP, PORT
    // servaddr.sin_family = AF_INET;
    // servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // servaddr.sin_port = htons(portNo);
    //
    // // Binding newly created socket to given IP and verification
    // if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    // 	printf("socket bind failed...\n");
    // 	exit(0);
    // }
    // else
    // 	printf("Socket successfully binded..\n");
    //
    // // Now server is ready to listen and verification
    // if ((listen(sockfd, 5)) != 0) {
    // 	printf("Listen failed...\n");
    // 	exit(0);
    // }
    // else{
    // 	printf("Server listening..\n");
    //        len = sizeof(cli);
    //
    //
    // 	// Accept the data packet from client and verification
    // 	connfd = accept(sockfd, (SA*)&cli, &len);
    // 	if (connfd < 0) {
    // 		printf("server acccept failed...\n");
    // 		exit(0);
    // 	}
    // 	else
    // 		printf("server acccept the client...\n");
    //
    // 	// Function for chatting between client and server
    // 	func(connfd);
    //
    // 	// After chatting close the socket
    //
    // }
    // close(sockfd);
}
