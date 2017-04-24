#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <limits.h>

#define PROC_PATH "/proc"
#define STAT_FILENAME "stat"

// typedef struct pstreeNode{
// 	char name[128];
// 	long int pid;
// 	long int ppid;
// 	struct pstreeNode *parent;
// 	struct FilaProcs *children;
// 	struct pstreeNode *next;
// }pstreeNode;

// static struct pstreeNode *pstreeHead;

//estrutura de fila
typedef struct TipoItem {
	char name[128];
	long int pid;
	long int ppid;
} TipoItem;
typedef struct Celula *Apontador;
typedef struct Celula {
	TipoItem Item;
	Apontador Prox;
} Celula;

typedef struct FilaProcs {
	Apontador Frente, Tras;
} FilaProcs;

void FFVazia(FilaProcs *Fila){
	Fila->Frente = (Apontador) malloc(sizeof(Celula));
	Fila->Tras = Fila->Frente;
	Fila->Frente->Prox = NULL;
}

int Vazia(FilaProcs *Fila){
return (Fila->Frente == Fila->Tras);
}

void Enfileira(TipoItem x, FilaProcs *Fila){
	Fila->Tras->Prox = (Apontador) malloc(sizeof(Celula));
	Fila->Tras = Fila->Tras->Prox;
	Fila->Tras->Item = x;
	Fila->Tras->Prox = NULL;
}

TipoItem Desenfileira(FilaProcs *Fila){
	Apontador q;
	if (Vazia(Fila)) {
		printf("Erro: fila está vazia\n");
	}
	q = Fila->Frente;
	Fila->Frente = Fila->Frente->Prox;
	free(q);
	return Fila->Frente->Item;
}
//fim da estrutura de fila

int main(){

	FILE *file_statX=NULL; 
	DIR *folder_proc;
	char buf[1000], pName[1000], nameDir[NAME_MAX], trash, string[100];	
	struct dirent* in_proc;
	int checkFolder=0, pid=0, ppid=0, teste=0;
	int numProcs=0;
	FilaProcs *processos= (FilaProcs*)malloc(sizeof(FilaProcs));
	TipoItem NovoProc, Retirado;

	FFVazia(processos);

	if ((folder_proc = opendir (PROC_PATH)) == NULL) 
    {
        fprintf(stderr, "Erro ao abrir o diretorio proc\n");
        return 1;
    }

    while ((in_proc = readdir(folder_proc))) 
    {
    
	    strcpy(nameDir, PROC_PATH);
	    strcat(nameDir, "/");
	    strcat(nameDir, in_proc->d_name);
	    strcat(nameDir, "/");
	    strcat(nameDir, STAT_FILENAME);

    	checkFolder = strtol(in_proc->d_name, NULL,10);

   		//lê apenas pastas que são processos 	
    	if(checkFolder != 0L && checkFolder != LONG_MAX && checkFolder != LONG_MIN){    		

    		//lê arquivos de stat
    		if((file_statX = fopen(nameDir, "r")) == NULL){
		            fprintf(stderr, "Erro ao abrir arquivo de stat: %s\n", nameDir);
	        }else{

	        	// fgets(string,30,file_statX );
	        	// printf("%s\n", string);

	            fscanf(file_statX, "%d", &pid);
	            fscanf(file_statX, "%s", pName);
	            fscanf(file_statX, "%c", &trash);
	            fscanf(file_statX, "%d", &ppid);
	            printf("\n%d %d %s\n", pid, ppid, pName);
	        }
	        //Enfileira(NovoProc, processos);

	        //Retirado = Desenfileira(processos);

			//printf("\n\n%li %li %s\n\n", Retirado.pid, Retirado.ppid, Retirado.name);

	        if(fclose(file_statX) != 0){
            	fprintf(stderr, "Erro ao fechar arquivo de stat: %s\n", nameDir);
            }

            //  

		}
		//
	}	

    //free(pstreeHead);
    if(closedir(folder_proc) == -1){
	    fprintf(stderr, "Erro ao fechar o /proc\n");
	    return 1;
  	}

	return 0;
}