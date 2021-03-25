// Funcion para manejar los errores
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(-1); }

// Indices para obtener los valores del argv del main
#define SERVER_PORT_INDEX 1
#define CONVERTER_PORT_INDEX 2

// Estructura de SERVIDOR del proxy
typedef struct
{
	int SERVER_PORT;
	int SERVER_FD;
	int CLIENT_FD;

	struct sockaddr_in proxy;
	struct sockaddr_in client;
} s_proxy_server;

// Estructura de CLIENTE del proxy
typedef struct
{
	int CONVERTER_PORT;
	int CLIENT_FD;

	char* IP_DIRECTION;

	struct sockaddr_in converter;
}s_proxy_client;

// Mencion de las funciones para que no surja errores al momento de la compilacion
void inicializar_Proxy_Server(s_proxy_server *);
void inicializar_Proxy_Client(s_proxy_client *);