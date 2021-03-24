#include <stdio.h>		// Libreria basica
#include <stdlib.h>		// Libreria basica
#include <string.h> 	// Para el uso de memcpy
#include "proxy.h"

state analizar_Header_ESP32(int to_read, esp32_header *header)
{
	// Estado para guardar la orden del ESp32
	state orden_header;

	if (!to_read) // Si no hay nada para leer
	{
		// El estado es esperando
		orden_header = esperando;
		printf("Header: Esperando\n");
	}
	else if (to_read < 0) // Si el valor es negativo es que hubo un error
	{
		on_error("Client read Error\n");
	}
	else // Si el valor es positivo es que hay algo para leer
	{
		short ORDEN = (short) header->Flag;

		printf("Numero recivido: %hi\n", ORDEN);

		if (ORDEN > 0) // Si lo recibido es un numero mayor a 0 
		{
			// Es que hubo un error
			orden_header = hubo_error;
			printf("Header: Hubo error numero = %hu\n", ORDEN);
		}
		else // Sino
		{
			// Todo esta correcto y se envia el proximo frame
			orden_header = todo_correcto;
			printf("Header: Todo correcto\n");
		}
	}

	return orden_header;
}