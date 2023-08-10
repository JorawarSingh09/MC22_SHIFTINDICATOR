#include <SPI.h>
#include <Wire.h>
#include <FastLED.h>


#include <U8glib.h>
#include <stdio.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define LED_PIN     5
#define NUM_LEDS    30

#define OLED_MOSI   9
#define OLED_CLK   10           
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13

#define ANALOG_IN_PIN A0
 
// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;
 
// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 
 
// Float for Reference Voltage
float ref_voltage = 5.0;
 
// Integer for ADC value
int adc_value = 0;

CRGB leds[NUM_LEDS];
CRGB C1 = CRGB(0, 0, 255);
CRGB C2 = CRGB(0, 0, 0);

U8GLIB_SH1106_128X64 u8g(OLED_CLK, OLED_MOSI, OLED_CS, OLED_DC);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(ANALOG_IN_PIN, INPUT_PULLUP);
  
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB ( 255, 255, 255);
  }
  FastLED.setBrightness(100);
  FastLED.show();
  u8g.setRot180();
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255); // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3); // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1); // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }
  
  for (int i = 0; i <= NUM_LEDS/2; i++) {
    leds[i] = C1;
    leds[i-1] = C2;

    leds[NUM_LEDS-i] = C1;
    leds[NUM_LEDS-i+1] = C2;
    FastLED.show();
    delay(100);
  }

  for (int i = NUM_LEDS/2; i >= 0; i--) {
    if(i == 0){
      leds[i] = C1;
      leds[30] = C2;
      leds[1] = C2;
     }
    else{
      leds[i-1] = C1;
      leds[i] = C2;
  
      leds[NUM_LEDS-i+1] = C1;
      leds[NUM_LEDS-i] = C2;
    }
    FastLED.show();
    delay(100);
  }
  
}

void loop() {

  u8g.firstPage();
  do {      
    // Read the Analog Input
    adc_value = analogRead(ANALOG_IN_PIN);
   
     // Determine voltage at ADC input
    adc_voltage  = (adc_value * ref_voltage) / 1024.0; 
   
    // Calculate voltage at divider input
    in_voltage = adc_voltage / (R2/(R1+R2)) ;
    float vout = (in_voltage - 3) / 24;
//    Serial.println(vout);
    u8g.setFont(u8g_font_unifont);
    //u8g.setFont(u8g_font_osb21);
    char myString[10]; // Create a character array to store the string representation
    for (int i = 0; i <= NUM_LEDS/2; i++) {
      // BLUE
      if(i <= vout * NUM_LEDS/2){
        leds[i] = C1;
        leds[NUM_LEDS - i] = C1;
      }else{
       // RED
        leds[i] = C2;
        leds[NUM_LEDS - i] = C2;
      }
  }
  FastLED.show();
  dtostrf(vout, 6, 2, myString);
  u8g.drawStr(0, 22, myString);
  

  } while ( u8g.nextPage() );
  delay(100);
}
