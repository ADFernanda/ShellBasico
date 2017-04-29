#include <stdio.h>
#include <stdlib.h>
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
	char nome[NAME_MAX];

}t_Processo;

int ContaProcessos(){
	DIR *folder_proc;
	struct dirent* in_proc;
	int checkFolder=0, numProcs=0;

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
  	return numProcs;
}

void PreencheDadosProcessos(t_Processo *processos, int numProcs){
	int i=0, checkFolder=0;
	DIR *folder_proc=NULL;
	struct dirent* in_proc=NULL;
	char nameDir[NAME_MAX];
	FILE *file_statX=NULL;

	if ((folder_proc = opendir (PROC_PATH)) == NULL) 
    {
        fprintf(stderr, "Erro ao abrir o diretorio proc\n");
        return;
    }

    while ((in_proc = readdir(folder_proc))){    
    
    	//caso o número de processos tenha mudado desde o início do processamento
    	if(i>=numProcs){
    		break;
    	}

    	strcpy(nameDir, PROC_PATH);
	    strcat(nameDir, "/");
	    strcat(nameDir, in_proc->d_name);
	    strcat(nameDir, "/");
	    strcat(nameDir, STAT_FILENAME);
	    //printf("\n\n%s\n\n",nameDir );	 

    	checkFolder = strtol(in_proc->d_name, NULL,10);
   		
    	if(checkFolder != 0L && checkFolder != LONG_MAX && checkFolder != LONG_MIN){
    		//lê arquivos de stat
    		if((file_statX = fopen(nameDir, "r")) == NULL){
	            fprintf(stderr, "Erro ao abrir arquivo de stat: %s\n", nameDir);
	        }else{
	            fscanf(file_statX, "%d", &processos[i].pid);
	            fscanf(file_statX, " %s ", processos[i].nome);
	        }	        

			// printf("%d %s\n",processos[i].pid, processos[i].nome);

	        if(fclose(file_statX) != 0){
            	fprintf(stderr, "Erro ao fechar arquivo de stat: %s\n", nameDir);
            }
            i++;
		}		
	}
	if(closedir(folder_proc) == -1){
	    fprintf(stderr, "Erro ao fechar o /proc\n");
	    return;
  	}

  	// for(i=0;i<numProcs;i++){
  	// 	printf("%d\n",processos[i].pid );
  	// }

}

int PesquisaBinaria (int pidBuscado, t_Processo *vector, int e, int d){
 int i = (e + d)/2;
 if (vector[i].pid == pidBuscado || (pidBuscado==0) )
    return i;
 if (e >= d)
    return -1; // Não foi encontrado
 else
     if (vector[i].pid  < pidBuscado)
        return PesquisaBinaria(pidBuscado, vector, i+1, d);
     else
        return PesquisaBinaria(pidBuscado, vector, e, i-1);
}

// void ImprimePSTree(t_Processo *processos, int posicaoPai, int numProc, int maxProcs){

// 	FILE *file_childrenX=NULL;
// 	char nameDir[NAME_MAX], aux[NAME_MAX];
// 	int checkFolder=0, child=0, i=0,n=0;

// 	if(numProc == maxProcs){
// 		return;
// 	}

// 	//printf("%d %s\n",processos[posicaoPai].pid, processos[posicaoPai].nome );

// 	//monta nome do arquivo children
// 	n=sprintf (aux,"%d",processos[posicaoPai].pid );
// 	strcpy(nameDir, PROC_PATH);
//     strcat(nameDir, "/");
//     strcat(nameDir, aux);
//     strcat(nameDir, TASK_PATH);
//     strcat(nameDir, "/");
//     strcat(nameDir, aux);
//     strcat(nameDir, "/");
//     strcat(nameDir, CHILDREN_FILENAME);

//     if((file_childrenX = fopen(nameDir, "r")) == NULL){
//         return;
//     }

// 	//lê children
// 	while(!feof(file_childrenX)){
// 		fscanf(file_childrenX, "%d", &child);
		
// 		if(child==0){
// 			return;
// 		}	        		
// 		printf("%d %s\n\t",processos[posicaoPai].pid, processos[posicaoPai].nome );
// 		//printf("Pai: %d filho:%d \n", processos[posicaoPai].pid, child);
// 		posicaoPai = PesquisaBinaria(child, processos, 0, maxProcs);
// 		if(posicaoPai == -1){	        			
// 			return;
// 		}
		
// 		numProc++;		
// 		ImprimePSTree(processos, posicaoPai, numProc, maxProcs);

// 	}        		        
// }

void LeChildren(int **matrix, int numProcs,t_Processo *processos){

	FILE *file_childrenX=NULL;
	char nameDir[NAME_MAX], aux[NAME_MAX];
	int i=0, j=0, n=0, child=0;

    for(i=0;i<numProcs;i++){
    	j=0;
    	child=0;
    	//monta nome do arquivo children
    	n=sprintf (aux,"%d",processos[i].pid );
		strcpy(nameDir, PROC_PATH);
		strcat(nameDir, "/");
		strcat(nameDir, aux);
		strcat(nameDir, TASK_PATH);
		strcat(nameDir, "/");
		strcat(nameDir, aux);
		strcat(nameDir, "/");
		strcat(nameDir, CHILDREN_FILENAME);

		matrix[i][j] = processos[i].pid;
		j++;
		//printf("pai: %d\n", matrix[i][j]);

		if((file_childrenX = fopen(nameDir, "r")) == NULL){
            fprintf(stderr, "Erro ao abrir arquivo: %s\n", nameDir);
	    }else{

			//lê children
			while(!feof(file_childrenX)){
				fscanf(file_childrenX, "%d", &child);
				
				if(child!=0){
					matrix[i][j] = child;
				}
				j++;	        					
			}

		    
		}
		if(fclose(file_childrenX) != 0){
        	fprintf(stderr, "Erro ao fechar arquivo %s\n", nameDir);
        }


    }

}

int main(){

	FILE *file_childrenX=NULL;
	char nameDir[NAME_MAX], aux[NAME_MAX];	
	struct dirent* in_proc=NULL;
	int checkFolder=0, pid=0, numProcs=0, child=0, i=0,n=0, posicaoProcesso=0, j=0;
	t_Processo *processos=NULL, processo;	
	int **matrix = NULL;

	numProcs = ContaProcessos();
	processos = (t_Processo*) calloc (numProcs+1,sizeof(t_Processo));
	matrix = (int**) calloc (numProcs+1,sizeof(int*));
	PreencheDadosProcessos(processos, numProcs);

	for(i=0;i<=numProcs;i++){
		matrix[i] = (int*) calloc (numProcs,sizeof(int*));		
	}	
	
	LeChildren(matrix, numProcs,processos);

	for(i=0;i<numProcs;i++){
		for(j=0;j<10;j++){
			printf("%d ",matrix[i][j]);
		}
		printf("\n");
	}



	free(processos);
	for(i=0;i<=numProcs;i++){
		free(matrix[i]);
	}	
	free(matrix);
	return 0;
}