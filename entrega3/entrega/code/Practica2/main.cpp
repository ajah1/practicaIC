#include <string.h>
#include <sys/resource.h>

#include "lib/EasyBMP.h"
#include <omp.h>

#define N 1000
#define CHUNKSIZE 100

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define MAX 160                /* number of rows in matrix A */
#define NRA 80                 /* number of rows in matrix A */
#define NCA 80                /* number of columns in matrix A */
#define NCB 80                  /* number of columns in matrix B */



#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */



using namespace std;



long double
tiempo(void){
 struct rusage usage;
 getrusage(RUSAGE_SELF,&usage);
 return( (long double) usage.ru_utime.tv_sec +
	 (long double) usage.ru_utime.tv_usec/1e6 
	 /* + (long double) usage.ru_stime.tv_sec +
         (long double) usage.ru_stime.tv_usec/1e6 */  
       );
}






uint8_t bilinealInterpolation(uint8_t pixela1, uint8_t pixela2, uint8_t pixelb1, uint8_t pixelb2, float x, float y){


//cout << (int)pixela1.Red;

    float newColor = pixela1 * (1 - x) * ( 1 - y) +
            pixelb1 * (x) * (1 - y) +
            pixela2 * (1-x) * (y) +
            pixelb2 * x * y;

    

    return (int)newColor;

}



void interpolate(int i, int j,uint8_t OriginalImage[][NRA], uint8_t FinalImage[][MAX], int height, int width ){


	                        if(j%2 == 1){ // Si se trata de una columna impar
                            if(i != 0 && j != 0 && i < width - 1  && j < height -1){ // no estamos en un borde
                                    uint8_t pixel1 = OriginalImage[i/2][j/2];
                                    uint8_t pixel2 = OriginalImage[(i+1)/2][j/2];
                                    uint8_t pixel3 = OriginalImage[i/2][(j+1)/2];
                                    uint8_t pixel4 = OriginalImage[(i+1)/2][(j+1)/2];

                                    uint8_t tmp = bilinealInterpolation( pixel1,pixel2,pixel3,pixel4,0.45,0.45 );  
                                    
                                    FinalImage[i][j] = tmp;
                                    //FinalImage.SetPixel(i,j,tmp); 

                            }
                        }else if(i%2 == 1){// si se trata de una fila impar
                            if(i != 0 && j != 0 && i < width - 1  && j < height -1){ // no estamos en un borde
								
				                    uint8_t pixel1 = OriginalImage[i/2][j/2];
                                    uint8_t pixel2 = OriginalImage[(i+1)/2][j/2];
                                    uint8_t pixel3 = OriginalImage[i/2][(j+1)/2];
                                    uint8_t pixel4 = OriginalImage[(i+1)/2][(j+1)/2];

                                    uint8_t tmp = bilinealInterpolation( pixel1,pixel2,pixel3,pixel4,0.55,0.55 );  
                                    
                                    FinalImage[i][j] = tmp;
                                    
                                    
                                 
                            }
                        }else{// una copia exacta del pixel original
                               
                                 FinalImage[i][j] = OriginalImage[i/2][j/2];

                                //FinalImage.SetPixel(i,j,OriginalImage.GetPixel(i/2,j/2));
                            
                        }
}




//Funcion de interpolación:
//Esta funcion recibe 4 pixeles por parametro ademas, de un punto (x,y) que define el peso final de cada pixel
//De esta forma se puede calibrar para que tengan unos pixeles mas importancia que otra.
RGBApixel bilinealInterpolation(RGBApixel pixela1, RGBApixel pixela2, RGBApixel pixelb1, RGBApixel pixelb2, float x, float y){

    RGBApixel tmp;

//cout << (int)pixela1.Red;

    float newRed = pixela1.Red * (1 - x) * ( 1 - y) +
            pixelb1.Red * (x) * (1 - y) +
            pixela2.Red * (1-x) * (y) +
            pixelb2.Red * x * y;

    float newGreen = pixela1.Green * (1 - y) * ( 1 - y) +
            pixelb1.Green * (x) * (1 - y) +
            pixela2.Green * (1-x) * (y) +
            pixelb2.Green * x * y;

    float newBlue = pixela1.Blue * (1 - y) * ( 1 - y) +
            pixelb1.Blue * (x) * (1 - y) +
            pixela2.Blue * (1-x) * (y) +
            pixelb2.Blue * x * y;


    float newAlpha = pixela1.Alpha * (1 - y) * ( 1 - y) +
            pixelb1.Alpha * (x) * (1 - y) +
            pixela2.Alpha * (1-x) * (y) +
            pixelb2.Alpha * x * y;
            
            
    tmp.Red = newRed;


    tmp.Green = newGreen;
    tmp.Blue = newBlue;
    tmp.Alpha = newAlpha;


    return tmp;

}



void interpolate(int i, int j,BMP &OriginalImage, BMP &FinalImage, int height, int width ){


	                        if(j%2 == 1){ // Si se trata de una columna impar
                            if(i != 0 && j != 0 && i < width - 1  && j < height -1){ // no estamos en un borde
                                    RGBApixel pixel1 = OriginalImage.GetPixel(i/2,j/2);
                                    RGBApixel pixel2 = OriginalImage.GetPixel((i+1)/2,j/2);
                                    RGBApixel pixel3 = OriginalImage.GetPixel(i/2,(j+1)/2);
                                    RGBApixel pixel4 = OriginalImage.GetPixel((i+1)/2,(j+1)/2);

                                    RGBApixel tmp = bilinealInterpolation( pixel1,pixel2,pixel3,pixel4,0.45,0.45 );  
                                    
                                    FinalImage.SetPixel(i,j,tmp); 

                            }
                        }else if(i%2 == 1){// si se trata de una fila impar
                            if(i != 0 && j != 0 && i < width - 1  && j < height -1){ // no estamos en un borde
                                    RGBApixel pixel1 = OriginalImage.GetPixel(i/2,j/2);
                                    RGBApixel pixel2 = OriginalImage.GetPixel((i+1)/2,j/2);
                                    RGBApixel pixel3 = OriginalImage.GetPixel(i/2,(j+1)/2);
                                    RGBApixel pixel4 = OriginalImage.GetPixel((i+1)/2,(j+1)/2);

                                    RGBApixel tmp = bilinealInterpolation( pixel1,pixel2,pixel3,pixel4,0.55,0.55 );  
                                    
                                    FinalImage.SetPixel(i,j,tmp);                 
                            }
                        }else{// una copia exacta del pixel original
                               
                                FinalImage.SetPixel(i,j,OriginalImage.GetPixel(i/2,j/2));
                            
                        }
}




int main (int argc, char *argv[])
{
int	numtasks,              /* number of tasks in partition */
	taskid,                /* a task identifier */
	numworkers,            /* number of worker tasks */
	source,                /* task id of message source */
	dest,                  /* task id of message destination */
	mtype,                 /* message type */
	rows,                  /* rows of matrix A sent to each worker */
	averow, extra, offset, /* used to determine rows sent to each worker */
	i, j, k, rc;           /* misc */
uint8_t	r[NRA][NCA],           /* matrix A to be multiplied */
	g[NCA][NCB],           /* matrix B to be multiplied */
	b[NRA][NCB],
	rgbtmp[NRA][NCB];          /* result matrix C */
	
uint8_t	newr[MAX][MAX];           /* result matrix C */
uint8_t	newg[MAX][MAX];           /* result matrix C */
uint8_t	newb[MAX][MAX];           /* result matrix C */
uint8_t	newrgbtmp[MAX][MAX];           /* result matrix C */

MPI_Status status;

MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
if (numtasks < 2 ) {
  printf("Need at least two MPI tasks. Quitting...\n");
  MPI_Abort(MPI_COMM_WORLD, rc);
  exit(1);
  }
numworkers = numtasks-1;


/**************************** master task ************************************/
   if (taskid == MASTER)
   {
	   
	string filename;
    int thread_count = 1;
    if (argc >= 2) {
        filename = argv[1];

    }else{

        cout << "Imagen de entrada: ";
        getline(cin, filename);


    }


    BMP OriginalImage;
    
    cout << "[?]Leyendo imagen..." << endl;
    OriginalImage.ReadFromFile(filename.c_str());





    BMP FinalImage;

    FinalImage.SetSize(OriginalImage.TellWidth()*2,OriginalImage.TellHeight()*2 );
    FinalImage.SetBitDepth(OriginalImage.TellBitDepth());
    

    int width = OriginalImage.TellWidth()*2;
    int height = OriginalImage.TellHeight()*2;
        

    
    cout << "[?]Interpolando pixeles..." << endl;
    
    
    
      //obtenemos tiemoo


int chunk = CHUNKSIZE;

    //Bucles para recorrer todo los pixeles


	for(int i = 0; i < NRA ; i++){
		for(int j=0; j<NRA; j++){
			RGBApixel tmp = OriginalImage.GetPixel(i,j);
			
			//printf("%i",tmp.Red);



			r[i][j] =  OriginalImage.GetPixel(i,j).Red;
			g[i][j] =  OriginalImage.GetPixel(i,j).Green;
			b[i][j] =  OriginalImage.GetPixel(i,j).Blue;


			
		}
					//printf("\n");

	}





	//en vez de operar le pasamos el contenido a los esclavos




        for(int i=0; i< width ; i++){

           #pragma omp parallel  num_threads(thread_count) default(none) shared(height, i, width,OriginalImage,FinalImage)
        {


	int nThreads = omp_get_num_threads();
	int idThread = omp_get_thread_num() ;


					# pragma omp for nowait 
			                for(int j=idThread; j < height; j += nThreads){

			       			  //    interpolate(i,j,OriginalImage,FinalImage,height,width);

			                }

                }    



}
        //ordenamos



    //obtenesmo tiempo y restamos
      long double fin = tiempo();

   // cout << "[!]Tiempo de interpolacion: "  << fin-ini << " segundos" <<endl;
      
      
    cout << "[?]Exportando imagen..." <<endl;

    
   // FinalImage.WriteToFile("output.bmp");
    
    cout << "[!]Finalizado..." <<endl;
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
	   
      printf("mpi_mm has started with %d tasks.\n",numtasks);
      printf("Initializing arrays...\n");

      /* Send matrix data to the worker tasks */
      averow = NRA/numworkers;
      extra = NRA%numworkers;
      offset = 0;
      mtype = FROM_MASTER;
      for (dest=1; dest<=numworkers; dest++)
      {
		  
		 if(dest == 1){
			  
			printf("Sending red\n");
			MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
			MPI_Send(&r, NCA*NCB, MPI_BYTE, dest, mtype, MPI_COMM_WORLD);

			  
		}else if(dest == 2){
			printf("Sending green \n");
			MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
			MPI_Send(&g, NCA*NCB, MPI_BYTE, dest, mtype, MPI_COMM_WORLD);
			
		}else if(dest == 3){
			printf("Sending blue \n");
			MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
			MPI_Send(&b, NCA*NCB, MPI_BYTE, dest, mtype, MPI_COMM_WORLD);
		}else{
			
			cout << "MAL" << endl;
		}
		  
		  


      }


	//dest=1 -> red
	//dest=2 -> green
	//dest=3 -> blue
		





	cout  << (int)r[77][5] << (int)g[25][63] << (int)b[15][0] << endl;

      /* Receive results from worker tasks */
      mtype = FROM_WORKER;
      for (i=1; i<=numworkers; i++)
      {
         source = i;
     /*    MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);

		MPI_Recv(&newr, 2*NCA*NCB*2, MPI_BYTE, source, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&newg, 2*NCA*NCB*2, MPI_BYTE, source, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&newb, 2*NCA*NCB*2, MPI_BYTE, source, mtype, MPI_COMM_WORLD, &status);
*/

		 if(source == 1){ //red
			  
			 printf("Received results from task %d\n",source);
			 MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
			 MPI_Recv(&newr, 2*NCA*NCB*2, MPI_BYTE, source, mtype, MPI_COMM_WORLD, &status);
			  
		}else if(source == 2){ //green
			 printf("Received results from task %d\n",source);
			 MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
			 MPI_Recv(&newg, 2*NCA*NCB*2, MPI_BYTE, source, mtype, MPI_COMM_WORLD, &status);
			
		}else if(source == 3){ //blue
			printf("Received results from task %d\n",source);
			MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
			MPI_Recv(&newb, 2*NCA*NCB*2, MPI_BYTE, source, mtype, MPI_COMM_WORLD, &status);
		}else{
			
			cout << "MAL" << endl;
		}


      }





		for(int i=0; i< MAX;i++){
			for(int j=0; j< MAX;j++){
				
				RGBApixel full;
				full.Red =0;
				full.Green = 0;
				full.Blue =0;
								FinalImage.SetPixel(i,j,full); 

				RGBApixel tmp;
				tmp.Red = newr[i][j];
				tmp.Green = newg[i][j];
				tmp.Blue = newb[i][j];

				FinalImage.SetPixel(i,j,tmp); 
			}
		}
		
    FinalImage.WriteToFile("output.bmp");







      /* Print results */
      printf("******************************************************\n");
      printf("Result Matrix:\n");
    /*  for (i=0; i<NRA; i++)
      {
         printf("\n"); 
         for (j=0; j<NCB; j++) 
            printf("%6.2f   ", c[i][j]);
      }*/
      printf("\n******************************************************\n");
      printf ("Done.\n");
   }


/**************************** worker task ************************************/
   if (taskid > MASTER)
   {
      mtype = FROM_MASTER;
      MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);

     // MPI_Recv(&r, NCA*NCB, MPI_BYTE, MASTER, mtype, MPI_COMM_WORLD, &status);
     // MPI_Recv(&g, NCA*NCB, MPI_BYTE, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rgbtmp, NCA*NCB, MPI_BYTE, MASTER, mtype, MPI_COMM_WORLD, &status);

	



	cout  << (int)r[77][5] << (int)g[25][63] << (int)b[15][0] << endl;

      long double ini = tiempo();

	for(int i = 0; i<MAX; i++){
		for(j = 0; j<MAX; j++){
			interpolate(i,j,rgbtmp,newrgbtmp,MAX,MAX);
			//interpolate(i,j,g,newg,MAX,MAX);
			//interpolate(i,j,b,newb,MAX,MAX);

		}
	}
	
	
      long double fin = tiempo();

    cout << "[!]Tiempo de interpolacion: "  << fin-ini << " segundos" <<endl;
      
    	FILE *arch;
       arch=fopen("./tiempo.txt","w");
       fprintf(arch,"Tiempo: %Lf\n", (long double)fin-ini);
       fclose(arch);


	
   // FinalImage.WriteToFile("output.bmp");




      mtype = FROM_WORKER;
      MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&newrgbtmp, MAX*MAX, MPI_BYTE, MASTER, mtype, MPI_COMM_WORLD);
     // MPI_Send(&newg, MAX*MAX, MPI_BYTE, MASTER, mtype, MPI_COMM_WORLD);
    //  MPI_Send(&newb, MAX*MAX, MPI_BYTE, MASTER, mtype, MPI_COMM_WORLD);

   }
   MPI_Finalize();
}
