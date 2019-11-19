#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "header.h"

int main(int argc, char *argv[]){
  char dir[50];
  if(argc<2){
    printf("No directory chosen\n");
    printf("Please choose a directory: \n");
    char buffer [50];
    while(fgets(buffer, 50 , stdin)){
      printf("Directory Chosen: %s\n", buffer);
      strcpy(dir,buffer);
      dir[strlen(dir)-1] = 0;
      break;
    }
  }
  else{
    printf("Directory Chosen: %s\n",argv[1]);
    strcpy(dir,argv[1]);
  }
  if(! opendir(dir)){
      printf("Error when attempting to open %s: %s\n",dir,strerror(errno));
      return 0;
  }
  printf("Statistics for directory: %s\n", dir);
  int fileSize = 0;
  fileSize = print_list_of_file_type(dir,DT_DIR,fileSize,1);
  fileSize = print_list_of_file_type(dir,DT_REG,fileSize,1);
  printf("\nTotal Directory Size:");
  fileSizePrinter(fileSize);
  printf("\n");
  return 0;
}

void fileSizePrinter(off_t size){
  float kiloSize = size/1024.0;
  float gigaSize = kiloSize / 1024;
  float megaSize = gigaSize /1024;
  if (megaSize > 1)
    printf(" %8.5f MB",megaSize);
  else if(gigaSize > 1)
    printf(" %8.5f GB",gigaSize);
  else if(kiloSize > 1)
    printf(" %8.5f KB",kiloSize);
  else
    printf(" %8ld  B",size);
}
void binaryPrint(int mode, unsigned char fileType){
  int i;
  char permissions [10];
  for(i=8;i>=0;i--){
    switch(mode%2){
      case 0:
        permissions[i] = '-';
        break;
      case 1:
        switch(i%3){
          case 0:
            permissions[i] = 'r';
            break;
          case 1:
            permissions[i] = 'w';
            break;
          case 2:
            permissions[i] = 'x';
            break;
        }
    }
    mode = mode>>1;
  }
  permissions[9] = 0;
  //If there's a directory, add a d
  if((fileType) == DT_DIR){
    printf("d");
  }
  else
    printf("-");
  printf("%s",permissions);
}
int print_list_of_file_type(char * dirName, unsigned char fileType, int fileSize, int numberOfTabs){
  DIR * dir = opendir(dirName);
  struct dirent * file = readdir(dir);

  char * file_type_in_words;
  //Choosing what file type the file is
  if(fileType == DT_DIR)
    file_type_in_words = "Directories";
  else
    file_type_in_words = "Regular Files";
  //Printing outer tab
  int j;
  for(j = 0;j<numberOfTabs;j++)
      printf("\t");
  printf("%s:\n",file_type_in_words);
  while(file){
    //Skipping if improoper file type
    if(file->d_type != fileType){
      file = readdir(dir);
      continue;
    }
    //Pritnting tabs again
    for(j = 0;j<numberOfTabs+1;j++)
        printf("\t");
    //Accesing proper file stat name
    struct stat sb;
    char fileStatName [5000];
    strncpy(fileStatName,dirName,sizeof(fileStatName)-257);
    strcat(fileStatName,"/");
    strcat(fileStatName,file->d_name);
    //Gettin stats
    stat(fileStatName,&sb);
    //Printing Permissions
    int mode = sb.st_mode;
    int killer = 256+128+64+32+16+8+4+2+1;
    mode = killer & mode;
    binaryPrint(mode,fileType);

    //Creating the file Size String
    fileSizePrinter(sb.st_size);
    fileSize+=sb.st_size;
    //Printing last time of access
    char access[25];
    strcpy(access, ctime(& (sb.st_atime)));
    char printer[13];
    int i;
    int j =0;
    for(i=0;i<24;i++){
      if(i>3 && i < 16){
        printer[j] = access[i];
        j++;
      }
    }
    printer[j] =0;
    printf(" %s",printer);
    //Pritning file name
    char fileName [256];
    strcpy(fileName,file->d_name);
    printf(" %s\n",fileName);
    //Recursive looping into subdirectory
    //Also I'm ignoring hiden directories as well as . and .. directories

    //Creating a string for a the first char
    char posDot[2];
    posDot[0] = fileName[0];
    posDot[1] = 0;
    if(fileType == DT_DIR && strcmp(".",posDot)){
      char nextDirectory [100];
      strcpy(nextDirectory,dirName);
      strcat(nextDirectory,"/");
      strcat(nextDirectory,fileName);
      fileSize = print_list_of_file_type(nextDirectory,DT_DIR,fileSize,numberOfTabs+2);
      fileSize = print_list_of_file_type(nextDirectory,DT_REG,fileSize,numberOfTabs+2);
    }
    //Reading new directory
    file = readdir(dir);
  }
  closedir(dir);
  return fileSize;
}
