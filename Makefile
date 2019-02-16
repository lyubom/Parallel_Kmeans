all: simple parallel

OMPFLAGS = -fopenmp
CC = gcc

.c.o:	$(CC) -c $<

H_FILES = kmeans.h
OMP_SRC = parallel_main.c parallel_kmeans.c
OMP_OBJ = $(OMP_SRC:%.c=%.o)

parallel_kmeans.o: parallel_kmeans.c $(H_FILES)
	$(CC) $(OMPFLAGS) -c parallel_kmeans.c

parallel: $(OMP_OBJ) helper.o
	$(CC) $(OMPFLAGS) -o parallel_main $(OMP_OBJ) helper.o

SEQ_SRC = simple_main.c simple_kmeans.c helper.c
SEQ_OBJ = $(SEQ_SRC:%.c=%.o)
$(SEQ_OBJ): $(H_FILES)

simple: $(SEQ_OBJ) $(H_FILES)
	$(CC) -o simple_main $(SEQ_OBJ)

clean:
	rm -rf *.o parallel_main simple_main
