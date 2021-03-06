#include <string>
#include <sys/resource.h>

#include "lib/EasyBMP.h"
#include <omp.h>

#include <stdint.h>
#include <mpi.h>

#define N 1000
#define CHUNKSIZE 100


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







//Funcion de interpolación:
//Esta funcion recibe 4 pixeles por parametro ademas, de un punto (x,y) que define el peso final de cada pixel
//De esta forma se puede calibrar para que tengan unos pixeles mas importancia que otra.
RGBApixel bilinealInterpolation(RGBApixel pixela1, RGBApixel pixela2, RGBApixel pixelb1, RGBApixel pixelb2, float x, float y){

    RGBApixel tmp;


    
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


int main(int argc, char* argv[]){
    string filename;
    int thread_count = 4;
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
        

	// parte de la imagen a recibir
	uint8_t imagen [10][10];

	// inicializar imagen
	for ( int i = 0; i < 10; ++i )
	{
		for (int j = 0; j < 10; ++j )
		{
			imagen[i][j] = 3;
		}
	}

	std::clog << "\n [MPI]: imagen inicializada" << std::endl;
	for ( int i = 0; i < 10; ++i )
	{
		for (int j = 0; j < 10; ++j )
		{
			std::cout << (int)imagen[i][j];
		}
		std::cout << endl;
	}


	MPI_Comm servidor;

	int size, rank;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Status status;


	string portname = "1026424832.0;tcp://172.20.43.142:41247+1026424833.0;tcp://172.20.43.142::300";

  	std::clog << "[MPI]: conectando con master" << std::endl;
	MPI_Comm_connect ( portname.c_str(), MPI_INFO_NULL, 0, MPI_COMM_WORLD, &servidor );

	std::clog << "[MPI]: recibir datos del master" << std::endl;
	MPI_Recv ( &imagen, 100, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, servidor, &status );

	std::clog << "[MPI]: datos obtenidos" << std::endl;
	for ( int i = 0; i < 10; ++i )
	{
		for ( int j = 0; j < 10; ++j )
		{
			std::cout << (int)imagen[i][j];
		}
		std::cout << endl;
	}

	std::clog << "[MPI]: finalizar conexión con master" << std::endl;
	MPI_Comm_disconnect(&servidor);
	MPI_Finalize();
	

    cout << "[?]Interpolando pixeles..." << endl;
    
    
    
      //obtenemos tiemoo
      long double ini = tiempo();

int chunk = CHUNKSIZE;





        for(int i=0; i< width ; i++){

           #pragma omp parallel  num_threads(thread_count) default(none) shared(height, i, width,OriginalImage,FinalImage)
        {


	int nThreads = omp_get_num_threads();
	int idThread = omp_get_thread_num() ;


					# pragma omp for nowait 
			                for(int j=idThread; j < height; j += nThreads){

			       			      interpolate(i,j,OriginalImage,FinalImage,height,width);

			                }

                }    



}
        //ordenamos



    //obtenesmo tiempo y restamos
      long double fin = tiempo();

    cout << "[!]Tiempo de interpolacion: "  << fin-ini << " segundos" <<endl;
      
      
    cout << "[?]Exportando imagen..." <<endl;

    
    FinalImage.WriteToFile("output.bmp");
    
    cout << "[!]Finalizado..." <<endl;
    
	return 0;
}
