#!/bin/sh
# Generate rsa key 
ssh-keygen -t rsa

#Copy de rsa key in slaves
ssh-copy-id alu@172.20.43.143
ssh-copy-id alu@172.20.43.142
ssh-copy-id alu@172.20.43.141
ssh-copy-id alu@172.20.43.140

#Send project to all slaves
scp mpi.tar.gz alu@172.20.43.140:~/
scp mpi.tar.gz alu@172.20.43.141:~/
scp mpi.tar.gz alu@172.20.43.142:~/
scp mpi.tar.gz alu@172.20.43.143:~/

# mpirun -mca plm_rsh_no_tree_spawn 1 -hostfile listanodos.txt -n 8 ./main

