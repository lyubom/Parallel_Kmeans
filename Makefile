.KEEP_STATE:

all: simple parallel

OPTFLAGS    = -O -NDEBUG
OPTFLAGS    = -g -pg
INCFLAGS    = -I.
CFLAGS      = $(OPTFLAGS) $(DFLAGS) $(INCFLAGS)
LDFLAGS     = $(OPTFLAGS)

OMPFLAGS    = -fopenmp
CC          = gcc

.c.o:	$(CC) $(CFLAGS) -c $<

H_FILES     = kmeans.h

OMP_SRC     = parallel_main.c parallel_kmeans.c

OMP_OBJ     = $(OMP_SRC:%.c=%.o)

parallel_kmeans.o: parallel_kmeans.c $(H_FILES)
	$(CC) $(CFLAGS) $(OMPFLAGS) -c parallel_kmeans.c

parallel: parallel_main
parallel_main: $(OMP_OBJ) file_io.o
	$(CC) $(LDFLAGS) $(OMPFLAGS) -o parallel_main $(OMP_OBJ) file_io.o $(LIBS)

SEQ_SRC     = simple_main.c   \
              simple_kmeans.c \
	      			file_io.c    \
	      			wtime.c

SEQ_OBJ     = $(SEQ_SRC:%.c=%.o)

$(SEQ_OBJ): $(H_FILES)

simple: simple_main
simple_main: $(SEQ_OBJ) $(H_FILES)
	$(CC) $(LDFLAGS) -o simple_main $(SEQ_OBJ) $(LIBS)

clean:
	rm -rf *.o parallel_main simple_main mpi_main \
	       core* .make.state gmon.out
