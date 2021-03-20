CC = gcc
IDIR = .
CFLAGS = -I$(IDIR) -std=gnu99
LIBS = 
OBJ = principal.c punto.c cadena.c 
DEPS = punto.h cadena.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


ds: $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# clean out the dross
clean:
	rm -f ds  *~ *.o 
