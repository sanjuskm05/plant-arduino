// Venancio da Silva 2021
// Plant monitoring - arduino pro micro analog sensing
// Handle the I2C communication to the ESP32

// Include the required Wire library for I2C and rgb led
#include <Wire.h>
#include <FastLED.h>

#define light_pin    A0
#define moisture_pin A1

int  moisture_val;
byte moisture_val_b;
int  light_val;
byte light_val_b;

#define low_res  10
#define med_res  50
#define high_res 100

#define DATA_PIN    6
#define NUM_LEDS    40
#define BRIGHTNESS  125
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

int clock_frequency = 100000;
int i2c_add = 11; //using hard address for now

int light_on = 0; //global control for turning on the light
  
void requestEvent(){
  Wire.write(light_val_b);
  Wire.write(moisture_val_b);
}
void receiveEvent(int len){
  light_on = Wire.read();
}

void setup() {
  Wire.setClock(clock_frequency); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  // Attach a function to trigger when something is requested.
  Wire.onRequest(requestEvent);
  // Start the I2C Bus as Slave on address i2c_add
  Wire.begin(i2c_add);

  //initialize the led
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Setup Serial Monitor 
  Serial.begin(115200);
  
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(15);
  for(int i=1; i<40; i++){
    leds[i] = CRGB::Red;
    leds[i-1] = CRGB::Black;
    FastLED.show();
    delay(15);
  }
  leds[NUM_LEDS-1] = CRGB::Black;
  FastLED.show();
  delay(15);

  Serial.println("\nInitialization complete.\n");
}

void loop() {
  light_val = analogRead(light_pin);
  light_val_b = map(light_val, 0, 1023, 255, 0);
  
  moisture_val = analogRead(moisture_pin);
  moisture_val_b = map(moisture_val, 0, 1023, 0, 255);

  if(light_on){
    for(int i=0; i<40; i++){
      leds[i] = CRGB::White;
    }
    FastLED.show();
  }
  else {
    for(int i=0; i<40; i++){
      leds[i] = CRGB::Black;
    }
    FastLED.show();
    
  }
  
  delay(100);

}
