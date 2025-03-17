#include <PNGdec.h>
#include "SevenSeg.h"   // Seven Segment Display Font for clock
#include "NotoSansBold15.h"   // Standard Font

// Clock functionality
#include <WiFi.h>
#include "time.h"
#include "NetworkProperties.h"   // ssid and password for home network

// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library

#define MAX_IMAGE_WIDTH 320 // Adjust for your images

PNG png; // PNG decoder instance
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

// image display coord; xpos = pixels right from top left corner; ypos = pixels down from top left corner
int xpos = 0;
int ypos = 0;

// image processing
int imageSize = 0;  // length of image array
const char startMarker = '<'; //Marks the start of the image array
const char endMarker = '>'; //Marks the end of the image array
String content = "";  //temp String to hold serial input before insertion to data[]

// clock
const char* ntpServer = "us.pool.ntp.org";
const int gmtOffset_sec = -28800;   //-32400 when daylight savings active, -28800 otherwise (Alaska)
const int daylightOffset_sec = 0;   //offset in seconds
int prevSec = -1;
int prevHour = -1;
int prevMin = -1;

//display coordinate offsets
const int hourPos = 10;
const int secPos = 150;
const int minPos = 170;
const int clockYOffset = 40;
const int cpuTempX = 10;
const int gpuTempX = 170;
const int tempY = 140;

// clock toggle
bool clockEnabled = true;
const int clearImagePin = 13;
const int clearImagePout = 14;

// temperature
WiFiServer server(65432); //server port to listen for temperature client
int tempResetTime = 10000; // milliseconds
int prevUpdateTime = -tempResetTime; //allows server ip to show up instantly

const int doorPin = 12;
bool doorOpen = false;

void checkForTempUpdates(){
  static WiFiClient client;

  if (!client)
    client = server.available();  // Listen for incoming clients
    if (millis() - prevUpdateTime >= tempResetTime){  // Clear temperature area of screen and print the IP address if too much time has elapsed from last update
      tft.fillRect(0,139,320,15,TFT_BLACK);
      tft.setTextColor(TFT_WHITE,TFT_BLACK);  
      tft.loadFont(NotoSansBold15);
      tft.setCursor(cpuTempX, tempY);

      prevUpdateTime = millis();
    }
  if (client) {  // if client connected
    int bufferSize = 5;
    prevUpdateTime = millis();

    if (client.available() >= bufferSize) {    // if data available from client read and display it
      String readIn = client.readStringUntil('\n');
      String cpuTemp = "";
      String gpuTemp = "";
      cpuTemp += readIn[0];
      cpuTemp += readIn[1];
      gpuTemp += readIn[2];
      gpuTemp += readIn[3];

      // text formatting
      tft.loadFont(NotoSansBold15);
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
      // clear temperature text (x,y,w,h,color)
      tft.fillRect(0,139,320,15,TFT_BLACK);
      // print cpu temp
      tft.setCursor(cpuTempX, tempY);
      tft.print("CPU: ");
      tft.print(cpuTemp);
      tft.print("°C");
      // print gpu temp
      tft.setCursor(gpuTempX, tempY);
      tft.print("GPU: ");
      tft.print(gpuTemp);
      tft.print("°C");

      // clear buffer leftovers
      while (client.available()) 
        client.read();
    }
  }
}

void displayTimeInitError(){
  tft.loadFont(NotoSansBold15);
  Serial.println("Failed to obtain time");
  tft.setCursor(hourPos, clockYOffset);
  tft.setTextColor(TFT_RED,TFT_BLACK);
  tft.print("Failed to retrieve time");
  ESP.restart();
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    displayTimeInitError();
    return;
  }

  tft.loadFont(SevenSeg); // ONLY has '0-9' and ':' characters, size 86pt

  //Hour
  if(timeinfo.tm_hour != prevHour){
    // clear previous hour
    tft.setCursor(hourPos, clockYOffset);
    tft.setTextColor(TFT_BLACK,TFT_BLACK);
    tft.print("88");

    // display current hour
    tft.setCursor(hourPos, clockYOffset);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.print(&timeinfo, "%I");
  }

  //Seconds
  if(timeinfo.tm_sec % 2 == 0){
    // clear colon symbol
    tft.setCursor(secPos, clockYOffset);
    tft.setTextColor(TFT_BLACK,TFT_BLACK);
    tft.print(":");
  }
  else{
    // display colon symbol
    tft.setCursor(secPos, clockYOffset);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.print(":");
  }

  //Minutes
  if(timeinfo.tm_min != prevMin){
    // clear previous minute
    tft.setCursor(minPos, clockYOffset);
    tft.setTextColor(TFT_BLACK,TFT_BLACK);
    tft.print("88");

    // display current minute
    tft.setCursor(minPos, clockYOffset);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.print(&timeinfo, "%M");
  }

  //Placeholder thing until I get an idea for what to do with the door
  if (digitalRead(doorPin) == 0 && doorOpen){
    tft.loadFont(NotoSansBold15);
    tft.setCursor(10, 10);
    tft.fillRect(10,10,50,17,TFT_BLACK);
    tft.setTextColor(TFT_RED,TFT_BLACK);
    tft.print("Closed");
    doorOpen = false;
  } else if(digitalRead(doorPin) == 1 && !doorOpen){
    tft.loadFont(NotoSansBold15);
    tft.setCursor(10, 10);
    tft.fillRect(10,10,50,15,TFT_BLACK);    tft.setTextColor(TFT_RED,TFT_BLACK);
    tft.print("Open");
    doorOpen = true;
  }

  prevSec = timeinfo.tm_sec;
  prevMin = timeinfo.tm_min;
  prevHour = timeinfo.tm_hour;
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid, password);
}


//=========================================v==========================================
//                                      pngDraw
//====================================================================================
// This next function will be called during decoding of the png file to
// render each image line to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
// Callback function to draw pixels to the display
void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  xpos = ((MAX_IMAGE_WIDTH / 2) - (png.getWidth() / 2));  // center image on screen
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}


//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(115200, SERIAL_8E1);
  Serial.setRxBufferSize(8196);

  // Run WiFiStationDisconnected() when wifi disconnects
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  // Connect to wifi
  WiFi.begin(ssid, password);


  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.loadFont(NotoSansBold15);
  tft.setCursor(hourPos, clockYOffset-15);
  tft.print("Establishing WiFi Connection.");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    tft.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");

  // Start server for temperature data
  server.begin();

  //initialize LCD Screen
  tft.begin();
  tft.setRotation(1);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Set "cursor" at top left corner of display (0,0) and select font 1
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  tft.setCursor(0, 0);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.fillScreen(TFT_BLACK);

  pinMode(clearImagePin, INPUT_PULLDOWN);
  pinMode(clearImagePout, OUTPUT);
  pinMode(doorPin, INPUT_PULLDOWN);

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    displayTimeInitError();
    return;
  }
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{
  if (Serial.available() && Serial.read() == startMarker) {
    tft.loadFont(NotoSansBold15);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);  
    tft.setTextSize(1);
    tft.print("Incoming Image...");

    int iter = 0;

    //read buffer until first comma, then input that value into imageSize as integer
    imageSize = std::stoi(Serial.readStringUntil(',').c_str());

    //initialize dynamic char array in heap of length imageSize
    unsigned char* data = new unsigned char[imageSize];

    //loop and read in serial data to data[] until end marker is reached
    while(true){
      if(Serial.available()){
        char readIn = Serial.read();

        if(readIn == endMarker){
          data[iter] = (unsigned char)std::stoi(content.c_str());
          content = "";
          break;
        }
        else if(readIn == ','){
          data[iter] = (unsigned char)std::stoi(content.c_str());
          content = "";
          iter++;
        }
        else{
          content += readIn;
        }
      }
    }

    tft.fillScreen(TFT_BLACK);

    //display data[] as image
    int16_t rc = png.openFLASH((uint8_t *)data, imageSize, pngDraw);
    if (rc == PNG_SUCCESS) {
      tft.startWrite();
      rc = png.decode(NULL, 0);
      tft.endWrite();
    }

    clockEnabled = false;
    digitalWrite(clearImagePout, HIGH);

    // reset variables
    String content = "";
    delete[] data;
    imageSize = 0;

    //clear serial buffer
    while(Serial.available() > 0)
      char temp = Serial.read();
  }
  else{  // If no serial input is detected, print the time
    if(!clockEnabled && digitalRead(clearImagePin) == 1){
      tft.fillScreen(TFT_BLACK);
      prevSec = -1;
      prevHour = -1;
      prevMin = -1;
      clockEnabled = true;
      digitalWrite(clearImagePout, LOW);
    }
    if(clockEnabled){
      printLocalTime();
      checkForTempUpdates();
    }
  }
}

