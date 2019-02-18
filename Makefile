all: sequential parallel

OMPFLAGS = -fopenmp
CC = gcc

.c.o:	$(CC) -c $<

OMP_SRC = parallel_kmeans.c
OMP_OBJ = $(OMP_SRC:%.c=%.o)

parallel: $(OMP_OBJ)
	$(CC) $(OMPFLAGS) -o parallel_kmeans $(OMP_OBJ)

SEQ_SRC = sequential_kmeans.c
SEQ_OBJ = $(SEQ_SRC:%.c=%.o)

sequential: $(SEQ_OBJ)
	$(CC) -o sequential_kmeans $(SEQ_OBJ)

clean:
	rm -rf *.o parallel_kmeans sequential_kmeans
