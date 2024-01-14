#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

void print_lcd(float R, float M, float t);

#define OLED_ADDR   0x3C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void setup(){

  // Initialize UART communication
  Serial.begin(9600);

  // Initialize I2C communication
  Wire.begin(); 

  // Initialize the display
  u8g2.begin();
    
  // Set text size and color
  u8g2.setFont(u8g2_font_7x14_mf);
  u8g2.setColorIndex(1);
  print_lcd(0,0,0);
}


void loop(){
    if (Serial.available()){
      float input_values[30];
      for (int i = 0; i < 30; i++) {
          // Read 4 bytes (size of float)
          char buffer[4];
          Serial.readBytes(buffer, sizeof(float));

          // Convert the received bytes to a float
          float value;
          memcpy(&value, buffer, sizeof(float));
          input_values[i]=value;
      }
      Serial.println("OK");
      Serial.println("Revcieved : ");
      for (int i = 0; i < 30; i++) {
        char to_print[10];
        sprintf(to_print,"%f",input_values[i]);
        Serial.println(to_print);
      }
    }
}


void print_lcd(float R, float M,float t){

  char to_print[50];
  u8g2.clearBuffer();
  u8g2.setCursor(0, 10);
  u8g2.print("Param estimator");
  u8g2.setCursor(0, 32-6);
  sprintf(to_print,"R_l : %f",R);
  u8g2.print(to_print);
  u8g2.setCursor(0, 48-6);
  sprintf(to_print,"M   : %f",M);
  u8g2.print(to_print);
  u8g2.setCursor(0, 64-6);
  sprintf(to_print,"Took %fms",t*1000);
  u8g2.print(to_print);
  u8g2.sendBuffer();
}