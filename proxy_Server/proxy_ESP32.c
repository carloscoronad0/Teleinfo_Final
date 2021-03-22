#include <stdio.h>		// Libreria basica
#include <stdlib.h>		// Libreria basica
#include <string.h> 	// Para el uso de memcpy
#include "proxy.h"

state analizar_Header_ESP32(int to_read, char *esp32_header)
{
	state orden_header;

	if (!to_read)
	{
		orden_header = esperando;
		printf("Header: Esperando\n");
	}
	else if (to_read < 0)
	{
		on_error("Client read Error\n");
	}
	else
	{
		short ORDEN;

		memcpy(&ORDEN, esp32_header , sizeof(ORDEN));
		if (ORDEN > 0)
		{
			orden_header = hubo_error;
			printf("Header: Hubo error numero = %hu\n", ORDEN);
		}
		else
		{
			orden_header = todo_correcto;
			printf("Header: Todo correcto\n");
		}
	}

	return orden_header;
}