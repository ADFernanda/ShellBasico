#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <unistd.h>

#define PROC_PATH "/proc"
#define STAT_FILENAME "stat"

#define MAX_DIGITS 20

#define INPUT_SEPARATORS " \n"

int *trashInt;

void termination_handler(int signum){
  free(trashInt);
  exit(signum);
}

void refreshInputSelect(struct timeval *tv, fd_set *set){
  tv->tv_sec = 5;
  tv->tv_usec = 0;
  FD_ZERO(set);
  FD_SET(STDIN_FILENO, set);
}

int main(int argc, char *argv[]){

  DIR *procsDir;
  struct dirent* readingProcs;
  FILE *procStatFile;
  char procName[BUFSIZ], filePath[NAME_MAX], procState, signalIn[255], *signalToken;
  long int numberCheck;
  struct stat fileStat;
  struct passwd *pwd;
  int procCount, keepLooping, signalingIndex, signalingStatus, signalPid, signalCode, selectReturn;
  struct winsize w;
  clock_t startTime;
  struct timeval inputInterval;
  fd_set inputFdSet;

  trashInt = (int*) malloc(sizeof(int));

  ioctl(0, TIOCGWINSZ, &w);
  refreshInputSelect(&inputInterval, &inputFdSet);

  if (signal (SIGINT, termination_handler) == SIG_IGN)
    signal (SIGINT, SIG_IGN);
  if (signal (SIGHUP, termination_handler) == SIG_IGN)
    signal (SIGHUP, SIG_IGN);
  if (signal (SIGTERM, termination_handler) == SIG_IGN)
    signal (SIGTERM, SIG_IGN);

  keepLooping = 1;
  signalingIndex = 0;
  signalingStatus = 0;

  while(keepLooping){

    printf("\e[2J\e[H\e[3J");

    if((procsDir = opendir(PROC_PATH)) == NULL){
      fprintf(stderr, "Nao foi possivel abrir o /proc\n");
      termination_handler(1);
    }

    printf("%8s|%16s|%24s|%8s|\n", "PID", "User", "PROCNAME", "Estado");
    printf("%8s|%16s|%24s|%8s|\n",  "--------", "----------------", "------------------------",
                                    "--------");
    procCount = 0;
    while((readingProcs = readdir(procsDir)) && procCount < (w.ws_row - 4)){
      if (!strcmp(readingProcs->d_name, ".")){
        continue;
      }
      if (!strcmp(readingProcs->d_name, "..")){
        continue;
      }
      numberCheck = strtol(readingProcs->d_name, NULL, 10);
      if(numberCheck == 0L || numberCheck == LONG_MAX || numberCheck == LONG_MIN){
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
          termination_handler(1);
        }
        else{
          if((pwd = getpwuid(fileStat.st_uid)) != NULL){
            if((procStatFile = fopen(filePath, "r")) == NULL){
              fprintf(stderr, "Erro ao abrir arquivo de stat: %s\n", filePath);
              termination_handler(1);
            }
            else{
              procCount++;
              fscanf(procStatFile, "%d", trashInt);
              fscanf(procStatFile, "%s", procName);
              fscanf(procStatFile, " %c", &procState);
              printf("%8s|%16s|%25s%c|%8c|\n",  readingProcs->d_name, pwd->pw_name,
                                                (procName + 1), 8, procState);
              if(fclose(procStatFile) != 0){
                fprintf(stderr, "Erro ao fechar arquivo de stat: %s\n", filePath);
                termination_handler(1);
              }
            }
          }
          else{
            fprintf(stderr, "Nao foi possivel achar o usuario de id: %d\n", fileStat.st_uid);
            termination_handler(1);
          }
        }
      }
    }

    if(closedir(procsDir) == -1){
      fprintf(stderr, "Erro ao fechar o /proc\n");
      termination_handler(1);
    }

    if((selectReturn = select(FD_SETSIZE, &inputFdSet, NULL, NULL, &inputInterval)) <= -1){
      fprintf(stderr, "Erro ao fazer select do input\n");
      termination_handler(1);

    }
    else if(selectReturn >= 0){
      fgets(signalIn, sizeof(signalIn), stdin);
      signalToken = strtok(signalIn, INPUT_SEPARATORS);
      if(strcmp(signalToken, "q") == 0){
        keepLooping = 0;
      }
      else{
        numberCheck = strtol(signalToken, NULL, 10);
        if(numberCheck != 0L && numberCheck != LONG_MAX && numberCheck != LONG_MIN){
          signalPid = numberCheck;
          signalToken = strtok(NULL, INPUT_SEPARATORS);
          numberCheck = strtol(signalToken, NULL, 10);
          if(numberCheck != 0L && numberCheck != LONG_MAX && numberCheck != LONG_MIN){
            signalCode = numberCheck;
            signalToken = strtok(NULL, INPUT_SEPARATORS);
            if(signalToken == NULL){
              if(kill(signalPid, signalCode) == -1){
                fprintf(stderr, "Nao foi possivel enviar sinal para o processo\n");
              }
            }
          }
        }
      }
    }
    refreshInputSelect(&inputInterval, &inputFdSet);
  }

  termination_handler(0);
}