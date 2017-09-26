#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>

#include "EasyBMP.h"


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








RGBApixel bilinealInterpolation(RGBApixel pixela1, RGBApixel pixela2, RGBApixel pixelb1, RGBApixel pixelb2, float x, float y){

    RGBApixel tmp;


    
    int newRed = pixela1.Red * (1 - x) * ( 1 - y) +
            pixelb1.Red * (x) * (1 - y) +
            pixela2.Red * (1-x) * (y) +
            pixelb2.Red * x * y;

    int newGreen = pixela1.Green * (1 - y) * ( 1 - y) +
            pixelb1.Green * (x) * (1 - y) +
            pixela2.Green * (1-x) * (y) +
            pixelb2.Green * x * y;

    int newBlue = pixela1.Blue * (1 - y) * ( 1 - y) +
            pixelb1.Blue * (x) * (1 - y) +
            pixela2.Blue * (1-x) * (y) +
            pixelb2.Blue * x * y;


    int newAlpha = pixela1.Alpha * (1 - y) * ( 1 - y) +
            pixelb1.Alpha * (x) * (1 - y) +
            pixela2.Alpha * (1-x) * (y) +
            pixelb2.Alpha * x * y;
            
            
    tmp.Red = newRed;
    tmp.Green = newGreen;
    tmp.Blue = newBlue;
    tmp.Alpha = newAlpha;
    return tmp;

}


int main(int argc, char* argv[]){
    string filename;

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
      long double ini = tiempo();



	for(int i=0; i< width ; i++){

    		for(int j=0; j < height; j++){

    			if(j%2 == 1){
                    if(i != 0 && j != 0 && i < width - 1  && j < height -1){
                            RGBApixel pixel1 = OriginalImage.GetPixel(i/2,j/2);
                            RGBApixel pixel2 = OriginalImage.GetPixel((i+1)/2,j/2);
                            RGBApixel pixel3 = OriginalImage.GetPixel(i/2,(j+1)/2);
                            RGBApixel pixel4 = OriginalImage.GetPixel((i+1)/2,(j+1)/2);

                            RGBApixel tmp = bilinealInterpolation( pixel1,pixel2,pixel3,pixel4,0.5,0.75 );  
                            
                            FinalImage.SetPixel(i,j,tmp); 

                    }
                }else if(i%2 == 1){
                    if(i != 0 && j != 0 && i < width - 1  && j < height -1){
                            RGBApixel pixel1 = OriginalImage.GetPixel(i/2,j/2);
                            RGBApixel pixel2 = OriginalImage.GetPixel((i+1)/2,j/2);
                            RGBApixel pixel3 = OriginalImage.GetPixel(i/2,(j+1)/2);
                            RGBApixel pixel4 = OriginalImage.GetPixel((i+1)/2,(j+1)/2);

                            RGBApixel tmp = bilinealInterpolation( pixel1,pixel2,pixel3,pixel4,0.25,0.5 );  
                            
                            FinalImage.SetPixel(i,j,tmp);                 
                    }
                }else{
                       
                        FinalImage.SetPixel(i,j,OriginalImage.GetPixel(i/2,j/2));
                    
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