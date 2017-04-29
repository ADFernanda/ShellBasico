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
#include <curses.h>

#define PROC_PATH "/proc"
#define STAT_FILENAME "stat"

#define MAX_DIGITS 20

using namespace std;

int *trashInt;

void termination_handler(int signum){
  free(trashInt);
  endwin();
}

int main(int argc, char *argv[]){

  DIR *procsDir;
  struct dirent* readingProcs;
  FILE *procStatFile;
  char procName[BUFSIZ], filePath[NAME_MAX], procState, signalIn[MAX_DIGITS + 1], charbuf;
  long int pidCheck;
  struct stat fileStat;
  struct passwd *pwd;
  int procCount, keepLooping, signalingIndex, signalingStatus, signalPid, signalCode, scrX, scrY;
  struct winsize w;

  trashInt = (int*) malloc(sizeof(int));

  initscr();
  keypad(stdscr, TRUE);
  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
  ioctl(0, TIOCGWINSZ, &w);

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

    if((charbuf = getch()) == ERR){
      clear();
      if((procsDir = opendir(PROC_PATH)) == NULL){
        printw("Nao foi possivel abrir o /proc\n");
        termination_handler(0);
        return 1;
      }

      printw("%8s|%16s|%24s|%8s|\n", "PID", "User", "PROCNAME", "Estado");
      printw("%8s|%16s|%24s|%8s|\n",  "--------", "----------------", "------------------------",
                                      "--------");
      procCount = 0;
      while((readingProcs = readdir(procsDir)) && procCount < (w.ws_row - 4)){
        if (!strcmp(readingProcs->d_name, ".")){
          continue;
        }
        if (!strcmp(readingProcs->d_name, "..")){
          continue;
        }
        pidCheck = strtol(readingProcs->d_name, NULL, 10);
        if(pidCheck == 0L || pidCheck == LONG_MAX || pidCheck == LONG_MIN){
          //printw("Diretorio nao de processo: %s\n", readingProcs->d_name);
        }
        else{
          strcpy(filePath, PROC_PATH);
          strcat(filePath, "/");
          strcat(filePath, readingProcs->d_name);
          strcat(filePath, "/");
          strcat(filePath, STAT_FILENAME);
          if(stat(filePath, &fileStat) == -1){
            printw("Erro ao pegar stat do arquivo: %s\n", filePath);
          }
          else{
            if((pwd = getpwuid(fileStat.st_uid)) != NULL){
              if((procStatFile = fopen(filePath, "r")) == NULL){
                fprintf(stderr, "Erro ao abrir arquivo de stat: %s\n", filePath);
              }
              else{
                procCount++;
                fscanf(procStatFile, "%d", trashInt);
                fscanf(procStatFile, "%s", procName);
                fscanf(procStatFile, " %c", &procState);
                printw("%8s|%16s|%25s%c|%8c|\n",  readingProcs->d_name, pwd->pw_name, (procName + 1), 8,
                                                  procState);
                if(fclose(procStatFile) != 0){
                  printw("Erro ao fechar arquivo de stat: %s\n", filePath);
                }
              }
            }
            else{
              printw("Nao foi possivel achar o usuario de id: %d\n", fileStat.st_uid);
            }
          }
        }
      }

      if(closedir(procsDir) == -1){
        printw("Erro ao fechar o /proc\n");
        termination_handler(0);
        return 1;
      }

      delay_output(1000);
    }

    else{
      if(signalingStatus == 0 && charbuf == 'q'){
        keepLooping = 0;
      }
      else if(charbuf >= '0' && charbuf <= '9'){
        if(signalingStatus == 0){
          signalingStatus = 1;
        }
        if(signalingIndex >= MAX_DIGITS){
          termination_handler(0);
          printw("estourou o maximo de digitos no sinal\n");
          return 1;
        }
        signalIn[signalingIndex] = charbuf;
        signalingIndex++;
        signalIn[signalingIndex] = '\0';
      }
      else if(charbuf == ' '){
        if(signalingStatus == 1){
          signalPid = strtol(signalIn, NULL, 10);
          signalingIndex = 0;
          signalIn[signalingIndex] = '\0';
          signalingStatus = 2;
        }
      }
      else if(charbuf == '\n'){
        if(signalingStatus == 2){
          signalCode = strtol(signalIn, NULL, 10);
          signalingIndex = 0;
          signalIn[signalingIndex] = '\0';
          signalingStatus = 0;
          if(kill(signalPid, signalCode) == -1){
            printw("nao foi possivel enviar o sinal\n");
          }
        }
      }
    }

    getyx(stdscr, scrY, scrX);
    move(scrY, 0);
    clrtoeol();

    if(signalingStatus == 2){
      printw("%d ", signalPid);
    }
    if(signalingStatus >= 1){
      printw("%s", signalIn, strlen(signalIn));
    }

    refresh();
    
  }
  termination_handler(0);
  return 0;
}