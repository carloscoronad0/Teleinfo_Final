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
	if (argc < 3) on_error("Usage: %s [port] \n", argv[0]);

	// Declaracion de las estructuras que determinan como actuara el proxy
	s_proxy_server proxy_server;
	s_proxy_client proxy_client;

	// Obtencion de las variables de el vector argv
	proxy_server.SERVER_PORT = atoi(argv[SERVER_PORT_INDEX]);
	proxy_client.CONVERTER_PORT = atoi(argv[CONVERTER_PORT_INDEX]);
	proxy_client.IP_DIRECTION = "127.0.0.1"; // Direccion de localhost

	// Inicializacion del Proxy como Servidor
	inicializar_Proxy_Server(&proxy_server);

	// Inicializacion del proxy como Cliente
	inicializar_Proxy_Client(&proxy_client);

	// Funcionamiento PROXY ------------------------------------------------------------
	// ---------------------------------------------------------------------------------

	int to_read;
	int received;
	int data_Size;
	int sent;

	char receiver_buffer[1000];
	char aux[1000];

	// For converter comunication ------------------------------------------------------

	request_converter_header request_converter;
	response_converter_header response_converter;
	char direccion[50];
	char paquete[16500];

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

		// Empieza la comunicacion
		while(1)
		{
			memset(direccion, 0, sizeof(direccion));
			// Obtencion y envio del frame ---------------------------------------------
			obteniendo_direccion(frame, direccion);
			data_Size = obtener_Data_Size(direccion);
			printf("data_Size: %d\n", data_Size);
			request_converter.Total_Size = sizeof(request_converter) + data_Size;
			printf("Total_Size: %d\n", (request_converter.Total_Size));

			memset(paquete, 0, sizeof(paquete));

			printf("Paquete seteado a 0\n");

			obtener_Fichero_Completo(&request_converter, paquete, direccion);
			printf("FIchero obtenido\n");
			sent = send((proxy_client.CLIENT_FD), paquete, (request_converter.Total_Size), 0);
			printf("FIchero enviado\n");

			// Recepcion de la respuesta del converter ---------------------------------
			
			to_read = 0;
			received = 0;

			to_read = recv((proxy_client.CLIENT_FD), receiver_buffer, sizeof(receiver_buffer), 0);
			printf("Recieved: %d bytes from proxy\n", to_read);

			while(received < sizeof(response_converter.Total_Size))
			{
				to_read = recv((proxy_client.CLIENT_FD), receiver_buffer, sizeof(receiver_buffer), 0);
				printf("Recieved: %d bytes from proxy\n", to_read);
				memcpy(aux + received, receiver_buffer, to_read);
				received += to_read;
			}

			memcpy(&(response_converter.Total_Size), aux, sizeof(response_converter.Total_Size));
			memset(paquete, 0, sizeof(paquete));

			memcpy(paquete, aux, received);

			while(received < (response_converter.Total_Size))
			{
				to_read = recv((proxy_client.CLIENT_FD), receiver_buffer, sizeof(receiver_buffer), 0);
				printf("Recieved: %d bytes from proxy\n", to_read);
				memcpy(paquete + received, receiver_buffer, to_read);
				received += to_read;
			}

			printf("Recepcion exitosa del frame %d convertido\n", frame);

			// Recepcion del header del ESP32 ------------------------------------------

			to_read = recv((proxy_server.CLIENT_FD), &(request_esp32.Flag), sizeof(request_esp32), 0);
			if (to_read < 0) on_error("Error en la recepcion del ESP32\n");

			// Analisis del header y envio ---------------------------------------------

			header_orden = analizar_Header_ESP32(to_read, &request_esp32);

			if (header_orden == todo_correcto)
			{
				sent = send((proxy_server.CLIENT_FD), paquete, (response_converter.Total_Size), 0);
				frame = (frame % 148) + 1;
			}

		}

	}

	return 0;
}