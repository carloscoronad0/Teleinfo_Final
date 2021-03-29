//                    Oled Image
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//                     TCP
#include <ctype.h>
#include <SPI.h>
#include <WiFi.h>
//    Wifi Network
char ssid[] = "COMTECO-96013956";          //  your network SSID (name)
char pass[] = "ZHBGQ85157"  ;   // your network password

int status = WL_IDLE_STATUS;
char servername[]="192.168.100.21";  // server
int port = 49000;

WiFiClient client;

// buffer to receive
const int BUFF_SIZE = 5500;
unsigned char recive_buff[BUFF_SIZE+1];
int width;
int height;

char send_Next = 0;
char send_Error = 1;

void setup() {
   Serial.begin(115200);
   //      Connect to the network 
   connectWifi();
   
   Serial.println("\nStarting connection...");
   // handshake succesful..
   if (client.connect(servername, port)) {
    Serial.println("connected");
   }
          // inicializamos la pantalla 
   Serial.println("Show IMAGE...");             
 //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
}

void loop() {
  Serial.println("Start the film");
  //bucle
  while(true){
    //request to the server for the bitmap
    toWrite(send_Next);
    delay(50);
    // receive from the server the bitbap
    toRead();
    display.clearDisplay(); // Make sure the display is cleared
    Serial.print("Ancho ");
    Serial.println(width);
    Serial.print("Largo ");
    Serial.println(height);
    //cargar bitmap
    display.drawBitmap(0,0,recive_buff, 128,64,WHITE);
    //mostrar
    display.display();
    Serial.println("Esperar ......");
    delay(100);
    display.clearDisplay(); // Make sure the display is cleared

  }
  
}
void toRead( ){
  Serial.println("leemos");
  for(;client.available()<4 ;){ 
  } 
  int c;
  int to_read;
  char cLen[2];
  short len; 
  cLen[0]= client.read();
  cLen[1]= client.read();
  memcpy(&len, cLen,2);
  width  = (int) client.read();
  height = (int) client.read();
  Serial.print("Tamano total Archivo");
  Serial.println(len);


  // CheckSum
  for(int i = 0 ; i<(len-4);){
    to_read = client.available();
    Serial.print("bucle ");
    Serial.println(to_read);
    for(int j = 0; j<to_read;j++){
     recive_buff[i] = client.read();
     i++;

     
    }
  }
  for(int i =0 ; i<(len-4);i++){
      Serial.print(recive_buff[i]);

   }
   Serial.println("");
}

void toWrite(char b){
  client.write(b);  
}

void connectWifi(){
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
}
