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
int port = 49999;

WiFiClient client;
// buffer to receive
const int BUFF_SIZE = 5500;
unsigned char recive_buff[BUFF_SIZE+1];
int width;
int height;
char send_Next = '1';
char send_Error = '0';

void setup() {
   Serial.begin(115200);
   //         Connect to the network 
   connectWifi();
   
   Serial.println("\nStarting connection...");
   // handshake succesful..
   if (client.connect(servername, port)) {
    Serial.println("connected");
   }
          // Oled Init 
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
  while(true){
    toWrite(send_Next);
    delay(50);
    toRead();
    display.clearDisplay(); // Make sure the display is cleared
    display.drawBitmap(0, 0, recive_buff, width, height, WHITE);
  }
  
  
  
}
void toRead( ){
  for(;client.available()<3 ;){ 
  } 
  int to_read;
  int len = (int) client.read();
  width  = (int) client.read();
  height = (int) client.read();
  // CheckSum
  for(int i = 0 ; i<len; i++){
    to_read = client.available();
    for(int j = 0; j>to_read;j++,i++){
     recive_buff[i] = client.read();
    }
  }
}

void toWrite(char b){
  Serial.print(b);
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
