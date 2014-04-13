MPICC   = mpicc
MPIEXEC = mpiexec
CFLAGS += -Wall

EJECUTABLE = tp3

.PHONY : all new clean run

Desktop: all

all : $(EJECUTABLE)

tp3 : tp3.c srv.c
	$(MPICC) $(CFLAGS) -o tp3 tp3.c srv.c

run : tp3
	$(MPIEXEC) -np 30 ./$(EJECUTABLE)  'a' 500 500 1 500 500 >/dev/null

clean :
	rm -f $(EJECUTABLE)

new : clean all
