// Solo recibira 1 byte de parte del esp32, indicando la
// orden a realizar
typedef struct
{
	char Flag;
}esp32_header;

typedef enum { todo_correcto, hubo_error, esperando } state;

// El converter solo necesita saber cuantos bytes debe recibir
// como el header es fijo, el bmp es el que hara variar al size
typedef struct 
{
	short Total_Size;
}request_converter_header;

// Indices para el header reply proxy esp32
#define WIDTH_INDEX 0
#define HEIGHT_INDEX 1

// El esp32 requiere del Width y Height para graficar la imagen
// cada uno ira codificado en un byte
typedef struct
{
	short Total_Size;
	char Width_Height[2];
}response_converter_header;

state analizar_Header_ESP32(int , char *);