#include <stdio.h>		// Libreria basica
#include <stdlib.h>		// Libreria basica
#include <sys/types.h> 	// managing socket
#include <sys/socket.h> // managing socket
#include <netinet/in.h> // sockaddr_in struct
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "proxy.h"		// Necesaria para las estructuras del proxy
#include "headers.h"	// Necesaria para el manejo de los headers

int main(int argc, char const *argv[])
{
	if (argc < 4) on_error("Usage: %s [port] \n", argv[0]);

	// Declaracion de las estructuras que determinan como actuara el proxy
	s_proxy_server proxy_server;
	s_proxy_client proxy_client;

	// Obtencion de las variables de el vector argv
	proxy_server.SERVER_PORT = atoi(argv[SERVER_PORT_INDEX]);
	proxy_client.CONVERTER_PORT = atoi(argv[CONVERTER_PORT_INDEX]);
	proxy_client.IP_DIRECTION = argv[CONVERTER_IP_DIRECTION_INDEX];

	// Inicializacion del Proxy como Servidor
	inicializar_Proxy_Server(&proxy_server);

	// Inicializacion del proxy como Cliente
	inicializar_Proxy_Client(&proxy_client);

	// Funcionamiento PROXY ------------------------------------------------------------
	// ---------------------------------------------------------------------------------

	int to_read;
	int data_Size;
	int total_length;

	// For converter comunication ------------------------------------------------------

	request_converter_header request_converter;
	response_converter_header response_converter;
	char direccion[50];

	// For esp32 comunication ----------------------------------------------------------

	state header_orden;
	esp32_header request_esp32;

	while(1)
	{
		// Estableciendo conexion con el cliente ---------------------------------------

		socklen_t client_len = sizeof(proxy_server.client);
		proxy_server.CLIENT_FD = accept((proxy_server.SERVER_FD), (struct sockaddr *) &(proxy_server.client), &client_len);
		if (proxy_server.CLIENT_FD < 0) on_error("Error in establishing client connection");

		printf("Client connected\n");
		int frame = 1;

		// Seccion para comunicacion PROXY - CONVERTER ---------------------------------
		// -----------------------------------------------------------------------------



		// Seccion para comunicacion ESP32 -PROXY --------------------------------------
		// -----------------------------------------------------------------------------

		to_read = recv((proxy_server.CLIENT_FD), &(request_esp32.Flag), sizeof(request_esp32), 0);
		header_orden = analizar_Header_ESP32(to_read, &request_esp32);

		if (header_orden == todo_correcto)
		{
			//to_read = send((proxy_server.CLIENT_FD), )
			frame++;
		}

	}

	return 0;
}