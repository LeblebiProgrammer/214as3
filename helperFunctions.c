#include "helperFunctions.h"


char *concat(char *str1, char *str2, char delimeter){
    int length = strlen(str1) + 1 + strlen(str2) + 1;
    int count = strlen(str1)+1;
    if(delimeter == '\0'){
      length -= 1;
      count -= 1;
      //val[strlen(str1)] = delimeter;
    }
    char *val = (char*)malloc(sizeof(char)*length);

    strcpy(val, str1);
    if(delimeter != '\0'){
      val[strlen(str1)] = delimeter;
    }

    int i = 0;
    for(i = 0; i< strlen(str2); i++){
        val[count] = str2[i];
        count++;
    }
    val[count] = '\0';
		printf("%d len out concat\t", length);
    return val;
}

char *subString(char *str, char delimeter){
  char *ptr = strchr(str, delimeter);
  char *sub = NULL;
  if(ptr) {
     int index = ptr - str;
     int i = index + 1;

     int subLen = (strlen(str)-index);

     sub = (char*)malloc(sizeof(char)* subLen);
     int count = 0;
     for(; i< strlen(str); i++){
         sub[count] = str[i];
         count++;
     }

     sub[count] = '\0';

  }
  return sub;
}


int fileWriter(char *fpath, char *string, int writeLen,char create){

    if(create == '1'){
        int fd = open(fpath, O_CREAT | O_RDWR , 0666);
        if (fd != -1) {
        //printf("%s\n", str);
            write(fd, string, (strlen(string))*sizeof(char));
        }
        close(fd);
    }
    return 0;
}
