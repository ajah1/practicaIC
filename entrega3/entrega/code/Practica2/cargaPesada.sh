  #!/bin/bash
	make
	make run
	cd debug
        for i in `seq 1 8`;
        do
                ./main output.bmp
        done  

	make clean
