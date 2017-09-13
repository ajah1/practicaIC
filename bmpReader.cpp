#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string.h>

using namespace std;

	typedef struct  __attribute__((__packed__)){
		uint16_t header; 
		uint32_t sizeF;
		uint16_t reserved;
		uint16_t reserved2;
		uint32_t offset;
		uint32_t sizeStrHead;
		uint32_t width;
		uint32_t height;
		uint16_t nPlanes;//2
		uint16_t bitsPerPixel;//2
		uint32_t compresion;//4
		uint32_t imageData;//4
		uint32_t horPixelMeter;//4
		uint32_t verPixelMeter;//4
		uint32_t nColor;//4
		uint32_t nImpColor;//4
	}bmpHeader;



	/*struct Pixel
	{
		uint8_t x: 3;
	    uint8_t a: 5;
	    uint8_t g: 7;
	    uint8_t r: 8;
	    uint16_t b: 9;

	}__attribute__((__packed__));*/

	struct Pixel
	{
	    uint8_t b;
	    uint8_t g;
	    uint8_t r;

	}__attribute__((__packed__));



int isMul4(int n){

    if (n % 8 == 0) {
        return n;
    }else{

        return isMul4(++n);
    }
}



Pixel mixPixels(Pixel pixel1, Pixel pixel2){
    Pixel tmp;
    tmp.r = (pixel1.r + pixel2.r)/2 ;
    tmp.g = (pixel1.r + pixel2.r)/2;
    tmp.b = (pixel1.r + pixel2.r)/2;
    return tmp;

} 



Pixel bilinealInterpolation(Pixel pixela1, Pixel pixela2, Pixel pixelb1, Pixel pixelb2, float x, float y){

    Pixel tmp;

    int sum1 = pixela1.r * (1 - x) * ( 1 - y);
    
    int sum2 = pixelb1.r * (x) * (1 - y);
    
    int sum3 = pixela2.r * (1 - x) * (y);
    
    int sum4 = pixelb2.r * x * y;
    
    
    int newRed = pixela1.r * (1 - x) * ( 1 - y) +
            pixelb1.r * (x) * (1 - y) +
            pixela2.r * (1-x) * (y) +
            pixelb2.r * x * y;

    int newGreen = pixela1.g * (1 - y) * ( 1 - y) +
            pixelb1.g * (x) * (1 - y) +
            pixela2.g * (1-x) * (y) +
            pixelb2.g * x * y;

    int newBlue = pixela1.b * (1 - y) * ( 1 - y) +
            pixelb1.b * (x) * (1 - y) +
            pixela2.b * (1-x) * (y) +
            pixelb2.b * x * y;

    tmp.r = newRed;
    tmp.g = newGreen;
    tmp.b = newBlue;

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

	bmpHeader imageHeader;
    cout << filename << endl;

	ifstream image(filename.c_str(), std::ios::binary );

	cout << filename.c_str() << endl;
	if(image.is_open()){
		cout << "Fichero abierto" << endl;
		cout << "sizeof(imageHeader): " << sizeof(imageHeader) << endl;
    	image.read((char*)&imageHeader,sizeof(imageHeader));

    	cout << "Header: " << hex <<imageHeader.header << endl;
    	cout << "Width: " <<  dec << (int)imageHeader.width << endl;
    	cout << "Height: " <<  dec << (int)imageHeader.height << endl;

    	cout << "OFFSET: " << dec << (int)imageHeader.offset << endl; 
    	cout << "Bits per Pixel: " << dec << (int)imageHeader.bitsPerPixel << endl; 



    	image.seekg((int)imageHeader.offset , ios_base::beg	 );

    	//Pixel pixels[(int)imageHeader.width * (int)imageHeader.height];
        Pixel pixels[(int)imageHeader.width ][(int)imageHeader.height];


        cout << "pixels buffer: "<< sizeof(pixels) <<  endl;
    	image.read((char*)&pixels,sizeof(pixels));

      	image.close();


        cout << "El multiplo mas cercano de 6 es: " << isMul4(6) << endl;


    	//Reservamos matriz del doble de tamaño 
    	//Pixel newPixels[(int)imageHeader.width*2 * (int)imageHeader.height* 2];
        Pixel newPixels[(int)imageHeader.width*2] [(int)imageHeader.height* 2];

  		//Lets create a big image



    Pixel negro;
    	negro.g=0;
    	negro.r=0;
    	negro.b=0;

  	Pixel blanco;
    	blanco.g=0xff;
    	blanco.r=0xff;
    	blanco.b=0xff;

  	Pixel red;
    	red.g=0x00;
    	red.r=0xff;
    	red.b=0x00;




        Pixel tmpPix = bilinealInterpolation(red,red,blanco,blanco,0.5,0.5);



        cout << "r: " << (int)tmpPix.r << "g: " << (int)tmpPix.g << "b: " << (int)tmpPix.b << endl;
 		cout << "Relleno de negro" << endl;

        for(int i=0; i< (int)imageHeader.width*2  ; i++){
                for(int j=0; j< (int)imageHeader.height *2 ; j++){
                    newPixels[i][j] = negro;
            }
        }

        int padding = isMul4(imageHeader.width*2*3) - imageHeader.width*2*3;

        cout << "El padding es: "<< padding << endl;
        int n=0;
        int m=0;
        Pixel tmp;
        tmp = red;
    	for(int i=0; i< (int)imageHeader.width*2 ; i++){
            n = (i+1) /2;
            m = 0;
    		for(int j=0; j < (int)imageHeader.height*2; j++){
                //cout << i<< " " << j << endl;

    			if(j%2 == 1){

                    //newPixels[i][j] = negro;



                if(i != 0 && j != 0 && i < (imageHeader.width-1) * 2  && j < (imageHeader.height-1) * 2){
                   
                  
                          newPixels[i][j] = bilinealInterpolation( pixels[i/2][j/2],pixels[i/2-1][j/2],pixels[i/2][j/2-1],pixels[i/2-1][j/2-1],0.5,0.5 );  

                }


                }else if(i%2 == 1){

                    //newPixels[i] [j] = red;




                if(i != 0 && j != 0 && i < (imageHeader.width-1)*2  && j < (imageHeader.height-1)*2 ){
                
                          newPixels[i][j] = bilinealInterpolation( pixels[i/2][j/2],pixels[i/2-1][j/2],pixels[i/2][j/2-1],pixels[i/2-1][j/2-1],0.5,0.5 );  
                 }

                }else{
                   // cout <<  "n: " << n<< " m: " << m << endl;

                    newPixels[i][j] = pixels[n][m];
                    tmp = pixels[n][m];

                    m++;
                }




    		}

        }
//
        imageHeader.width *=2;
		imageHeader.height *=2;
		imageHeader.sizeF *=2;

    	cout << "Generemos nuevo archivo ..." << endl;

  		ofstream outputF("test.bmp",std::ios::binary);




  		 outputF.write ((char *)&imageHeader,sizeof(imageHeader));
  		 outputF.write ((char *)newPixels,sizeof(newPixels));

  		 outputF.close();

	}else{
		cout << "Can't not open the file" << endl;

	}

	return 0;
}