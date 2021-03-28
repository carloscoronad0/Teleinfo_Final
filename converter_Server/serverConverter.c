#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(-1); }

#define BUFFER_SIZE 1024
#define PORT_INDEX 1
#define SEND_BUFFER_SIZE_INDEX 2

typedef enum { recieved, waiting } state;

int main(int argc, char const *argv[])
{
	if (argc < 3) on_error("Usage: %s [port] \n", argv[0]);

	int PORT = atoi(argv[PORT_INDEX]);
	int SEND_BUFFER_SIZE = atoi(argv[SEND_BUFFER_SIZE_INDEX]);

	// Si el size ingresado es mayor al size determinado de recepcion
	// saltara un error debido a que se correra el riesgo de que se envien
	// mas datos de los que se pueda almacenar el buffer de recpcion
	if (SEND_BUFFER_SIZE > BUFFER_SIZE)
		on_error("Send buffer size bigger than reception buffer\n");

	// SETTING SOCKET -----------------------------------------------------
	// --------------------------------------------------------------------
	
	int server_fd, client_fd, err;
	struct sockaddr_in server, client;
	char buffer[BUFFER_SIZE];

	// Auxi var 
	char bufferSend[BUFFER_SIZE];
	int len;
	int widgt;
	int height; 
	int paleta;
	int initData;

	short lenSend;
	short heightSend;
	short widgtSend;


	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		on_error("Error during the creation of the socket\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt_val = 1;
	err = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
	if (err < 0) on_error("Socket option not set successfully\n");

	err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
	if (err < 0) on_error("Error, could not bind socket\n");

	err = listen(server_fd, 128);
	if (err < 0) on_error("Error, socket listening not possible\n");

	printf("Server is listening on: %d\n", PORT);

	// SERVER FUNCTION ----------------------------------------------------
	// --------------------------------------------------------------------

	int to_read;
	unsigned short CLIENT_BUFFER_SIZE, client_send_size;
	state header;

	while(1)
	{
		// CONNECTION WITH CLIENT -----------------------------------------
		// ----------------------------------------------------------------

		socklen_t client_len = sizeof(client);
		client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

		if (client_fd < 0) 
		{
			on_error("Error in establishing client connection");
		}
		else 
		{
			printf("Connected client\n");
		}

		header = waiting;

		while (1)
		{
			// PROCESSING OF PACKETS RECIEVED -----------------------------
			// ------------------------------------------------------------

			// Recibir el mensaje de parte del proxy
			to_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
			printf("Recieved: %d bytes from client\n", to_read);

			if (!to_read) break; // Si ya no hay mas datos que leer, terminar la procesamiento 
			if (to_read < 0) on_error("Client read Error\n");


			// leemos el archivo  header
			memcpy(&lenSend,buffer, 2);


			/// leeamos el header del bit mao
			memcpy(&len,buffer + 4, 4);
			memcpy(&initData,buffer + 12, 4); // pos 10 en bitmap 
			memcpy(&widgt,buffer + 20, 4); //pos 18 
			memcpy(&height,buffer + 24, 4);
			memcpy(&paleta,buffer + 56, 8); // paleta inicia en 54
			//cast los valores a short (2 byts)
			lenSend = (short) len;
			heightSend = (short) height;
			widgtSend = (short) widgt;

			//creamos el bufferSend header
			memcpy(bufferSend,lenSend, 2);
			memcpy(bufferSend+2 , widgtSend, 2);
			memcpy(bufferSend+4, heightSend, 2);
			// bitmap 
			memcpy(bufferSend+6, buffer, 54);
			// capturo la paleta y lo invierto con esto pero no estoy seguro son 8 byts 
			paleta =  128 - paleta;

			memcpy(bufferSend+60, paleta, 8);


			for(int i =0 ; i < (initData - len);i++ ){ // 6 + 14 + 40 + 8
				bufferSend[68+i] = buffer[len - i]; // 68 byts del header y el bimap hasta 
			}
			
			printf("Se termino de convertir el bitmap");



			err = send(client_fd, buffer, to_read, 0); // Se envian los datos
			printf("Sent bytes: %d\n", err);

			if (err < 0) on_error("Client response failed\n");
			if (err != to_read) printf("Warning: wanted to send: %d, but sent %d\n", to_read, err);
		}
	}

	return 0;
}= 