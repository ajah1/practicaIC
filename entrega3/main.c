#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdint.h>

/* Programa 'hola mundo' donde cada procesador requerido se identifica,
basado en ejemplos orginales de Tim Kaiser (http://www.sdsc.edu/~tkaiser),
del San Diego Supercomputer Center, en California */

int main(int argc, char **argv)
{



 int rank, size;
 
 uint8_t buffer[10][10];
  
  for(int i = 0; i < 10; i++ ){
	  for(int j = 0; j < 10; j++){
		  buffer[i][j] = 5;
		  printf("%d ", buffer[i][j]);
		  
		 }
		 
		 printf("\n");
	 }

 
 
 
    MPI_Init(&argc, &argv); // Inicializamos las hebras
    MPI_Comm cliente;
    MPI_Status status;
   char portname[MPI_MAX_PORT_NAME];
    
    

 
   MPI_Comm_size(MPI_COMM_WORLD,&size);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
 
   MPI_Open_port(MPI_INFO_NULL, portname);
   printf("portname: %s\n", portname);
   MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_SELF, &cliente);
   printf("client connected\n");
    
    
    
   int dest = 0;
	
 
 
    MPI_Send(&buffer //referencia al vector de elementos a enviar
    		,10*10 // tamaño del vector a enviar
    		,MPI_BYTE // Tipo de dato que envias
    		,dest // pid del proceso destino
    		,0 //etiqueta
    		,cliente); //Comunicador por el que se manda
    
      MPI_Close_port(portname);
    
    MPI_Finalize();
    return 0;


	
}
