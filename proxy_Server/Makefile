CC = gcc
IDIR = .
CFLAGS = -I$(IDIR) -std=gnu99
LIBS = 
OBJ = proxy_Server.c proxy_Inicializacion.c proxy_ESP32.c proxy_Converter.c
DEPS = proxy.h headers.h

%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $< $(CFLAGS)


ds: $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# clean out the dross
clean:
	rm -f ds  *~ *.o 
