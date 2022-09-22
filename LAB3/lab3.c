//LAB 3


#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"
#include<time.h>

float *matriz; //matriz que será carregada do arquivo
float *matriz2; //matriz 2 que será carregada do arquivo
float *matriz_saida;
int linhas, colunas, linhas2, colunas2; //dimensoes da matriz
int nthreads;

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int dim; //dimensao das estruturas de entrada
} tArgs;

void * tarefa(void *arg){
   tArgs *args = (tArgs*) arg;
   printf("Thread %d\n", args->id);
   for(int i=args->id; i<args->dim; i+=nthreads)
      for(int j=0; j<args->dim; j++)
         matriz_saida[j]+= matriz[i*(args->dim) + j] * matriz2[j];
   pthread_exit(NULL);
}   
   
int main(int argc, char*argv[]) {
   int dim;
   pthread_t *tid;
   tArgs *args;
   double inicio,fim,delta;
   long long int tam, tam2; //qtde de elementos na matriz
   FILE * descritorArquivo; //descritor do arquivo de entrada
   FILE * descritorArquivo2; //descritor do arquivo de entrada 2
   FILE * descritorArquivo_saida; //descritor do arquivo de saida
   size_t ret; //retorno da funcao de leitura no arquivo de entrada
   
   GET_TIME(inicio);
   
   //recebe os argumentos de entrada
   if(argc < 5) {
      fprintf(stderr, "Digite: %s <arquivo entrada> <arquivo entrada2> <arquivo saída> <quantidade de threads>\n", argv[0]);
      return 1;
   }
   
   nthreads = atoi(argv[4]);
   
   //abre o arquivo para leitura binaria
   descritorArquivo = fopen(argv[1], "rb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo de entrada\n");
      return 2;
   }

   //abre o arquivo para leitura binaria 2
   descritorArquivo2 = fopen(argv[2], "rb");
   if(!descritorArquivo2) {
      fprintf(stderr, "Erro de abertura do arquivo de entrada2\n");
      return 2;
   }

   //le as dimensoes da matriz
   ret = fread(&linhas, sizeof(int), 1, descritorArquivo);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   ret = fread(&colunas, sizeof(int), 1, descritorArquivo);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   tam = linhas * colunas; //calcula a qtde de elementos da matriz
   dim = linhas;

    //le as dimensoes da matriz 2
   ret = fread(&linhas2, sizeof(int), 1, descritorArquivo2);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo2 \n");
      return 3;
   }
   ret = fread(&colunas2, sizeof(int), 1, descritorArquivo2);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo2 \n");
      return 3;
   }
   tam2 = linhas2 * colunas2; //calcula a qtde de elementos da matriz


   //aloca memoria para a matriz
   matriz = (float*) malloc(sizeof(float) * tam);
   if(!matriz) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
   }

   //aloca memoria para a matriz 2
   matriz2 = (float*) malloc(sizeof(float) * tam2);
   if(!matriz2) {
      fprintf(stderr, "Erro de alocao da memoria da matriz2\n");
      return 3;
   }
   
   //aloca memoria para a matriz saída
   matriz_saida = (float*) malloc(sizeof(float) * linhas * colunas2);
   if(!matriz_saida) {
      fprintf(stderr, "Erro de alocao da memoria da matriz_saida\n");
      return 3;
   }

   //carrega a matriz de elementos do tipo float do arquivo
   ret = fread(matriz, sizeof(float), tam, descritorArquivo);
   if(ret < tam) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
   }
   
    //carrega a matriz2 de elementos do tipo float do arquivo
   ret = fread(matriz2, sizeof(float), tam2, descritorArquivo2);
   if(ret < tam2) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
   }
   GET_TIME(fim);
   delta= fim - inicio;
   printf("Tempo de inicializacao: %lf\n",delta);
   
   
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   args = (tArgs*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   //criacao das threads
   GET_TIME(inicio);
   for(int i=0; i<nthreads; i++){
     (args+i)->id = i;
     (args+i)->dim = dim;
     if(pthread_create(tid+i,NULL, tarefa, (void*) (args+i))){
     	puts("ERRO--pthread_create");return 3;
     	}
   }  	
   //espera pelo termino das threads
   for(int i=0; i<nthreads; i++){
   	pthread_join(*(tid+i), NULL);
   }
   //multiplicacao das matrizes

   GET_TIME(fim);
   delta= fim - inicio;
   printf("Tempo de processamento: %lf\n",delta);
   
   //escreve a matriz no arquivo
   //abre o arquivo para escrita binaria
   GET_TIME(inicio);
   descritorArquivo_saida = fopen(argv[3],"wb");
   if(!descritorArquivo_saida){
   	fprintf(stderr, "Erro de abertura do arquivo de saida\n");
   	return 5;
   }
   
   //escreve os elementos da matriz
   ret = fwrite(matriz_saida, sizeof(float), tam2, descritorArquivo_saida);
   if(ret < tam2){
   	fprintf(stderr, "Erro de escrita no arquivo de saida");
   	return 6;
   }   	
   free(matriz);
   free(matriz2);
   free(matriz_saida);
   free(args);
   free(tid);
   GET_TIME(fim);
   delta= fim - inicio;
   printf("Tempo de finalizacao: %lf\n",delta);
   
   return 0;
}
