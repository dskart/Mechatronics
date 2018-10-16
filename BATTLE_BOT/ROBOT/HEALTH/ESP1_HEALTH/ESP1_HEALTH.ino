//Include Libraries
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include "Esp.h"
#include <SD.h>
#include "Wire.h"
#include "Adafruit_MCP23017.h"


#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// pins used for SPI communication
#define _sclk SCLK //D5
#define _miso MISO //D6
#define _mosi MOSI //D7 
#define _cs 10 //SD3
#define _dc 9 //SD2
#define _rst D4
#define SD_CS D0

// other pins
#define frequency_pin D1 // MCP
#define dead_pin      3
#define LED_pin       D8 //MCP

//set spi comunication
Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
//Adafruit_MCP23017 mcp;
//---------------------------------------GLOBAL VARIABLES----------------------------------------------
int TEAM = 1; // red = 1 blue = 2
int health_data_start;
int robot_number = 1; // player number
int LED_timeout1;
int value;
int n = 1;
int time0;
int time1;
int wanted_Hz = 226;
int my_health = 50;
int DEAD = 0;
int old_health = 0;

//---------------------------------------WIFI----------------------------------------------------------
// set up wifi const
//const char* ssid = "modlab1";
//const char* password = "ESAP2017";
const char* ssid = "Team2";
const char* password = "wreckandroll";

// set up wifi port to read and packet size
WiFiUDP UDPTestServer;
unsigned int UDPPort = 2390;
const int packetSize = 22;
String ReadData;
byte packetBuffer[packetSize];

//-------------------------------------------------SETUP---------------------------------------------------------------
void setup() {
  
  // set serial communication and pins
  Serial.begin(115200); while (!Serial);

  //set Cs pin that talks to the SD card in the LCD screen
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  //initializes the SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");
  
  //set output pins
  pinMode(LED_pin, OUTPUT); // healing LED
  pinMode(dead_pin, OUTPUT);

  //set input pins
  pinMode(frequency_pin, INPUT);

  // turns on motors switch
  digitalWrite(dead_pin, HIGH);

  Serial.print("robot_number ");
  Serial.println(robot_number);
  Serial.print("TEAM ");
  Serial.println(TEAM);
  delay(100);

  //CONNECTION TO WIFI
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //begin wifi connection
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 1, 40),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));

  // wait for wifi status to me connected
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi connected");

  // begin udpport
  UDPTestServer.begin(UDPPort);
  
  tft.begin();



  // know where to start reading your health depending on your team_color and number
  if (TEAM ==1){ // RED
  if( robot_number == 1){
    health_data_start = 3;
  }else if( robot_number == 2){
    health_data_start = 5;
  }else if( robot_number == 3){
    health_data_start = 7;
  }else if( robot_number == 4){
    health_data_start = 9;
  }

  }else if (TEAM == 2){ // BLUE
    if( robot_number == 1){
      health_data_start = 14;
    }else if( robot_number == 2){
      health_data_start = 16;
    }else if( robot_number == 3){
      health_data_start = 18;
    }else if( robot_number == 4){
      health_data_start = 20;
    }
  }
  
  yield();
  delay(100);

  //START DISPLAYING
  tft.setRotation(3);
  tft.fillScreen(ILI9340_BLACK);
  bmpDraw("SK.bmp", 0, 130);
  TEAM_color(TEAM);
  TEAM_number(robot_number);
  Display_health(my_old_health);
  

}

//-------------------------------------MAIN LOOP---------------------------------
void loop() {
  //start reading data from Wifi
  yield();
  
  //check how much has passed
  int LED_timeout2 =millis();

  //start reading data from Wifi
  handleUDPServer();

  //stores your health from the string depending on your player number and team color
  my_health = ReadData.substring(health_data_start,health_data_start+2).toInt();

  
  if (my_health ==0) {
    //DEAD CODE
    
    //turn off motors
    digitalWrite(dead_pin, LOW);
    
    // ADD DISPLAY CODE
    bmpDraw("death.bmp", 0, 0);

    //set dead variable for display
    DEAD = 1;
  }else{
    //NOT DEAD YET
      
    //if we were dead before, reset screen
    if (DEAD ==1){
      tft.fillScreen(ILI9340_BLACK);
      bmpDraw("SK.bmp", 0, 130);
      TEAM_color(TEAM);
      TEAM_number(robot_number);
      DEAD = 0;
    }

    //turn motors on
    digitalWrite(dead_pin, HIGH);

    // ADD DISPLAY CODE
    Display_health(my_health);

    
    //checks frequency for healingS
    int timer = millis();
    while ((millis() - timer) <= 50){ // run this function for 50ms to make sure we the phototransistor can read a frequency
      healing_sensor();
    }
      
    
    // turns out the healing LED after 2 seconds you stared healing
    if ((LED_timeout2 - LED_timeout1) > 2000){
      digitalWrite(LED_pin, LOW);
      LED_timeout1 = 0;
    }

  }

}

//----------------------------------------------------------FUNCTIONS----------------------------------------------

// check frequency from phototransistor and turns it on if it matches with wanted Frequency
void healing_sensor(){
  value = digitalRead(frequency_pin);
  //Serial.println(value);
  if (value == 0){
    if (n == 1){
      time0 = micros();
      n = 0;
    }
  }else{
    if (n == 0){
      time1 = micros();
      n=1;
    }
  }

  double Period = (time0-time1)*2.0;
  double Hz = 1.0/(Period*0.000001);
  if (Hz >= (wanted_Hz-20) && Hz <= (wanted_Hz+20)){
    digitalWrite(LED_pin, HIGH);
    delay(100);
    LED_timeout1 = millis();
  }
}


// this function listen at the wifi port and imports packet into a string
void handleUDPServer(){
  yield();

  int c = UDPTestServer.parsePacket();


  if (c) {
    UDPTestServer.read(packetBuffer, packetSize);
    ReadData = "";
    for(int i = 0; i < packetSize; i++){
      ReadData += (char)packetBuffer[i];
    }
  }
}


//--------------------------------------------------------DISPLAY FUNCTIONS------------------------------------------
//used to display the Health on the LCD screen
void Display_health(int health){
  tft.setCursor(54, 86);
  tft.setTextColor(ILI9340_GREEN); tft.setTextSize(4);
  tft.print("HEALTH "); 
  if (health != old_health){
    tft.fillRect(222 , 86, 44, 28, ILI9340_BLACK);
  }
  tft.print(health);
  tft.drawRect(60, 122, 200, 21, ILI9340_WHITE);

  int pixel_health = health*2;
  tft.fillRect(61, 123, pixel_health, 19, ILI9340_GREEN);
  tft.fillRect(61+pixel_health, 123, 198-pixel_health, 19, ILI9340_BLACK);
  old_health = health;

}

// used to display the team number
void TEAM_number(int robot_number){
  tft.setCursor(66,46);
  tft.setTextColor(ILI9340_WHITE); tft.setTextSize(4);
  tft.print("PLAYER "); tft.print(robot_number);
}

//used to display the team color
void TEAM_color(int TEAM){
  if (TEAM == 1){ // RED
    tft.setCursor(66, 6);
    tft.setTextColor(ILI9340_RED); tft.setTextSize(4);
    tft.print("RED TEAM");
    
  }else if (TEAM == 2){ // BLUE
    tft.setCursor(54, 6);
    tft.setTextColor(ILI9340_BLUE); tft.setTextSize(4);
    tft.print("BLUE TEAM");
  }
}



//-----------------------------------------------------OLED SD CARD READ/BITMAP DISPLAY---------------------------------------

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates. 
#define BUFFPIXEL 40

void bmpDraw(char *filename, uint16_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

//  /
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    //Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File & f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File & f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
