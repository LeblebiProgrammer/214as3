#ifndef HelperFunctions

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <ftw.h>

#include <netdb.h>
#include <dirent.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define CHUNK 16384

#define MAX 20

char *_read(char *path);

char *concat(char *str1, char *str2, char delimeter);

char *subString(char *str, char delimeter, char begin);

char *subIndexer(char *fileStr, char *word, char endChar);

int fileWriter(char *fpath, char *string, int writeLen,char create);

char *digitToString(int number, int length);

int digitCounter(int number);

char *msgPreparer(char *msg);

char *sockReader(int sockf);

void createDir(char *str);

char *folderFinder(char *path);

char *fileReader(char *path);

#endif
