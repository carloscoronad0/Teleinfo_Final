#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 	// managing socket
#include <sys/socket.h> // managing socket
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(-1); }

#define SERVER_PORT_INDEX 1
#define CONVERTER_PORT_INDEX 2

int main(int argc, char const *argv[])
{
	if (argc < 3) on_error("Usage: %s [port] \n", argv[0]);
	int SERVER_PORT = atoi(argv[SERVER_PORT_INDEX]);
	int CONVERTER_PORT = atoi(argv[CONVERTER_PORT_INDEX]);

	// SETTING SOCKETS -----------------------------------------------------------
	// ---------------------------------------------------------------------------

	int err;

	// Setting SERVER socket -----------------------------------------------------

	int proxy_server_fd, esp32_proxy_fd;
	struct sockaddr_in proxy_server, client_to_server;

	proxy_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (proxy_server_fd < 0) on_error("Error during the creation of the server socket\n");

	proxy_server.sin_family = AF_INET;
	proxy_server.sin_port = htons(SERVER_PORT);
	proxy_server.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt_val = 1;
	err = setsockopt(proxy_server_fd, SOL_SOCKET, SO_REUSADOR, &opt_val);
	if (err < 0) on_error("Socket option not set succesfully\n");

	err = bind(proxy_server_fd, (struct sockaddr *) &proxy_server, sizeof(proxy_server));
	if (err < 0) on_error("Error, could not bind the socket\n");

	err = listen(proxy_server_fd, 128);
	if (err < 0) on_error("Error, socket listening not possible\n");

	printf("Server is listening on: %d\n", SERVER_PORT);

	// Setting CLIENT socket -----------------------------------------------------

	int proxy_client_fd;
	struct sockaddr_in proxy_converter;

	proxy_client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (proxy_client_fd < 0) on_error("Error during the creation of the client socket\n");

	proxy_converter.sin_family = AF_INET;
	proxy_converter.sin_port = htons(CONVERTER_PORT);
	proxy_converter.sin_addr.s_addr = inet_addr("Direccion IP Converter");

	err = connect(proxy_client_fd, (struct sockaddr *) &proxy_converter, sizeof(proxy_converter));

	return 0;
}