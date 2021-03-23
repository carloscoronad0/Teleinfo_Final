#include <stdio.h>		// Libreria basica
#include <stdlib.h>		// Libreria basica
#include <sys/types.h>  // managing socket
#include <sys/socket.h> // managing socket
#include <netinet/in.h> // sockaddr_in struct
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "proxy.h" 		// Necesaria para las estructuras del proxy

void obteniendo_direccion(int numero_frame, char* direccion_completa)
{
	char temp[3];

	int pos = 22;
	char direccion_parcial[] = "PROXY-FRAMES/out-frame-";
	strcat(direccion_completa, direccion_parcial);

	if (numero_frame < 10)
	{
		direccion_completa[pos] = '0';
		direccion_completa[pos+1] = '0';
	}
	else if (numero_frame < 100)
	{
		direccion_completa[pos] = '0';
	}

	sprintf(temp, "%d", numero_frame);
	strcat(direccion_completa, temp);

	char formato[] = ".bmp";
	strcat(direccion_completa, formato);

	printf("Obtenido: %s\n", direccion_completa);
}

int obtener_Data_Size(char *direccion)
{
	// Obtencion de los datos ----------------------------------------------------------
	// ---------------------------------------------------------------------------------

	FILE *fichero;

	int data_size;

	fichero = fopen(direccion, "r");
	fseek(fichero, 0, SEEK_END);
	data_size = ftell(fichero);

	printf("Data_Size: %d\n", data_size);

	fclose(fichero);

	return data_size;
}

void obtener_Fichero_Completo(int data_size, char *packet, char *direccion)
{
	short total_size = sizeof(packet);

	FILE *fichero;
	fichero = fopen(direccion, "r");

	char aux[data_size];
	memset(aux, 0, sizeof(aux));
	fgets(aux, data_size, fichero);

	memcpy(packet, &total_size, sizeof(total_size));
	memcpy(packet + sizeof(total_size), aux, data_size);

	fclose(fichero);
}