#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <limits.h>

#define PROC_PATH "/proc"
#define TASK_PATH "/task"
#define STAT_FILENAME "stat"
#define CHILDREN_FILENAME "children"

typedef struct t_Processo{
	int pid;
	char *nome;

}t_Processo;

int ContaProcessos(){
	if ((folder_proc = opendir (PROC_PATH)) == NULL) 
    {
        fprintf(stderr, "Erro ao abrir o diretorio proc\n");
        return 1;
    }

    while ((in_proc = readdir(folder_proc))) 
    {
    	checkFolder = strtol(in_proc->d_name, NULL,10);

   		//lê apenas pastas que são processos (nomes são números)	
    	if(checkFolder != 0L && checkFolder != LONG_MAX && checkFolder != LONG_MIN){    		
    		numProcs++;    		
		}		
	}
	if(closedir(folder_proc) == -1){
	    fprintf(stderr, "Erro ao fechar o /proc\n");
	    return 1;
  	}	
}

void PreencheDadosProcessos(t_Processo *processos, int numProcs){
	int i=0;

	if ((folder_proc = opendir (PROC_PATH)) == NULL) 
    {
        fprintf(stderr, "Erro ao abrir o diretorio proc\n");
        return 1;
    }

    while ((in_proc = readdir(folder_proc))){    
    
	    strcpy(nameDir, PROC_PATH);
	    strcat(nameDir, "/");
	    strcat(nameDir, in_proc->d_name);
	    strcat(nameDir, "/");
	    strcat(nameDir, STAT_FILENAME);

    	checkFolder = strtol(in_proc->d_name, NULL,10);
   		
    	if(checkFolder != 0L && checkFolder != LONG_MAX && checkFolder != LONG_MIN){    		
    		numProcs++;
    		//lê arquivos de stat
    		if((file_statX = fopen(nameDir, "r")) == NULL){
		            fprintf(stderr, "Erro ao abrir arquivo de stat: %s\n", nameDir);
	        }else{
	            fscanf(file_statX, "%d", &processos.pid);
	            fscanf(file_statX, "%s", processos.nome);
	        }
	        Enfileira(NovoProc, processos);

	        Retirado = Desenfileira(processos);

			printf("\n\n%d %d %s\n\n", Retirado.pid, Retirado.ppid, Retirado.name);

	        if(fclose(file_statX) != 0){
            	fprintf(stderr, "Erro ao fechar arquivo de stat: %s\n", nameDir);
            }
		}
		i++;
	}
	if(closedir(folder_proc) == -1){
	    fprintf(stderr, "Erro ao fechar o /proc\n");
	    return 1;
  	}
}

int main(){

	FILE *file_statX=NULL, *file_childrenX=NULL; 
	DIR *folder_proc;
	char buf[1000], pName[1000], nameDir[NAME_MAX];	
	struct dirent* in_proc;
	int checkFolder=0, pid=0, numProcs=0, *children;
	t_Processo *processos;	

	numProcs = ContaProcessos();
	processos = (t_Processo*)malloc(sizeof(t_Processo));
	children = (int*)malloc(sizeof(int));

	PreencheDadosProcessos(processos, numProcs);

	if ((folder_proc = opendir (PROC_PATH)) == NULL) 
    {
        fprintf(stderr, "Erro ao abrir o diretorio proc\n");
        return 1;
    }

    while ((in_proc = readdir(folder_proc))) 
    {    	   
    	checkFolder = strtol(in_proc->d_name, NULL,10);

   		//lê apenas pastas que são processos 	
    	if(checkFolder != 0L && checkFolder != LONG_MAX && checkFolder != LONG_MIN){    		
    		
    		//monta nome do arquivo children
    		strcpy(nameDir, PROC_PATH);
		    strcat(nameDir, "/");
		    strcat(nameDir, in_proc->d_name);
		    strcat(nameDir, "/");
		    strcat(nameDir, TASK_FILENAME);
		    strcat(nameDir, "/");
		    strcat(nameDir, in_proc->d_name);
		    strcat(nameDir, "/");
		    strcat(nameDir, CHILDREN_FILENAME);

    		//lê arquivos de children
    		if((file_childrenX = fopen(nameDir, "r")) == NULL){
		            fprintf(stderr, "Erro ao abrir arquivo: %s\n", nameDir);
	        }else{


	            fscanf(file_statX, "%d", &NovoProc.pid);
	            fscanf(file_statX, "%s", NovoProc.name);
	            fscanf(file_statX, " %c ", &trash);
	            fscanf(file_statX, "%d", &NovoProc.ppid);
	        }

	        if(fclose(file_statX) != 0){
            	fprintf(stderr, "Erro ao fechar arquivo de stat: %s\n", nameDir);
            }

            //  

		}
		//
	}	

	//cria grafo de processos

	//

    //free(pstreeHead);
    if(closedir(folder_proc) == -1){
	    fprintf(stderr, "Erro ao fechar o /proc\n");
	    return 1;
  	}

	return 0;
}