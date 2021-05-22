# mpicc -Wall -o main main.c worker.c dispatcher.c
# mpiexec -n 2 ./main text0.txt