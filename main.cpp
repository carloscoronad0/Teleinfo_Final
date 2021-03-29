#include <iostream>

using namespace std;


int DS; //Data Size
int DPos; //Data position
int x; //image width
int y; //image height
int LHeader = 6; //Header length converter-proxy protocol
char BUFFER [ 1092 ]; // 1024 datos maximos + 62 header BMP + 6 header protocolo converter-proxy (buffer de recepcion)

char BUFFERsend [ 1030 ]; //enviaremos la imagen bitmap siempre en 64x128 pixeles ( 64 * 16 bytes), rellenando lo necesario, junto con el header del protocolo proxy-converter (6bytes).
char BUFFERaux [ 1024 ]; //pondre todos los datos invertidos en orden y valor, con la resolucion exacta en este buffer.




int xr; // width de los datos ( considerando padding )
char aux; //char auxiliar para intercambiar datos


int main()
{
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
     for ( int i = DPos + LHeader; i < DS + LHeader; i ++ )
     {
         BUFFER [ i ] = ~( BUFFER[ i ] ); // "~" es la operacion NOR - cambiamos los bits de cada byte en los datos
     }
     //Segundo paso:
     //2.1. Encontrar los bytes que usan para representar una sola fila (incluyendo padding)
     xr = x + ( ( 32 - ( x % 32 ) ) % 32 );
     xr /= 8;

     //2.2. Voltear el buffer en grupos de "xr" manteniendo su orden interno.
     for( int i = DPos + LHeader, k = 0; k < (y / 2); i += xr, k ++ )
     {
         //copiamos la ultima fila de "xr" bytes correspondientes en los "xr" iniciales, mientras se hace el viceverso al mismo tiempo
         for( int j = 0; j < xr; j ++ )
         {
             aux = BUFFER[ i + j ]; //auxiliar para guardar el dato del "inicio" para ponerlo al "final" segun el ciclo
             BUFFER[ i + j ] = BUFFER[ DS + LHeader - ( xr * ( k + 1 ) ) + j ]; //Copiamos los "ultimos" xr bytes en los "primeros"
             BUFFER[ DS + LHeader - ( xr * ( k + 1 ) ) + j ] = aux; //Los "primeros" a los "ultimos"
         }
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
    /**este conjunto de fors acomoda solamente los datos en el BUFFERaux, de tal manera que la resolucion que vayamos a enviar es de 128x64
    o sea, no importa el buffer de entrada, tendremos siempre 1024 bytes que representen los 128x64 pixeles que necesita el ESP32.
    Este buffer no tiene el header, pero se le agregaria al BUFFERsend en los primeros 6 bytes (header length siempre constante).
    **/

    return 0;
}
