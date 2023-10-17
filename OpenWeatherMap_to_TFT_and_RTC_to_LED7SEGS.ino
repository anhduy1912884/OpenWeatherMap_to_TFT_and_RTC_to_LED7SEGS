/* Portrait Display */
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include "weather_image.h"
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Wire.h>
#include <DS1307.h>
#include <TM1637Display.h>
#include <ctime>
DS1307 rtc;
TM1637Display display(D1, D2);

#include <iostream>
#include <string>

const char* ssid = "Gggggg";
const char* password = "25862586";

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "f9980632671d4621dd517cddae9a7086";
String city = "Di An";
String countryCode = "VN";
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
char buffer[10]; 
String jsonBuffer;
/***********************************************************END WEB **************************************/ 
// ST7735 TFT module connections
#define TFT_RST   D4     // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    D3     // TFT CS  pin is connected to NodeMCU pin D4 (GPIO0)
#define TFT_DC    D8     // TFT DC  pin is connected to NodeMCU pin D4 (GPIO4)
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
char * monthName[12] = {" Jan" , " Feb" , " Mar" , " Apr" , " May" , " Jun", " Jul" , " Aug" , " Sep" , " Oct" , " Nov" , " Dec"};
char * TMdate(char day, char mon, uint16_t year);
char* thuTrongTuan ;

void setup() {
  tft.initR(INITR_18BLACKTAB);   //  INITR_BLACKTAB initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  delay(1000);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
 
  rtc.begin();
  display.setBrightness(7); 
 // rtc.set(00, 2 , 20 , 10, 10, 2023);
  rtc.stop();
  rtc.start();
  uint8_t sec, min, hour, day, month;
  uint16_t year;
  rtc.get(&sec, &min, &hour, &day, &month, &year);
  thuTrongTuan= TMdate(day, month , year);
}

void loop() {
  /* VARIABLE            VARIABLE             VARIABLE           VARIABLE            VARIABLE             VARIABLE   */
nhietDo.x = 57 ;
nhietDo.y = 50+15 ;
doAm.x = 30  ;
doAm.y = 100 ;
gio.x = 30 ; 
gio.y = 130 ;
thoiTiet.x = 1 ;
thoiTiet.y = 15+15; 
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      JSONVar myObject = JSON.parse(jsonBuffer);
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        return;
      }
      /* LED 7 SEG              LED 7 SEG                LED 7 SEG            LED 7 SEG              LED 7 SEG                LED 7 SEG */
      uint8_t sec, min, hour, day, month;
      uint16_t year;
      rtc.get(&sec, &min, &hour, &day, &month, &year);
      bool isNight = (hour >= 17 && hour <=23 ) || (hour >= 0 && hour <= 6) ;
      /* TIME DISPLAY       TIME DISPLAY       TIME DISPLAY      TIME DISPLAY     TIME DISPLAY     TIME DISPLAY     TIME DISPLAY      TIME DISPLAY     TIME DISPLAY  */
      tft.fillRoundRect(10+1+15 , 1 , 60 , 14, 0, ST7735_BLACK);
      tft.setFont(&FreeSans9pt7b);  // FreeSansBold9pt7b   &FreeSans9pt7b
      dtostrf(hour, 2, 0, buffer);
      testdrawtext(buffer , 0Xffff , 1 , 10+1+18 , 1) ;
      dtostrf(min , 2, 0, buffer);
      testdrawtext(buffer , 0Xffff , 1 , 10+25+18 , 1) ;
      testdrawtext(":" , 0Xffff , 1 , 10+21+18 , 1) ;
      /* DAY DISPLAY          DAY DISPLAY         DAY DISPLAY           DAY DISPLAY           DAY DISPLAY           DAY DISPLAY            */
      dtostrf(day, 2, 0, buffer);
      char * plus = monthName[month - 1] ;
      sprintf(buffer + strlen(buffer), "%s", plus); // Nối plus vào buffer
      tft.setFont(NULL);
      testdrawtext(thuTrongTuan, 0Xffff , 1 , 65  , 10) ;
      testdrawtext(buffer, 0Xffff , 1 , 65 +20, 10) ;
     
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
    payload = http.getString();
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