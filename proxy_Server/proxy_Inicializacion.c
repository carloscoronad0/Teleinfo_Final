#include <stdio.h>		// libreria basica
#include <stdlib.h>		// Libreria basica
#include <sys/types.h> 	// managing socket
#include <sys/socket.h> // managing socket
#include <netinet/in.h> // sockaddr_in struct
#include "proxy.h"		// Necesaria para las estructuras del proxy

void inicializar_Proxy_Server(s_proxy_server *proxy_server)
{
	int err;

	proxy_server->SERVER_FD = socket(AF_INET, SOCK_STREAM, 0);
	if ((proxy_server->SERVER_FD) < 0) on_error("Error during the creation of the server socket\n");

	proxy_server->proxy.sin_family = AF_INET;
	proxy_server->proxy.sin_port = htons((proxy_server->SERVER_PORT));
	proxy_server->proxy.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt_val = 1;
	err = setsockopt((proxy_server->SERVER_FD), SOL_SOCKET, SO_REUSADOR, &opt_val);
	if (err < 0) on_error("Socket option not set succesfully\n");

	err = bind((proxy_server->SERVER_FD), (struct sockaddr *) &(proxy_server->proxy), sizeof((proxy_server->proxy)));
	if (err < 0) on_error("Error, could not bind the socket\n");

	err = listen((proxy_server->SERVER_FD),128);
	if (err < 0) on_error("Error, socket listening not possible\n");

	printf("Server is listening on: %d\n", (proxy_server->SERVER_PORT));
}

void inicializar_Proxy_Client(s_proxy_client *proxy_client)
{
	int err;

	proxy_client->CLIENT_FD = socket(AF_INET, SOCK_STREAM, 0);
	if ((proxy_client->CLIENT_FD) < 0) on_error("Error during the creation of the client socket\n");

	proxy_client->converter.sin_family = AF_INET;
	proxy_client->converter.sin_port = htons((proxy_client->CONVERTER_PORT));
	proxy_client->converter.sin_addr.s_addr = inet_addr((proxy_client->IP_DIRECTION));

	err = connect((proxy_client->CLIENT_FD), (struct sockaddr *) &(proxy_client->converter), sizeof((proxy_client->converter)));
	if (err < 0) on_error("Could not connect to Converter\n");

	printf("Connected to Converter\n");
}