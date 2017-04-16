#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <pwd.h>

#define PROC_PATH "/proc"
#define STAT_FILENAME "stat"

int main(char *argv[], int argc){

  DIR *procsDir;
  struct dirent* readingProcs;
  FILE *procStatFile;
  char procName[BUFSIZ], filePath[NAME_MAX], procState;
  long int pidCheck;
  struct stat fileStat;
  struct passwd *pwd;
  int *trashInt;

  if((procsDir = opendir(PROC_PATH)) == NULL){
    fprintf(stderr, "Nao foi possivel abrir o /proc\n");
    return 1;
  }
  printf("%8s|%16s|%24s|%8s|\n", "PID", "User", "PROCNAME", "Estado");
  printf("%8s|%16s|%24s|%8s|\n",  "--------", "----------------", "------------------------",
                                  "--------");
  trashInt = (int*) malloc(sizeof(int));
  while(readingProcs = readdir(procsDir)){
    if (!strcmp(readingProcs->d_name, ".")){
      continue;
    }
    if (!strcmp(readingProcs->d_name, "..")){
      continue;
    }
    pidCheck = strtol(readingProcs->d_name, NULL, 10);
    if(pidCheck == 0L || pidCheck == LONG_MAX || pidCheck == LONG_MIN){
      //fprintf(stderr, "Diretorio nao de processo: %s\n", readingProcs->d_name);
    }
    else{
      strcpy(filePath, PROC_PATH);
      strcat(filePath, "/");
      strcat(filePath, readingProcs->d_name);
      strcat(filePath, "/");
      strcat(filePath, STAT_FILENAME);
      if(stat(filePath, &fileStat) == -1){
        fprintf(stderr, "Erro ao pegar stat do arquivo: %s\n", filePath);
      }
      else{
        if((pwd = getpwuid(fileStat.st_uid)) != NULL){
          //printf("Estado\t|\n");
          if((procStatFile = fopen(filePath, "r")) == NULL){
            fprintf(stderr, "Erro ao abrir arquivo de stat: %s\n", filePath);
          }
          else{
            fscanf(procStatFile, "%d", trashInt);
            fscanf(procStatFile, "%s", procName);
            fscanf(procStatFile, " %c", &procState);
            printf("%8s|%16s|%25s%c|%8c|\n",  readingProcs->d_name, pwd->pw_name, (procName + 1), 8,
                                              procState);
            if(fclose(procStatFile) != 0){
              fprintf(stderr, "Erro ao fechar arquivo de stat: %s\n", filePath);
            }
          }
        }
        else{
          printf("Nao foi possivel achar o usuario de id: %d\n", fileStat.st_uid);
        }
      }
    }
  }
  free(trashInt);
  if(closedir(procsDir) == -1){
    fprintf(stderr, "Erro ao fechar o /proc\n");
    return 1;
  }
  return 0;
}