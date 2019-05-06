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

int projName_exists(char* projName) {//returns -1 for error, 0 if project name does not exist and 1 if it does
  	DIR *d;//duplicate function move to a helper file
  	struct dirent *dir;
  	d = opendir(".");
  
  	if (d == NULL) {
  		printf("Error: could not parse local files\n");
  		return -1;//error cond
  	}
  
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strncmp(projName, dir->d_name, strlen(projName)) == 0) {
				printf("Project '%s' exists\n", projName);
				return 1;//exists
			}
    	}
    	closedir(d);
  	}
  	printf("Project '%s' does not exist\n", projName);
  	return 0;//does not exist
}

void create(int fd_conn, char* proj) {
	/* NEED TO ADD DATA FOR .Manifest MUST FIGURE OUT CURRENTLY EMPTY */
	char manifest = "lol";

	if (projName_exists(proj) == 1) {
		printf("Error: project already exists on server\n");
		return;
	}

	int fd;
	char path[1024];
	snprintf(path, sizeof(path), "./%s", proj);
	
	fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 00644);
	if (fd < 0) {
		printf("Error: create() -> open()\n");
		return;
	}
	printf("project '%s' created\n", proj);
	path[0] = '\0';
	close(fd);
	
	snprintf(path, sizeof(path), "./%s/.Manifest", proj);
	fd = open(path, O_CREAT | O_RDWR, 00644);
	if (fd > 0) {
		if (write(fd, manifest, strlen(manifest)) > 0) {
			printf("project '%s/.Manifest' created\n", proj);
		}
		else {
			printf("Error: create() -> write()\n");
		}
	}
	else {
		printf("Error: create() -> open()\n");
	}
	close(fd);
	
	if (write(fd_conn, manifest, strlen(manifest)) > 0))
		printf("success .Manifest sent to client..\n");
		
	return;
}

void push(char *path) {
	char path_commit[1024];
	snprintf(path_commit, sizeof(path_commit), "/%s/.Commit", path);
	int fd = open(path_commit, O_CREAT | O_RDWR | O_TRUNC, 00644);
	
	char buffer[1024];
	char *str;
	int b_len = 0;
	int s_len = 0;
	while ((b_len = read(fd, buffer, 1023)) > 0) {
		s_len += b_len;
		str = realloc(str, s_len * sizeof(char));
		strncat(str, buffer, b_len);
	}//format for the commit should be [file name size]~[file name]~[file size]~[new file]~\n
	str[s_len] = '\0';
	
	while (*str) {//could definitely clean this up lol
		char len_s[6];
		int i = 0;
		while (*str != '~') {
			len_s[i] = *str;
			i++;
			str++;
		}
		len_s[i] = '\0';
	
		int len_i = atoi(len_s);
		char filename[len_i];
		str++;
		i = 0;
		while (*str != '~') {
			filename[i] = *str;
			i++;
			str++;
		}
		filename[i] = '\0';
		
		len_s[0] = '\0';
		i = 0;
		while (*str != '~') {
			len_s[i] = *str;
			i++
			str++;
		}
		len_i = atoi(len_s);
		char filedata[len_i];
		str++;
		i = 0;
		while(*str != '~') {
			filedata[i] = *str;
			i++;
			str++;
		}
		str += 2;
		
		char path_temp[1024];
		snprintf(path_temp, sizeof(path_temp), "/%s/%s", path, filename);
		int fd = open(path_temp, O_CREAT, O_RDWR, O_TRUNC, 00644);
		if (fd < 0) {
			printf("fd error\n");
			return;
		}
		if (write(fd, filedate, strlen(filedata)) != strlen(filedata)) {
			printf("write() error\n");
			return;
		}
		close(fd);
	}
}

void server_getCMD(int fd_conn) {//file descriptor for the connection	
	//commands format [sizeof message (8 bytes)][delimiter][command][delimiter][projects/files][delimiter[sizeof project/file]..
	//delim = '~'
	unsigned int msg_len_i = 0;
	char msg_len_s[6];
	msg_len_s[5] = '\0';
	
	if (read(fd_conn, msg_len_s, 5) != 5) {
		printf("Error: could not read size of msg\n");
		return;
	}
	
	msg_len_i = atoi(msg_len_s) - 4;
	
	char buffer[msg_len_i];
	buffer[msg_len_i - 1] = '\0';
	if (read(fd_conn, msg_len_s, msg_len_i) != msg_len_i) {
		printf("Error: could not read size of msg\n");
		return;
	}
	
	if (strncmp(buffer, "~ch", 3) == 0) {
		char* ptr;
		ptr = strrchr(buffer, '~');
		checkout(fd_conn, ptr + 1);
	}
	else if (strncmp(buffer, "~cr", 3) == 0) {
		char* ptr;
		ptr = strrchr(buffer, '~');
		create(fd_conn, ptr + 1);
	}
}

void server_START(char* _port) {
    int port;
    port = atoi(_port);
    
    int fd_sock, fd_conn;
    socklen_t len;
    struct sockaddr_in server_address, client_address;
    
    //socket create and verification
    fd_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_sock == -1) {
    	printf("Error: socket creation failed\n");
     	exit(0);
    }
    else
    	printf("socket creation success..\n");
    	
    bzero(&server_address, sizeof(server_address));
    
    // assign IP, PORT
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    
    // Binding newly created socket to given IP and verification
    if ((bind(fd_sock, (struct sockaddr*)&server_address, sizeof(server_address))) != 0) {
    	printf("Error: socket bind failed\n");
    	exit(0);
    }
    else {
    	printf("socket bind successful..\n");
   
    // Now server is ready to listen and verification
    	if ((listen(fd_sock, 5)) != 0) {
    		printf("Error: listen failed\n");
    		exit(0);
    	}
    	else
    		printf("listen successful..\n");
    	len = sizeof(client_address);
    
    // Accept the data packet from client and verification
    	fd_conn = accept(fd_sock, (struct sockaddr*)&client_address, &len);
    	if (fd_conn < 0) {
    		printf("Error: server accept failed\n");
    		exit(0);
    	}
    	else
        	printf("server accept success..\n");
    
    	server_getCMD(fd_conn);//get's the command sent by client
    }
    // After chatting close the socket
    close(fd_sock);
}

// Driver function
int main(int argc, char **argv) {
	server_START(argv[1]);
    //destroyFunction("ProjectZero");
    /*if(argc < 2){
        printf("Please enter port number\n");
        return 0;
    }*/
    
    return 0;
}
