#include <ESP8266WiFi.h>
#include "Adafruit_GFX.h"
#include "icons.h"
#include "Adafruit_ILI9341.h"
#include "Fonts/FreeSerif9pt7b.h"

const int RSSI_MAX =-50;// define maximum strength of signal in dBm
const int RSSI_MIN =-100;// define minimum strength of signal in dBm

// TFT Display.
#define TFT_DC 2
#define TFT_CS 15
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

typedef struct {
  String BSSID;
  String SSID;
  uint8_t encryptionType;
  int32_t RSSI;
  int32_t Channel;
  uint8_t Active;
  uint8_t Update;
} NetInfo;
NetInfo networkInfo[30];

typedef struct {
  int32_t Total;
  int32_t RSSI;
} ChannelInfo;
ChannelInfo totalChannel[14];

//,totalChannelOld[14];

#define TEXT_SIZE 12

int initclear = 1;

void setup() {
//  system_update_cpu_freq(160);
    Serial.begin(115200);
  // Battery voltage
  pinMode(A0, INPUT);

  //  Setup TFT;
  tft.begin();
  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  splashScreen();


  // Start WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(75, 200);
  tft.println("Scanning WiFi...");
  scanNetwork();
  BubbleSort(networkInfo, 30);
  //delay(4000);
  tft.fillScreen(ILI9341_BLACK);  
  
  memset( networkInfo, 0x0, sizeof(networkInfo) );
  memset( totalChannel, 0x0, sizeof(totalChannel) );
//  memset( totalChannelOld, 0x0, sizeof(totalChannelOld) );
//  tft.setTextSize(1);

  tft.drawRect(1, 12, 319, 107, ILI9341_DARKGREY); // ssid list
  tft.drawRect(1, 120, 319, 119, ILI9341_DARKGREY); // ssid chart


}


void loop() {
  // Update Battery Voltage
  checkBatt();

    drawGraphScreen(); 
//    BubbleSort(networkInfo, 30);
    showScreen1(30);
    drawGraph();
  //Scan network
  scanNetwork();
  BubbleSort(networkInfo, 30);



// -------------------------------
//  int n = WiFi.scanNetworks();
//  int m = n;
//  NetInfo netInfo;
//  for (int i = 0; i < 30; ++i) {
//    networkInfo[i].Update = 0;
//  }
//  for (int i = 0; i < 14; ++i) {
//   totalChannel[i].Total = 0;
//   totalChannel[i].RSSI = 0;
//  }
//  if ( initclear == 1 ) {
//    initclear = 0;
//    tft.fillScreen(ILI9341_BLACK);
//    drawGraphScreen();    
//  }
//  if (n == 0) {
//    tft.setTextSize(2);
//    tft.fillScreen(ILI9341_BLACK);
//    tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
//    tft.setCursor(0, 10);
//    tft.println();
//    tft.println();
//    tft.print("No networks found...");
//    tft.println();
//    tft.println("Scanning...");
//    tft.setTextSize(1);
//  } else {
//    if ( m > 30 ) m = 30;
//    for (int i = 0; i < m; ++i) {
//      netInfo.SSID = WiFi.SSID(i);
//      netInfo.encryptionType = WiFi.encryptionType(i);
//      netInfo.RSSI = WiFi.RSSI(i);
//      netInfo.Channel = WiFi.channel(i);
//      netInfo.Active = 1;
//      netInfo.Update = 1;
//      addToArray(netInfo);
//      totalChannel[netInfo.Channel - 1].Total = totalChannel[netInfo.Channel - 1].Total + 1;
//      if ( (netInfo.RSSI + 100) > totalChannel[netInfo.Channel - 1].RSSI ) {
//        totalChannel[netInfo.Channel - 1].RSSI = (netInfo.RSSI + 100);
//      }
//    }
//    for (int i = 0; i < 30; ++i) {
//      if ( networkInfo[i].Update == 0 ) {
//        networkInfo[i].SSID = "";
//        networkInfo[i].RSSI = 255;
//        networkInfo[i].Active = 0;
//      }
//    }
//    BubbleSort(networkInfo, 30);
//    tft.setCursor(5, 2);
//    tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
//    tft.print(n);
//    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
//    tft.print(" networks found");
//    tft.fillRect(0, 0, 320, 119, ILI9341_BLACK);
//    showScreen1(30);
//    drawGraph();
//  }

}

void scanNetwork() {
  Serial.println("Wifi scan started");
  for (int i = 0; i < 14; ++i) {
   totalChannel[i].Total = 0;
   totalChannel[i].RSSI = 0;
  }
  for (int i = 0; i < 30; ++i) {
    networkInfo[i].Update = 0;
  }
//memset(totalChannel, 0, sizeof(totalChannel));
//memset(networkInfo, 0, sizeof(networkInfo));
  NetInfo netInfo;

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("Wifi scan ended");
  if (n == 0) {
    tft.setCursor(2, 2);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.print("No networks found !");
  } else {
    tft.setCursor(2, 2);
    tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
    tft.print(n);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.print(" networks found");
    if ( n > 30 ) n = 30;
    for (int i = 0; i < n; ++i) {
      totalChannel[WiFi.channel(i) - 1].Total = totalChannel[WiFi.channel(i) - 1].Total + 1;
      if ( (WiFi.RSSI(i) + 100) > totalChannel[WiFi.channel(i) - 1].RSSI ) {
        totalChannel[WiFi.channel(i) - 1].RSSI = (WiFi.RSSI(i) + 100);
      }
      netInfo.BSSID = WiFi.BSSIDstr(i);
      netInfo.SSID = WiFi.SSID(i);
      netInfo.encryptionType = WiFi.encryptionType(i);
      netInfo.RSSI = WiFi.RSSI(i);
      netInfo.Channel = WiFi.channel(i);
      netInfo.Active = 1;
      netInfo.Update = 1;
      addToArray(netInfo);



//    tft.setTextSize(1);
//    tft.printf("Ch:%d, %s\n", WiFi.channel(i), WiFi.SSID(i).c_str());
//    tft.setTextSize(1);
//    tft.print(WiFi.RSSI(i));//Signal strength in dBm  
//    tft.print("dBm (");
//    tft.print(dBmtoPercentage(WiFi.RSSI(i)));//Signal strength in %  
//    tft.print("% )");
//    tft.print(" MAC:");
//    tft.print(WiFi.BSSIDstr(i)  );//MAC address  (Basic Service Set Identification) 
//    tft.print(" Encryption:");      
//    tft.println(encType(i));    
      
      // Print SSID and RSSI for each network found
/*      Serial.print(i + 1);
      Serial.print(") ");
      Serial.print(WiFi.SSID(i));// SSID     
      Serial.print(" ch:");  
      Serial.print(WiFi.channel(i));// display channel
      Serial.print(" ");     
      Serial.print(WiFi.RSSI(i));//Signal strength in dBm  
      Serial.print("dBm (");
      Serial.print(dBmtoPercentage(WiFi.RSSI(i)));//Signal strength in %  
      Serial.print("% )");
      Serial.print(" MAC:");
      Serial.print(WiFi.BSSIDstr(i)  );//MAC address  (Basic Service Set Identification) 
      if(WiFi.isHidden(i) ){
       Serial.print(" <<Hidden>> ");           
      }
        Serial.print(" Encryption:");      
        Serial.print(encType(i));
        Serial.println(WiFi.encryptionType(i));
*/    
//      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
//  delay(5000);
  WiFi.scanDelete();
}











void showScreen1( int num ) {
/*  for (int i = 0; i < num; ++i) {
    Serial.print(networkInfo[i].SSID);// SSID     
    Serial.print(" ");     
    Serial.print(networkInfo[i].RSSI);//Signal strength in dBm  
    Serial.println("dBm ");
  }*/
  tft.fillRect(2, 13, 317, 105, ILI9341_BLACK);
  int rowHeight = 12, left = 2, top = 12, colWidth = 150;
  int x = left, y = top;  
  for (int i = 0; i < num; ++i) {
    if (networkInfo[i].Active == 1) {
      tft.setCursor(x, y);      
      if (networkInfo[i].encryptionType == ENC_TYPE_NONE) {
        tft.drawBitmap(x, y, unlockBitmap10, 10, 10, ILI9341_GREEN, ILI9341_BLACK);
      } else {
        tft.drawBitmap(x, y, lockBitmap10, 10, 10, ILI9341_RED, ILI9341_BLACK);
      }
      tft.setCursor(x + 16, y);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      if (networkInfo[i].RSSI <= -80) {
        tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
      } else if (networkInfo[i].RSSI >= -80 && networkInfo[i].RSSI < -70) {
        tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      } else {
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      }
      tft.print(networkInfo[i].SSID);
      y=y+rowHeight;
      if (y>=120){
        y=top;
        x=left+colWidth;
      }
    }
  }




  
  /*int pos = 1, col = 0, total = 0;
  for (int i = 0; i < num; ++i) {
    if (networkInfo[i].Active == 1) {
      tft.fillRect(col+2, 12 + (TEXT_SIZE * pos), 160, TEXT_SIZE, ILI9341_BLACK);
      tft.setCursor(col+2, 12 + (TEXT_SIZE * pos));      
      if (networkInfo[i].encryptionType == ENC_TYPE_NONE) {
        tft.drawBitmap(col+2, (TEXT_SIZE * pos), unlockBitmap10, 10, 10, ILI9341_GREEN, ILI9341_BLACK);
      } else {
        tft.drawBitmap(col+2, (TEXT_SIZE * pos), lockBitmap10, 10, 10, ILI9341_RED, ILI9341_BLACK);
      }
      tft.setCursor(col + 16, 2 + (TEXT_SIZE * pos));
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      if (networkInfo[i].RSSI <= -80) {
        tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
      } else if (networkInfo[i].RSSI >= -80 && networkInfo[i].RSSI < -70) {
        tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      } else {
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      }
      
      tft.print(networkInfo[i].SSID);

      /*Serial.print(networkInfo[i].SSID);
      Serial.print(" : ");
      Serial.println(networkInfo[i].RSSI);*/
      
      //tft.fillRect(0, (TEXT_SIZE * pos), 240, TEXT_SIZE, ILI9341_BLACK);
      /*if (networkInfo[i].RSSI <= -80) {
        tft.drawBitmap(120, 20 + (TEXT_SIZE * pos), level1Bitmap, 8, 8, ILI9341_RED, ILI9341_BLACK);
      } else if (networkInfo[i].RSSI >= -80 && networkInfo[i].RSSI < -70) {
        tft.drawBitmap(120, 20 + (TEXT_SIZE * pos), level2Bitmap, 8, 8, ILI9341_YELLOW, ILI9341_BLACK);
      } else {
        tft.drawBitmap(120, 20 + (TEXT_SIZE * pos), level3Bitmap, 8, 8, ILI9341_GREEN, ILI9341_BLACK);
      }*/
/*      pos++;
      total++;
      if ( total > 21) break;
      if ( total == 11 ) {
        col = 160;
        pos = 0;
      }
    }
  }*/

  
}







void BubbleSort(NetInfo netInfo[], int numLength) {
  uint8_t i, j, flag = 1;    // set flag to 1 to start first pass
  NetInfo temp;             // holding variable
  for (i = 1; (i <= numLength) && flag; i++){
    flag = 0;
    for (j = 0; j < (numLength - 1); j++){
      if (abs(netInfo[j + 1].RSSI) < abs(netInfo[j].RSSI)){    // ascending order simply changes to <
        temp = netInfo[j];             // swap elements
        netInfo[j] = netInfo[j + 1];
        netInfo[j + 1] = temp;
        flag = 1;               // indicates that a swap occurred.
      }
    }
  }
  return;   //arrays are passed to functions by address; nothing is returned
}

void addToArray( NetInfo netInfo ) {
  uint8_t total = 0;
  for (int i = 0; i < 30; ++i) {
    if ( networkInfo[i].Active == 1 && (networkInfo[i].BSSID == netInfo.BSSID) ) {
      networkInfo[i] = netInfo;
      return;
    } 
  }
  for (int i = 0; i < 30; ++i) {    
    if ( networkInfo[i].Active == 1 ) {
      total++;
    } 
  }
  if ( total < 30 ) {
    networkInfo[total] = netInfo;
  }
}

void drawGraphScreen() {
  tft.setCursor(50, 220);
  tft.setTextColor(ILI9341_PURPLE, ILI9341_BLACK);
  tft.print("01 02 03 04 05 06 07 08 09 10 11 12 13 14");
  tft.drawFastHLine(40, 215, 265, ILI9341_ORANGE);
  tft.setTextColor(ILI9341_PURPLE, ILI9341_BLACK);
  tft.setCursor(15, 205);
  tft.print("-99");
  tft.setCursor(15, 195);
  tft.print("-80");
  tft.setCursor(15, 185);
  tft.print("-70");
  tft.setCursor(15, 175);
  tft.print("-60");
  tft.setCursor(15, 165);
  tft.print("-50");
  tft.setCursor(15, 155);
  tft.print("-40");
  tft.setCursor(15, 145);
  tft.print("-30");
  tft.setCursor(15, 135);
  tft.print("-20");
  tft.drawFastVLine(40, 135, 80, ILI9341_ORANGE);
}

void drawGraph() {
  tft.fillRect(41, 135, 265, 80, ILI9341_BLACK);
  for (int i = 0; i < 14; ++i) {
//    tft.drawFastVLine(55 + (i * 18), 125, 75, ILI9341_BLACK);
//    if ( totalChannelOld[i].Total > 0) {
//      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
//      tft.setCursor(60 + (i * 18), 210 - totalChannelOld[i].RSSI);
//      tft.print("  ");
//    }  
    if ( totalChannel[i].Total > 0) {
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.setCursor(60 + (i * 18), 210 - totalChannel[i].RSSI);
      tft.print(totalChannel[i].Total);
      tft.drawFastVLine(55 + (i * 18), 210 - totalChannel[i].RSSI, totalChannel[i].RSSI, ILI9341_GREEN);
    } 
//    totalChannelOld[i] = totalChannel[i];
  }
}

void splashScreen(){
  tft.fillScreen(ILI9341_BLACK);
  tft.drawBitmap(110, 10, wifiBitmap, 100, 70, ILI9341_WHITE);
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(80, 106);
  tft.print("Wifi Scan");
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setCursor(70, 130);
  tft.print("Omega Software Service");
  }

void checkBatt(){
  float value = 0;
  for (int i = 0; i < 10; ++i) {
    value = value + analogRead(A0);
  }
  value = value / 10;
  tft.setFont();
  tft.setTextSize(1);
  tft.drawBitmap(250, 2, battBitmap10, 20, 10, ILI9341_BLUE, ILI9341_BLACK);
  tft.setCursor(275, 2);
  tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);    
  tft.print(value * 9 / 945);
  tft.print(" V");
}

int dBmtoPercentage(int dBm){
  int quality;
    if(dBm <= RSSI_MIN){
      quality = 0;
    }
    else if(dBm >= RSSI_MAX){
      quality = 100;
    }
    else{
      quality = 2 * (dBm + 100);
    }
  return quality;
}

String encType(int id){
  String type;
  if(WiFi.encryptionType(id) == ENC_TYPE_WEP){
    type=" WEP";
  }
  else if(WiFi.encryptionType(id) == ENC_TYPE_TKIP){
    type="WPA / PSK";    
  }
  else if(WiFi.encryptionType(id) == ENC_TYPE_CCMP){
    type="WPA2 / PSK";    
  }
  else if(WiFi.encryptionType(id) == ENC_TYPE_AUTO){
    type="WPA / WPA2 / PSK";    
  }
  else if(WiFi.encryptionType(id) == ENC_TYPE_NONE){
    type="<<OPEN>>";
  }    
  else if(WiFi.encryptionType(id) == 255){
    type="Unknown";    
  }
  return type;
//1:  ENC_TYPE_WEP – WEP
//2 : ENC_TYPE_TKIP – WPA / PSK
//4 : ENC_TYPE_CCMP – WPA2 / PSK
//7 : ENC_TYPE_NONE – open network
//8 : ENC_TYPE_AUTO – WPA / WPA2 / PSK
}
