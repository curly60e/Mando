/*
Mando
https://github.com/curly60e/Mando
*/

#include <Watchy.h> //include the Watchy library
#include "resource.h"
#include "settings.h"

class WatchFace : public Watchy { //inherit and extend Watchy class
  using Watchy::Watchy;
  public:


    bool VibeMode = false;
    //(function below toggles the motor on or off)
    void VibeTo(bool Mode){
        if (Mode != VibeMode){
            if (Mode){
                sensor.enableFeature(BMA423_WAKEUP, false);
                pinMode(VIB_MOTOR_PIN, OUTPUT);
                digitalWrite(VIB_MOTOR_PIN, true);
            }else{
                digitalWrite(VIB_MOTOR_PIN, false);
                sensor.enableFeature(BMA423_WAKEUP, true);
            }
            VibeMode = Mode;
        }
    }
    
    void drawWatchFace() { //override this method to customize how the watch face looks
      
      int16_t  x1, y1, lasty;
      uint16_t w, h;
      String textstring;
      String textstringMinute;
      bool light = true; // left this here if someone wanted to tweak for dark
      //resets step counter at midnight everyday
      if(currentTime.Hour == 00 && currentTime.Minute == 00) {
        sensor.resetStepCounter();
      }
      
      //pinMode(MENU_BTN_PIN, INPUT);
      pinMode(BACK_BTN_PIN, INPUT);
      //pinMode(UP_BTN_PIN, INPUT);
      //pinMode(DOWN_BTN_PIN, INPUT);
   
      //if (digitalRead(MENU_BTN_PIN) == 1) {
      //  VibeTo(true);
      //  delay(100);
      //  VibeTo(false);
      //} 
      if (digitalRead(BACK_BTN_PIN) == 1) {
        VibeTo(true);
        delay(50);
        VibeTo(false);
      } 
      //if (digitalRead(UP_BTN_PIN) == 1) {
      //  VibeTo(true);
      //  delay(100);
      //  VibeTo(false);
      //} 
      //if (digitalRead(DOWN_BTN_PIN) == 1) {
      //  VibeTo(true);
      //  delay(100);
      //  VibeTo(false);
      //}
        
      // ** DRAW **
      
      //drawbg
      display.fillScreen(light ? GxEPD_WHITE : GxEPD_BLACK);
      display.fillRoundRect(2,2,196,196,8,light ? GxEPD_BLACK : GxEPD_WHITE);
      display.fillRoundRect(6,6,188,188,5,light ? GxEPD_WHITE : GxEPD_BLACK);
      
      display.setFont(&PebblecoNumbers_MONO32pt7b);
      display.setTextColor(light ? GxEPD_BLACK : GxEPD_WHITE);
      display.setTextWrap(false);

      //draw time
      textstring = currentTime.Hour;
      textstring += ":";
      if (currentTime.Minute < 10) {
        textstring += "0";
      } else {
        textstring += "";
      }
      textstring += currentTime.Minute;
      display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
      display.setCursor(96-w/2,88);
      display.print(textstring);
      
      //vibrate per o'clock
      if (currentTime.Minute < 10) {
        textstringMinute = "0";
      } else {
        textstringMinute = "";
      }
      textstringMinute += currentTime.Minute;
      display.getTextBounds(textstringMinute, 0, 0, &x1, &y1, &w, &h);
      if (textstringMinute == "00") {
        VibeTo(true);
        delay(100);
        VibeTo(false);
      }

      // draw battery
      display.fillRoundRect(16,16,34,12,4,light ? GxEPD_BLACK : GxEPD_WHITE);
      display.fillRoundRect(49,20,3,4,2,light ? GxEPD_BLACK : GxEPD_WHITE);
      display.fillRoundRect(18,18,30,8,3,light ? GxEPD_WHITE : GxEPD_BLACK);
      float batt = (getBatteryVoltage()-3.3)/0.9;
      if (batt > 0) {
       display.fillRoundRect(20,20,26*batt,4,2,light ? GxEPD_BLACK : GxEPD_WHITE);
      }                 

      //draw steps
      display.setFont(&LECO_2014_Regular10pt7b);
      lasty = 200 - 16;
      
      textstring = sensor.getCounter();
      display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
      display.fillRoundRect(16,lasty-h-2,w + 8,20,2,light ? GxEPD_BLACK : GxEPD_WHITE);
      display.fillRoundRect(151,16,33,16,2,light ? GxEPD_BLACK : GxEPD_WHITE);
      display.setCursor(19, 184);
      display.setTextColor(light ? GxEPD_WHITE : GxEPD_BLACK);
      display.print(textstring);
      display.setTextColor(light ? GxEPD_BLACK : GxEPD_WHITE);
      lasty += -8-h;

      // draw year
      textstring = currentTime.Year + 1970;
      display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
      display.setCursor(16, lasty);
      display.print(textstring);
      lasty += -20;

      // draw date
      textstring = monthShortStr(currentTime.Month);
      textstring += " ";
      textstring += currentTime.Day;
      display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
      display.setCursor(16, lasty);
      display.print(textstring);
      lasty += -20;
      
      // draw day
      textstring = dayStr(currentTime.Wday);
      display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
      display.setCursor(16, lasty);
      display.print(textstring);
      lasty += -40;
      
      //draw image
      display.drawBitmap(115,105, epd_bitmap_MANDO, 70, 82, GxEPD_BLACK);
      display.drawBitmap(86,8, epd_bitmap_bitcoin_PNG38, 30, 30, GxEPD_BLACK);

      //draw temperature  
      weatherData currentWeather = getWeatherData();
  
      int8_t temperature = currentWeather.temperature;
      int16_t weatherConditionCode = currentWeather.weatherConditionCode;
      display.setTextColor(light ? GxEPD_WHITE : GxEPD_BLACK);
      display.setFont(&LECO_2014_Regular7pt7b);
      display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
      if(159 - w - x1 > 87){
          display.setCursor(155, 28);
          //lasty += -70;
      }else{
          display.setFont(&LECO_2014_Regular7pt7b);
          display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
          display.setCursor(155, 28);
          //lasty += -70;
      }
      textstring = "C";
      display.println(temperature);
      display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
      display.setCursor(172, 28);
      display.print(textstring);
      display.setTextColor(light ? GxEPD_BLACK : GxEPD_WHITE);
      

    }
};


WatchFace m(settings); //instantiate your watchface

void setup() {
  m.init(); //call init in setup
}

void loop() {
  // this should never run, Watchy deep sleeps after init();
}
