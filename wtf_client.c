#include <netdb.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dirent.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void itoa_5(int src, char* dst) {
	int m = 10000;
	int n = 0;
	while (m > 0) {
		sprintf(dst + n, "%d", src / m);
		src %= m;
		m /= 10;
		n++;
	}
}

char* func(int fd_socket, int mode, char* projName) {
	char* msg = NULL;
	unsigned int msg_len_i = 0;
	char msg_len_s[6];//max size of an int is 32767 on these machines
	memcpy(msg_len_s, "00000\0", 6);

	if (mode == 1) {
		msg_len_i = (strlen(msg_len_s) + 1 + strlen("checkout") + 1 + strlen(projName) * sizeof(char));
		itoa_5(msg_len_i, msg_len_s);
		msg = (char*)malloc(msg_len_i);
		
		strncat(msg, msg_len_s, strlen(msg_len_s));
		strncat(msg, "~", 1);
		strncat(msg, "checkout", strlen("checkout"));
		strncat(msg, "~", 1);
		strncat(msg, projName, strlen(projName));

		//printf("%s\n", msg);
		write(fd_socket, msg, (strlen(msg) + 1) * sizeof(char));
		free(msg);
	}
	else if (mode == 6) {
		msg_len_i = ((strlen(msg_len_s) + 1 + strlen("create") + 1 + strlen(projName)) * sizeof(char));
		itoa_5(msg_len_i, msg_len_s);
		msg = (char*)malloc(msg_len_i);
		
		strncat(msg, msg_len_s, strlen(msg_len_s));
		strncat(msg, "~", 1);
		strncat(msg, "create", strlen("create"));
		strncat(msg, "~", 1);
		strncat(msg, projName, strlen(projName));
		
		write(fd_socket, msg, (strlen(msg) + 1) * sizeof(char));
		free(msg);
		
		char buffer[1024];
		char *manifest;
		int b_len = 0;
		int m_len = 0;
		while ((b_len = read(fd_socket, buffer, 1023)) > 0) {
			m_len += b_len;
			manifest = realloc(manifest, m_len * sizeof(char));
			strncat(manifest, buffer, b_len);
			manifest[m_len] = '\0';
		}
		
		create_e(projName, manifest);
	}
}

void connect_to_server(int mode, char* projName) {//rval = return values
	char _ip[128];
	char _port[128];
	getConfigure(_ip, _port);

    int fd_socket = -10; //int connection fd;
	struct sockaddr_in server_address;//socket;

	// socket create and varification
	fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1) {
		printf("Error: socket creation failure\n");
		return;
	}
	printf("socket creation success..\n");
	
	bzero(&server_address, sizeof(server_address));

	// assign IP, PORT
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(_ip);
    int port = atoi(_port);
	server_address.sin_port = htons(port);

	// connect the client socket to server socket
	if (connect(fd_socket, (struct sockaddr*)&server_address, sizeof(server_address)) != 0) {
		printf("Error: connection with the server failed\n");
		return;
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(fd_socket, mode, projName);

	// close the socket
	close(fd_socket);
}


void getConfigure(char* ip_dest, char* port_dest) {//make sure both dest have buffer room. function does not resize arguments
	int fd = open(".configure", O_RDONLY);//todo: prevent seg fault in getConfigure()
	if (fd < 0) {
		printf("Error: .configure does not exist\n");
		return;
	}
	
	char buffer[256];
	int buffer_length = read(fd, buffer, 256);
	if (buffer_length > 255) {
		printf("Error: .configure not set correctly\n");
		return;
	}
	buffer[buffer_length] = '\0';
	
	int buffer_index = 0;
	while (buffer[buffer_index] != '\n') {
		*ip_dest = buffer[buffer_index];
		buffer_index++;
		ip_dest++;
	}
	*ip_dest = '\0';
	
	buffer_index++;
	
	while (buffer_index < buffer_length) {
		*port_dest = buffer[buffer_index];
		buffer_index++;
		port_dest++;
	}
	*port_dest = '\0';
	
	printf(".configure read success..\n");
}

int configure(char* ip, char* port) {
	int fd = open(".configure", O_CREAT | O_TRUNC | O_RDWR, 00644);
	if (fd < 0) {
		printf("Error: .configure not created\n");
		return -1;
	}
	else {
		char* buffer;
		buffer = (char*)malloc((strlen(ip) + strlen(port) + 2) * sizeof(char));
		
		strncat(buffer, ip, strlen(ip));
		strncat(buffer + strlen(ip), "\n", 1);
		strncat(buffer + strlen(ip) + 1, port, strlen(port));
		buffer[strlen(ip) + 1 + strlen(port)] = '\0';

		if (write(fd, buffer, strlen(buffer)) != strlen(buffer)) {
			printf("Error: ip and/or port not written to .configure\n");
			close(fd);
			return -1;
		}
		
		printf("ip / port written to .configure..\n");
		close(fd);	
		return 1;
	}
}

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

void checkout(char* projName) {//checkout is mode==1
	if (projName_exists(projName) == 1) {
		printf("Error: checkout failure, '%s' already exists in client\n", projName);
		return;
	}

	connect_to_server(1, projName);
}

void create_p(projName) {//mode==6 & create_prep
	connect_to_server(projName, 6);
}

void create_e(char* proj, char* manifest) {//create_exec
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
	return;
}

int main(int argc, char** argv) {
	projName_exists("test");;
	//checkout("test");
	return 0;
}
