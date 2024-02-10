/*Landscape Display*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include "weather_image.h"
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Wire.h>
#include <ctime>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <iostream>
#include <string>

//const char* ssid = "A12a.14";
//const char* password = "0563495007";
const char* ssid = "Suong Mai";
const char* password = "07032013";
//const char* ssid = "Gggggg";
//const char* password = "25862586";

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "f9980632671d4621dd517cddae9a7086";
String city = "Di An";
String countryCode = "VN";
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;
char buffer[10]; 
String jsonBuffer;
/***********************************************************END WEB **************************************/ 
// ST7735 TFT module connections
#define TFT_RST   2     // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    0     // TFT CS  pin is connected to NodeMCU pin D3 (GPIO0)
#define TFT_DC    15     // TFT DC  pin is connected to NodeMCU pin D8 (GPIO15)
// initialize ST7735 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
/***********************************************************END TFT ******************************************/
typedef struct {
  uint16_t x ;
  uint16_t y ;
} toaDo ;
toaDo nhietDo ;
toaDo gio ;
toaDo doAm ;
toaDo thoiTiet ;
toaDo ngay ;
toaDo gioPhut ;
//String weekDays[7]={"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char * monthName[12] = {" Jan" , " Feb" , " Mar" , " Apr" , " May" , " Jun", " Jul" , " Aug" , " Sep" , " Oct" , " Nov" , " Dec"};
char * TMdate(char day, char mon, uint16_t year);
char* thuTrongTuan ;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  tft.initR(INITR_18BLACKTAB);   //  INITR_BLACKTAB initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  delay(500);
  //Serial.begin(115200);

  WiFi.begin(ssid, password);
  //Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.print("Connected to WiFi network with IP Address: ");
  timeClient.begin(); 
}

void loop() {
  /* VARIABLE            VARIABLE             VARIABLE           VARIABLE            VARIABLE             VARIABLE   */
nhietDo.x = 57 + 15 ;
nhietDo.y = 50+5 ;
doAm.x = 40  ;
doAm.y = 100-12 ;
gio.x = 40 ; 
gio.y = 100+15 ;
thoiTiet.x = 0 ;
thoiTiet.y = 15+5; 

ngay.x = 65+30 ;
ngay.y = 0 ;
gioPhut.x = 40  ;
gioPhut.y =  1  ;

  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      JSONVar myObject = JSON.parse(jsonBuffer);
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        //Serial.println("Parsing input failed!");
        return;
      }
      /* TIME CLIENT */
      timeClient.update();
      time_t epochTime = timeClient.getEpochTime();
      uint8_t ClientHour = timeClient.getHours();
      ClientHour += 7 ;
      if (ClientHour >= 24) {
          ClientHour -= 24 ;
        }
      uint8_t ClienttMinute = timeClient.getMinutes();
      
      /* LED 7 SEG              LED 7 SEG                LED 7 SEG            LED 7 SEG              LED 7 SEG                LED 7 SEG */
      struct tm *ptm = gmtime ((time_t *)&epochTime); 


      int monthDay = ptm->tm_mday;
      //Serial.println(monthDay);

      int currentMonth = ptm->tm_mon+1;
      String currentMonthName = monthName[currentMonth-1];
      //Serial.println(currentMonthName);

      int currentYear = ptm->tm_year+1900;
      //Serial.println(currentYear);

      thuTrongTuan= TMdate(monthDay, currentMonth , currentYear );

      bool isNight = (ClientHour >= 17 && ClientHour <=23 ) || (ClientHour >= 0 && ClientHour <= 6) ;
      delay(500); 
      /* TIME DISPLAY       TIME DISPLAY       TIME DISPLAY      TIME DISPLAY     TIME DISPLAY     TIME DISPLAY     TIME DISPLAY      TIME DISPLAY     TIME DISPLAY  */
      
      tft.fillRoundRect(gioPhut.x , gioPhut.y , 48 , 14, 0, ST7735_BLACK);
      tft.setFont(&FreeSans9pt7b);  // FreeSansBold9pt7b   &FreeSans9pt7b
      dtostrf(ClientHour, 2, 0, buffer);
      testdrawtext(buffer , 0Xffff , 1 , 1+ gioPhut.x , gioPhut.y) ;
      dtostrf(ClienttMinute , 2, 0, buffer);
      testdrawtext(buffer , 0Xffff , 1 , 25+gioPhut.x ,  gioPhut.y) ;
      testdrawtext(":" , 0Xffff , 1 , 21+gioPhut.x ,  gioPhut.y) ;
      
      /* DAY DISPLAY          DAY DISPLAY         DAY DISPLAY           DAY DISPLAY           DAY DISPLAY           DAY DISPLAY            */
      
      dtostrf(monthDay, 2, 0, buffer);
      char * plus = monthName[currentMonth - 1] ;
      sprintf(buffer + strlen(buffer), "%s", plus); // Nối plus vào buffer
      tft.setFont(NULL);
      testdrawtext(thuTrongTuan, 0Xffff , 1 , ngay.x  , ngay.y) ;
      testdrawtext(buffer, 0Xffff , 1 , ngay.x +20, ngay.y) ;
     
/* TEMPARATURE    TEMPARATURE       TEMPARATURE       TEMPARATURE       TEMPARATURE       TEMPARATURE   */
      tft.setFont(&FreeSansBold18pt7b);  //    FreeSans9pt7b
      // Lấy giá trị nhiệt độ từ JSON
      double temperatureKelvin = myObject["main"]["temp"];
      double temperatureCelsius = temperatureKelvin - 273.15;
      dtostrf(temperatureCelsius, 3, 0, buffer); 
      tft.fillRoundRect(nhietDo.x , nhietDo.y -14 , 60+15, 14+13, 0, ST7735_BLACK);
      testdrawtext(buffer , ST7735_WHITE ,1 , nhietDo.x  , nhietDo.y);
      tft.setFont(&FreeSans9pt7b);  // FreeSansBold9pt7b
      testdrawtext("o" , ST7735_WHITE ,1 , nhietDo.x+43+6  , nhietDo.y - 3 -10);
      testdrawtext("C" , ST7735_WHITE ,1 , nhietDo.x+43+15 , nhietDo.y -10);
/*
      tft.setFont(&FreeSans9pt7b);  // FreeSansBold9pt7b
      double Humid = myObject["main"]["humidity"] ;
      dtostrf(Humid, 3, 0, buffer); 
      sprintf(buffer, "%s%%", buffer); // Sử dụng sprintf để nối chuỗi buffer với "%"
      tft.fillRoundRect(doAm.x , doAm.y , 60, 14, 0, ST7735_BLACK);
      testdrawtext(buffer , ST7735_WHITE ,1 , doAm.x , doAm.y );
      tft.drawRGBBitmap(doAm.x - 20 , doAm.y, colorHumid , 16 , 16 );

      double Speed =  myObject["wind"]["speed"]  ;
      double WindSpeed = 3.6 * Speed ;
      dtostrf(WindSpeed, 5, 2, buffer); 
      sprintf(buffer, "%s km/h", buffer); // Sử dụng sprintf để nối chuỗi buffer với "%"
      tft.fillRoundRect(gio.x , gio.y , 60+25, 14, 0, ST7735_BLACK);
      testdrawtext(buffer , ST7735_WHITE ,1 , gio.x , gio.y);
      tft.drawRGBBitmap(gio.x - 20 , gio.y, colorWind , 16 , 16 ); 
*/
      /* DRAW WEATHER   DRAW WEATHER      DRAW WEATHER        DRAW WEATHER      DRAW WEATHER */
      // Lấy thông tin thời tiết từ JSON
      JSONVar weatherData = myObject["weather"][0];
      String weatherMain = JSON.stringify(weatherData["main"]);
      // In thông tin thời tiết ra màn hình
      tft.fillRoundRect( thoiTiet.x , thoiTiet.y , 60, 60 , 0, ST7735_BLACK);
      
      if      (weatherMain == "\"Rain\"")  tft.drawRGBBitmap(thoiTiet.x , thoiTiet.y , colorRain , 64, 64) ;
      else if (weatherMain == "\"Clouds\"" && !isNight ) tft.drawRGBBitmap(thoiTiet.x , thoiTiet.y, colorCloudy , 64, 64) ;
      else if (weatherMain == "\"Clouds\"" && isNight ) tft.drawRGBBitmap(thoiTiet.x , thoiTiet.y, colorCloudNight , 64, 64) ;
      else {
        if (isNight) tft.drawRGBBitmap(thoiTiet.x , thoiTiet.y, colorFineNight , 64, 64) ;
        else tft.drawRGBBitmap(thoiTiet.x , thoiTiet.x , colorFine , 64, 64) ;
      }
      
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return payload;
}

void testdrawtext(char *text, uint16_t color , uint16_t TextSize , uint16_t ver_pixels , uint16_t hor_pixels) {
  tft.setTextSize(TextSize);
  tft.setCursor(ver_pixels , hor_pixels + 12);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

char * TMdate(char day, char mon, uint16_t year) {
    char * Week[7] = {"Sun ", "Mon ", "Tue ", "Wed ", "Thu ", "Fri ", "Sat "};
    struct tm date = {0}; // Khởi tạo tất cả thành phần của date bằng 0
    date.tm_year = year - 1900; // Năm 2023 (trừ đi 1900)
    date.tm_mon = mon - 1;      // Tháng 10 (trừ đi 1)
    date.tm_mday = day;          // Ngày 7
    // Tính thứ trong tuần (0: Chủ Nhật, 1: Thứ Hai, ..., 6: Thứ Bảy)
    std::mktime(&date);
    int dayOfWeek = date.tm_wday;
    return Week[dayOfWeek];
}