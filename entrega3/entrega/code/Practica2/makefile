all:
	mpic++ -Wall -g ./main.cpp -o debug/main lib/EasyBMP.cpp -fopenmp
clean:
	rm debug/main debug/output.bmp
run:
	cd debug &&  mpirun -mca plm_rsh_no_tree_spawn 1 -hostfile listanodos.txt -n 4 ./main  ./test.bmp
