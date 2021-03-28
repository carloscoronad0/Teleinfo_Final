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

#define BUFFER_SIZE 16500
#define PORT_INDEX 1

int main(int argc, char const *argv[])
{
	if (argc < 2) on_error("Usage: %s [port] \n", argv[0]);

	int PORT = atoi(argv[PORT_INDEX]);

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
	int paletaN;
	int paletaB;

	int initData;

	short lenSend;
	char heightSend;
	char widgtSend;


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

		char aux[1000];
		int recieved = 0;

		while (1)
		{
			memset(buffer, 0, BUFFER_SIZE);
			memset(bufferSend, 0, BUFFER_SIZE);
			memset(aux, 0, sizeof(aux));

			// PROCESSING OF PACKETS RECIEVED -----------------------------
			// ------------------------------------------------------------

			// Recibir el mensaje de parte del proxy
			to_read = recv(client_fd, aux, sizeof(aux), 0);
			printf("Recieved: %d bytes from client\n", to_read);

			if (to_read < 0) on_error("Client read Error\n");

			recieved = to_read;
			memcpy(buffer, aux, to_read);

			while(recieved < sizeof(short))
			{
				to_read = recv(client_fd, aux, sizeof(aux), 0);
				printf("Recieved: %d bytes from client\n", to_read);

				memcpy(buffer + recieved, aux, to_read);
				recieved += to_read;
			}

			// leemos el archivo  header
			memcpy(&lenSend, buffer, 2);

			while(recieved < lenSend)
			{
				to_read = recv(client_fd, aux, sizeof(aux), 0);
				printf("Recieved: %d bytes from client\n", to_read);

				memcpy(buffer + recieved, aux, to_read);
				recieved += to_read;
			}

			// Se empieza con el analisis del archivo bmp -----------------------------------

			/// leeamos el header del bit mao
			memcpy(&len,buffer + 4, 4);
			memcpy(&initData,buffer + 12, 4); // pos 10 en bitmap 
			memcpy(&widgt,buffer + 20, 4); //pos 18 
			memcpy(&height,buffer + 24, 4);

			memcpy(&paletaN,buffer + 56, 4); // paleta inicia en 54
			memcpy(&paletaB,buffer + 60, 4); // paleta inicia en 54

			//cast los valores a short (2 byts)

			lenSend += 2;
			heightSend = height;
			widgtSend = widgt;

			//creamos el bufferSend header
			memcpy(bufferSend, &lenSend, 2);		//2
			memcpy(bufferSend+2 , &widgtSend, 1);	//3
			memcpy(bufferSend+3, &heightSend, 1);	//4
			// bitmap 
			memcpy(bufferSend+6, buffer, 54);     	//58  
			// capturo la paleta y lo invierto con esto pero no estoy seguro son 8 byts 
			
			memcpy(bufferSend+60, &paletaB, 4);		//62
			memcpy(bufferSend+64, &paletaN, 4);		//66

			printf("	los tamanos son initData: %d, y len: %d \n",initData , len);


			for(int i =0 ; i < ( len - initData);i++ ){ // 6 + 14 + 40 + 8
				bufferSend[68+i] = buffer[len - i]; // 68 byts del header y el bimap hasta 

				//printf("%c",buffer[len - i]);

			}
			
			printf("Se termino de convertir el bitmap\n");

			// -------------------------------------------------------------
			// Aca tiene que ir el size de los datos que me estas enviando
			// --------------------------------------------------------------
			err = send(client_fd, bufferSend, lenSend, 0); // Se envian los datos
			printf("Sent bytes: %d\n", err);

			if (err < 0) on_error("Client response failed\n");
		}
	}

	return 0;
}