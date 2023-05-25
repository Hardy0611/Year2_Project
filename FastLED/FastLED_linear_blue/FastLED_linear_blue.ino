  #include <FastLED.h>
  #define NUM_LEDS 35
  #define DATA_PIN 4
  CRGB leds[NUM_LEDS];
  const float alpha = 0.1;    // the filter coefficient
  float filteredValue = 0;    // the filtered input value

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(2);
  filteredValue = alpha * val + (1 - alpha) * filteredValue;
  int numLedsToLight = map(val, 0, 2000, 0, NUM_LEDS);

  // First, clear the existing led values
  FastLED.clear();
  for(int led = 0; led < numLedsToLight; led++) { 
    leds[led] = CRGB::Blue; 
  }
  FastLED.show();
}