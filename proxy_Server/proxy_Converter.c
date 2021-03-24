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
	// varibale de ayuda para guardar el numero del fichero en char
	char temp[3];

	// Este pedazo de la direccion no cambia para ningun frame
	char direccion_parcial[] = "PROXY-FRAMES/out-frame-";
	int pos = sizeof(direccion_parcial);

	// Se copia la direccion parcial en la direccion completa
	strcat(direccion_completa, direccion_parcial);

	if (numero_frame < 10) // Si el numero es menor a 10 hay que agregar dos 0
	{
		direccion_completa[pos] = '0';
		direccion_completa[pos+1] = '0';
	}
	else if (numero_frame < 100) // Si es menor a 100 solo un 0
	{
		direccion_completa[pos] = '0';
	}

	// Se convierte el numero a char[]
	sprintf(temp, "%d", numero_frame);
	// Se concatena el numero a la direccion completa
	strcat(direccion_completa, temp);

	// Se agrega el formato del archivo
	char formato[] = ".bmp";
	strcat(direccion_completa, formato);

	printf("Obtenido: %s\n", direccion_completa);
}

int obtener_Data_Size(char *direccion)
{
	// Obtencion del size de los datos -------------------------------------------------
	// ---------------------------------------------------------------------------------

	FILE *fichero;

	int data_size;

	// Se abre el fichero
	fichero = fopen(direccion, "r");
	// Se pone al puntero dentro del fichero en la posicion 0 y se le dice que lo recorra hasta el final
	fseek(fichero, 0, SEEK_END);
	// Se obtiene la posicion del puntero dentro del fichero y se la guarda
	data_size = ftell(fichero);

	printf("Data_Size: %d\n", data_size);

	// Se cierra el fichero
	fclose(fichero);

	return data_size;
}

void obtener_Fichero_Completo(request_converter_header *header, char *packet, char *direccion)
{
	int data_size = (header->total_size) - sizeof(header);
	FILE *fichero;
	// Se abre el fichero
	fichero = fopen(direccion, "r");

	// Variable auxiliar para guardar el data del fichero
	char aux[data_size];
	memset(aux, 0, sizeof(aux));
	// Se obtiene todos los datos fichero
	fgets(aux, data_size, fichero);

	// Se copia el header al paquete
	memcpy(packet, &(header->total_size), sizeof(header->total_size));
	// Se copia los datos obtenidos al paquete
	memcpy(packet + sizeof(header->total_size), aux, data_size);

	// Se cierra el fichero
	fclose(fichero);
}