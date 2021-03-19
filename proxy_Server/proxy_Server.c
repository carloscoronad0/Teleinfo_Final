#include <stdio.h>
#include <stdlib.h>

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(-1); }

#define SERVER_PORT_INDEX 1;

int main(int argc, char const *argv[])
{
	if (argc < 3) on_error("Usage: %s [port] \n", argv[0]);

	int PORT = atoi(argv[SERVER_PORT_INDEX]);

	
	return 0;
}