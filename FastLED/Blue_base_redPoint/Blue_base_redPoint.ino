  #include <FastLED.h>
  #define NUM_LEDS 25
  #define DATA_PIN 4
  #define UART_RX_PIN 2 // GPIO2

  CRGB leds[NUM_LEDS];
  const float alpha = 0.1;    // the filter coefficient
  float filteredValue = 0;    // the filtered input value

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1,UART_RX_PIN);  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.write(Serial2.read());

  int val = Serial2.read();
  filteredValue = alpha * val + (1 - alpha) * filteredValue; //filter for stablise input data
  int numLedsToLight = map(val, -180, 180, 2, NUM_LEDS-3); //maping input with LEDs

  // First, clear the existing led values
  FastLED.clear();
  for(int led = 2; led < NUM_LEDS/2; led++) {
    if(numLedsToLight == led || numLedsToLight+1 == led || numLedsToLight-1 == led){
      continue;
    }
    else{
      leds[led] = CRGB::Blue; 
      leds[led].fadeLightBy(led*15);
    }
  }
  for(int led = NUM_LEDS/2 ; led < NUM_LEDS-3; led++) {
    if(numLedsToLight == led || numLedsToLight+1 == led || numLedsToLight-1 == led){
      continue;
    }
    else{
      leds[led] = CRGB::Red; 
      leds[led].fadeLightBy(250-(led-NUM_LEDS/2)*15);
    }
  }
  leds[numLedsToLight] = CRGB::White;
  leds[numLedsToLight+1] = CRGB::White; 
  leds[numLedsToLight+1].fadeLightBy(240);
  leds[numLedsToLight-1] = CRGB::White;
  leds[numLedsToLight-1].fadeLightBy(240);  
  FastLED.show();
  delay(60);
}