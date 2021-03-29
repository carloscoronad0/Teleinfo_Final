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

	int to_read; // Variable donde almacenar la cantidad de datos que leyo el socket
	int received; // Variable donde almacenar la cantidad de datos totales que llegaron
	int data_Size; // Variable para almacenar el Size de los frames
	int sent; // Variable para almacenar la cantidad de datos enviados

	char receiver_buffer[1000]; // Donce se almacena lo recibido por el socket
	char aux[1000]; // Una variable temporal antes de pasar lo recibido al buffer principal

	// For converter comunication ------------------------------------------------------

	request_converter_header request_converter; // Estructura del header del request del proxy al converter
	response_converter_header response_converter; // Estructura del header del response del converter al proxy
	char direccion[50]; // Donde se almacenara la direccion del frame
	char paquete[16500]; // Donde se almacenara los headers y el contenido del frame

	// For esp32 comunication ----------------------------------------------------------

	state header_orden; // Un enum para facilitar el analisis de los recibido del esp32
	esp32_header request_esp32; // Estructura del header del esp32 - proxy

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
			// Se setea todos los chars a 0 para que no haya basura dentro de ellos y los mensajes que enviemos o 
			// recibamos no se vean afecatdos

			memset(direccion, 0, sizeof(direccion));
			memset(receiver_buffer, 0, sizeof(receiver_buffer));
			memset(aux, 0, sizeof(aux));

			// Obtencion y envio del frame ---------------------------------------------

			obteniendo_direccion(frame, direccion); // Se obtiene la direccion en base al numero de frame
			data_Size = obtener_Data_Size(direccion); // Se obtiene el Size del fichero
			request_converter.Total_Size = sizeof(request_converter) + data_Size; // Se calcula el Total Size = size del fichero + size del header
			printf("Total_Size: %d\n", (request_converter.Total_Size));

			memset(paquete, 0, sizeof(paquete)); // Se setea a 0 para evitar problemas como ya se meciono antes

			printf("Paquete seteado a 0\n");

			obtener_Fichero_Completo(&request_converter, paquete, direccion); // Obtenemos el paquete ya incluido con el header y los datos
			printf("Fichero obtenido\n");
			sent = send((proxy_client.CLIENT_FD), paquete, (request_converter.Total_Size), 0); // Se envia el paquete al converter
			printf("Fichero enviado a converter\n");

			// Recepcion de la respuesta del converter ---------------------------------
			
			received = 0;

			while(received < sizeof(response_converter.Total_Size)) // Mientras no se reciba la cantidad de bytes 
			{
				to_read = recv((proxy_client.CLIENT_FD), receiver_buffer, sizeof(receiver_buffer), 0);
				printf("Recieved: %d bytes from converter\n", to_read);
				memcpy(aux + received, receiver_buffer, to_read);
				received += to_read;
			}

			memcpy(&(response_converter.Total_Size), aux, sizeof(response_converter.Total_Size));

			printf("Response size: %d\n", (int) (response_converter.Total_Size));

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

			printf("Header esp32 recibido\n");

			// Analisis del header y envio ---------------------------------------------

			header_orden = analizar_Header_ESP32(to_read, &request_esp32);

			if (header_orden == todo_correcto)
			{
				sent = send((proxy_server.CLIENT_FD), paquete, (response_converter.Total_Size), 0);
				frame = (frame % 148) + 1;

				while (frame == 61 || frame == 81 || frame == 87 || frame == 88 || frame == 89)
				{
					frame = (frame % 148) + 1;
				}
				
				printf("Sent: %d\n", sent);
			}
			else
			{
				on_error("Error con el ESP32\n");
			}

		}

	}

	return 0;
}