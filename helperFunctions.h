#ifndef HelperFunctions

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>


#define CHUNK 16384

char *concat(char *str1, char *str2, char delimeter);

char *subString(char *str, char delimeter);

int fileWriter(char *fpath, char *string, int writeLen,char create);

#endif
