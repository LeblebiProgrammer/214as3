// Write CPP code here

#include "h.h"


#define SA struct sockaddr

typedef struct structObj{
    char *ip;
    char *port;
}serverStruct;

void func(int sockfd, char *message)
{
	write(sockfd, message, (strlen(message)+1)*sizeof(char));

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


char *serverConnect(serverStruct *server, char *msg){
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


  char *readString = sockReader(sockfd);
	printf("From Server : %s", readString);
  //free(readString);
	// close the socket
	close(sockfd);
  return readString;
}


serverStruct *ServerStringReader(char *str){
    serverStruct *server = (serverStruct*) malloc(sizeof(serverStruct));
    char *ptr = strchr(str, '\n');
    if(ptr) {
       int index = ptr - str;
       int i = 0;
       char *ip = (char*)malloc(sizeof(char)*(index+1));

       int portLen = (strlen(str)-index);

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
    char *msg = concat("checkout", fileName, ':');
    if(strlen(msg)>0){
      char *total = msgPreparer(msg);
      free(msg);
      if(total != NULL){
        char *response = serverConnect(server, total);
        free(total);
        free(response);
      }
      else{
        printf("Error while preparing message");
      }
    }

    free(serverInfo);
    free(server->ip);
    free(server->port);
    free(server);

}

void clientCreate(char *fileName){
  char *serverInfo = fileReader("./.configure");
  //printf("%s\n", serverInfo);
  serverStruct *server = ServerStringReader(serverInfo);
  char *msg = concat("create", fileName, ':');

  if(strlen(msg)>0){
    char *total = msgPreparer(msg);
    if(total != NULL){

      char *response = serverConnect(server, total);

      char *charEnd = subString(response, '>', '1');
      char *front = subString(response, '>', '0');
      if(strcmp(front, "success") == 0){
        char *path = subIndexer(charEnd, "<", '>');
        char *c1 = subString(charEnd, '>', '1');
        char *content = subString(c1, '<', '1');

        char *strs = folderFinder(path);
        fileWriter(strs, content, 0, '1');


        free(path);
        free(c1);
        free(content);
      }
      if(charEnd != NULL){
        free(charEnd);
      }
      if(front != NULL){
        free(front);
      }


      free(total);
      free(response);
    }
    else{
      printf("Error while preparing message");
    }
  }


  free(msg);
  free(serverInfo);
  free(server->ip);
  free(server->port);
  free(server);
}

void clientDestroy(char *fileName){
  char *serverInfo = fileReader("./.configure");
  //printf("%s\n", serverInfo);
  serverStruct *server = ServerStringReader(serverInfo);
  char *msg = concat("destroy", fileName, ':');
  if(strlen(msg)>0){
    char *total = msgPreparer(msg);

    if(total != NULL){
      //printf("%s\n", total);
      char *response = serverConnect(server, total);
      free(total);
      free(response);
    }
    else{
      printf("Error while preparing message");
    }
  }
  //printf("%ld len out of destrtoy\n", strlen(msg));
  //printf("message: %s\n",msg );

  //printf("%s \t %s \n", server->ip, server->port);

  free(serverInfo);
  free(server->ip);
  free(server->port);
  free(server);
}

void clientAdd(char *projectName, char *fname){
  struct stat stats;
  if (stat( projectName, &stats) == -1){
    printf("Project name doesn't exist\n");
    return;
  }
  char *path = concat(projectName, fname, '/');
  char *manipath = concat(projectName, ".manifest", '/');
  free(path);
  free(manipath);
}

void clientRollback(char *projectName, char *versionNumber){
  char *serverInfo = fileReader("./.configure");
  //printf("%s\n", serverInfo);
  serverStruct *server = ServerStringReader(serverInfo);
  char *tmp = concat("rollback", projectName, ':');
  char *msg = concat(tmp, versionNumber, ':');
  if(strlen(msg)>0){
    char *total = msgPreparer(msg);
    if(total != NULL){
      //printf("%s\n", total);
      char *response = serverConnect(server, total);
      free(total);
      free(response);
    }
    else{
      printf("Error while preparing message");
    }
    free(msg);
  }
  free(tmp);
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
        else if(strcmp(argv[1], "add") == 0){
          char *projectName = argv[2];
          if(argv[3] != NULL){
            char *fname = argv[3];
            clientAdd(projectName, fname);
          }
        }
        else if (strcmp(argv[1], "rollback") == 0){
          char *projectName = argv[2];
          if(argv[3] != NULL){
            char *versionNumber = argv[3];
            clientRollback(projectName, versionNumber);
          }
        }

    }

}
