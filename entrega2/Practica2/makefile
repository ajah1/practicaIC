all:
	g++ -Wall -g ./main.cpp -o debug/main lib/EasyBMP.cpp -fopenmp
clean:
	rm debug/main debug/output.bmp
run:
	cd debug && ./main ./test.bmp
