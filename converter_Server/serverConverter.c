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

#define BUFFER_SIZE 1028
#define PORT_INDEX 1

void convertir(int DS, int DPos, int x, int y, int LHeader, unsigned char *BUFFER, unsigned char *BUFFERaux)
{
	int xr;
	// char aux;

	// Despues de haber recibido los datos, es decir, tener lleno el buffer de chars
    // (Header protoclo proxy-converter + BMP )con la información que llega del proxy
    // sacamos los siguientes datos:
    /**
    DS del bitmap con header BMP en el byte[ LHeader + 2 ] de 4 bytes de longitud.
    DPos del inicio del bitmap está en el byte[ LHeader + 10 ] de 4 bytes de longitud.
    x, width de la imagen, está en el byte [ LHeader + 18 ] de 4 bytes de longitud. (esta codificado como entero con signo)
    y, height de la imagen, está en el byte [ LHeader + 22 ] de 4 bytes de longitud (esta codificado como entero con signo)
     **/
     //Ahora, el procedimiento que seguiremos, tiene tres pasos:
     // Primero invertimos los bits ( 1 a 0 y 0 a 1 ) de los datos del bitmap, que es lo mismo que intercambiar los colores blanco por negro y viceversa.
     // Segundo, invertimos las filas, intercambiando la primera con la última, según el ancho, considerando que el padding del bmp rellena el ancho a un múltiplo de 4 bytes.
     // Tercero, rellenamos de '0's (ya sea a lo vertical, horizontal u ambos) hasta que se obtenga un bitmap que cumpla con la resolucion de 128x64.
     //Primer paso:
     /*for ( int i = DPos + LHeader; i < DS + LHeader; i ++ )
     {
         BUFFER [ i ] = ~( BUFFER[ i ] ); // "~" es la operacion NOR - cambiamos los bits de cada byte en los datos
     }*/
     //Segundo paso:
     //2.1. Encontrar los bytes que usan para representar una sola fila (incluyendo padding)
     xr = x + ( ( 32 - ( x % 32 ) ) % 32 );
     xr /= 8;

     unsigned char aux_xr[xr];

     //2.2. Voltear el buffer en grupos de "xr" manteniendo su orden interno.
     for( int i = DPos + LHeader, k = 0; k < (y / 2); i += xr, k ++ )
     {
         //copiamos la ultima fila de "xr" bytes correspondientes en los "xr" iniciales, mientras se hace el viceverso al mismo tiempo
     	memcpy(aux_xr, BUFFER + i, xr);
     	memcpy(BUFFER + i, BUFFER + DS + LHeader - ( xr * ( k + 1 ) ), xr);
     	memcpy(BUFFER + DS + LHeader - ( xr * ( k + 1 ) ), aux_xr, xr);
        /* for( int j = 0; j < xr; j ++ )
         {
             aux = BUFFER[ i + j ]; //auxiliar para guardar el dato del "inicio" para ponerlo al "final" segun el ciclo
             BUFFER[ i + j ] = BUFFER[ DS + LHeader - ( xr * ( k + 1 ) ) + j ]; //Copiamos los "ultimos" xr bytes en los "primeros"
             BUFFER[ DS + LHeader - ( xr * ( k + 1 ) ) + j ] = aux; //Los "primeros" a los "ultimos"
         }
         */
     }
     //Tercer paso:
     //Recordemos que 128x64 es 128 para width y 64 para height
     //Verificar si width cumple con la resolucion necesaria, sino llenarla de 0s, ademas de agregarla al buffer para enviar
     //Se hace lo mismo para el height.

    for( int i = 0; i < 64; i ++ )
    {
        if( i < y )
        {
        	for( int j = 0; j < 16; j ++ )
        	{
            	if( j < xr )
            	{
                	BUFFERaux[ i*16 + j ] = BUFFER[ i*xr + j ];
            	}
            	else
            	{
                	BUFFERaux[ i*16 + j ] = '0';
            	}
        	}
        }
        else
        {
        	for( int j = 0; j < 16; j ++ )
        	{
            	BUFFERaux[ i*16 + j ] = '0';
        	}
        }

    }
}

int main(int argc, char const *argv[])
{
	if (argc < 2) on_error("Usage: %s [port] \n", argv[0]);

	int PORT = atoi(argv[PORT_INDEX]);

	// SETTING SOCKET -----------------------------------------------------
	// --------------------------------------------------------------------
	
	int server_fd, client_fd, err;
	struct sockaddr_in server, client;
	unsigned char buffer[BUFFER_SIZE];

	// Auxi var 
	unsigned char bufferaux[BUFFER_SIZE];
	unsigned char bufferSend[BUFFER_SIZE];
	int len;
	int widgt;
	int height; 
	int paletaN;
	int paletaB;

	int initData;

	short lenSend;

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
				if (to_read < 0) on_error("Client read Error\n");

				memcpy(buffer + recieved, aux, to_read);
				recieved += to_read;
			}

			// leemos el archivo  header
			memcpy(&lenSend, buffer, 2);

			while(recieved < lenSend)
			{
				to_read = recv(client_fd, aux, sizeof(aux), 0);
				printf("Recieved: %d bytes from client\n", to_read);
				if (to_read < 0) on_error("Client read Error\n");

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

			convertir(len, initData, widgt, height, 2, buffer, bufferaux);

			lenSend = 1028;

			int offset = 0;

			memcpy(bufferSend + offset, &lenSend, sizeof(lenSend));
			offset += sizeof(lenSend);

			memset(bufferSend +offset, 0, 2);
			offset+=2;
			printf("Offset: %d\n", offset);

			memcpy(bufferSend + offset, bufferaux, 1024);
			
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