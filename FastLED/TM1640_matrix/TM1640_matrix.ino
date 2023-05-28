/*
  Matrix library example for TM1640. Kept small to fit in the limited space of an ATtiny44.
  NOTE: compile using LTO enabled!

  Library based on TM1638 library by Ricardo Batista, adapted by Maxint-RD MMOLE 2018.
  Confirmed to work in the following environments:
    * ATtiny44A using Arduino IDE 1.8.2 and ATTinyCore (8MHz, LTO enabled), 3232 bytes flash, 103 bytes RAM ==> 3086/137 ==> 2996/141 ==> 3000/115
    * ATtiny44A using Arduino IDE 1.8.2 and ATtiny Microcontrolers (8MHz), 3212 bytes flash, 103 bytes RAM  == 3056/137 ==> 2976/115
    * WeMos D1-mini and TM1640 8x8 MatrixLED Shield using Arduino IDE 1.6.10: DIN=D7/13/MOSI, CLK=D5/14/SCK, 248644 bytes flash, 32312 bytes RAM

  Only compiled: not tested yet:
    * Arduino Nano using Arduino IDE 1.8.2, Nano (Old Bootloader)), 3388 bytes flash, 119 bytes RAM => 3298/115

  For more information see  https://github.com/maxint-rd/TM16xx
*/
#include <TM1640.h>
#include <TM16xxMatrix.h>
#include <Arduino.h>

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN 4
#endif

// Define a 4-digit display module. Pin suggestions:
// ESP8266 (Wemos D1): data pin 5 (D1), clock pin 4 (D2)
// ATtiny44A: data pin 9, clock pin 10 (LED_BUILTIN: 8 in ATtiny Core)
//TM1640 module(9, 10, 4);    // data, clock, 4 digits
TM1640 module(21, 22);    // For ESP8266/WeMos D1-mini: DIN=D7/21/MOSI, CLK=D5/22/SCK
#define MATRIX_NUMCOLUMNS 16
#define MATRIX_NUMROWS 8
TM16xxMatrix matrix(&module, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows

// Define Uart rx pin
#define UART_RX_PIN 16 // GPIO16
#define UART_TX_PIN 17 // GPIO16

//variables 
float angle = 0; // init angle = 0
float temp = 0; // init temperature as 0
int intensity = 0; //init intensity = 0
boolean mode = false;
const int buttonPin_bottom = 35;  // the pin that the button is connected to 2
const int buttonPin_left = 32;  // the pin that the button is connected to 3
const int buttonPin_right = 34;  // the pin that the button is connected to 4


const bool numbers[11][3][8] = {
  // Number 0
  {
    {true, true, true, true, true, true, true, true},
    {true, false, false, false, false, false, false, true},
    {true, true, true, true, true, true, true, true}
  },
  // Number 1
  {
    {true, false, false, false, false, false, false, false},
    {true, true, true, true, true, true, true, true},
    {true, false, false, false, false, false, false, false},
  },
  // Number 2
  {
    {true, true, true, true, true, false, false, true},
    {true, false, false, false, true, false, false, true},
    {true, false, false, false, true, true, true, true}
  },
  // Number 3
  {
    {true, false, false, false, true, false, false, true},
    {true, false, false, false, true, false, false, true},
    {true, true, true, true, true, true, true, true}
  },
  // Number 4
  {
    {false, false, false, false, true, true, true, true},
    {false, false, false, false, true, false, false, false},
    {true, true, true, true, true, true, true, true}
  },
  // Number 5
  {
    {true, false, false, false, true, true, true, true},
    {true, false, false, false, true, false, false, true},
    {true, true, true, true, true, false, false, true}
  },
  // Number 6
  {
    {true, true, true, true, true, true, true, true},
    {true, false, false, false, true, false, false, true},
    {true, true, true, true, true, false, false, true}
  },
  // Number 7
  {
    {false, false, false, false, false, false, false, true},
    {false, false, false, false, false, false, false, true},
    {true, true, true, true, true, true, true, true}
  },
  // Number 8
  {
    {true, true, true, true, true, true, true, true},
    {true, false, false, false, true, false, false, true},
    {true, true, true, true, true, true, true, true}
  },
  // Number 9
  {
    {true, false, false, false, true, true, true, true},
    {true, false, false, false, true, false, false, true},
    {true, true, true, true, true, true, true, true}
  },
  // Unit c
  {
    {false,true, true, true, false, false, false, false},
    {true, false, false, false, true, false, false, false},
    {true, false, false, false, true, false, false, false},
  }
};


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  
  // Serial connects to the computer
  Serial.begin(115200);
  // Serial2 is the hardware UART port that connects to external circuitry
  Serial2.begin(115200, SERIAL_8N1,UART_RX_PIN);

  //set button mode
  pinMode(buttonPin_bottom, INPUT); // sets the pushbutton pin as input
  pinMode(buttonPin_left, INPUT); // sets the pushbutton pin as input
  pinMode(buttonPin_right, INPUT); // sets the pushbutton pin as input
}

// Self def functions

void update_temp(float input){ // get the temperature from norb. 1.Change degree to temp 2.ignore overflow
  input = constrain(input, -3.14, 3.14)*100;
  temp = map(input, -314, 314, 330, 180)/10.0; // map the temp from 18 to 30 celsius
}

void display_temp(float input_value){ 
  // seperate the digits 
  int whole_part = int(input_value); // Extract the integer part of the input value
  int tens_digit = (whole_part / 10) % 10; // Extract the tens digit
  int ones_digit = whole_part % 10; // Extract the ones digit
  int tenths_digit = int(float(input_value - whole_part) * 10); // Extract the tenths digit
  // end seperation

  displaynumber(tens_digit, 1);
  displaynumber(ones_digit, 2);
  matrix.setPixel(8,0, true);
  displaynumber(tenths_digit, 3);
  displaynumber(10, 4); //unit with celsius
  //draw a small rect
  matrix.setPixel(2,5, true);
  matrix.setPixel(2,6, true);
  matrix.setPixel(3,5, true);
  matrix.setPixel(3,6, true);    
}

void displaynumber(int number, int index){
  int col = 0;
  int descol = 0;
  col = 15 - ((index - 1)*4);
  if (index > 3)
  {
    col--;
  }
  descol = (col-3);
  int x = 0;
  for(int i=col; i>descol; i--)
  {
    for(int j=0; j<MATRIX_NUMROWS; j++)
    {
        matrix.setPixel(i,j, numbers[number][x][j]);
    }
    x++;
  }
}

void update_intensity(float input){
  input = constrain(input, -3.14, 3.14)*100;
  intensity = map(input, -314, 314, 100, 0); // map the temp from 0 to 100 percent
}

void display_intensity(int input_value){
  // seperate the digits 
  int hundreds_digit = (input_value / 100) % 10; // Extract the hundreds digit
  int tens_digit = (input_value / 10) % 10; // Extract the tens digit
  int ones_digit = input_value % 10; // Extract the ones digit
  // end seperation
  displaynumber(hundreds_digit, 1);
  displaynumber(tens_digit, 2);
  displaynumber(ones_digit, 3);
  // draw percentage symbol
  matrix.setPixel(2,5, true);
  matrix.setPixel(2,6, true);
  matrix.setPixel(3,5, true);
  matrix.setPixel(3,6, true);

  matrix.setPixel(0,5, true);
  matrix.setPixel(1,4, true);
  matrix.setPixel(2,3, true);
  matrix.setPixel(3,2, true);
  
  matrix.setPixel(0,1, true);
  matrix.setPixel(0,2, true);
  matrix.setPixel(1,1, true);
  matrix.setPixel(1,2, true);  
}


void loop()
{
  // write norb to angle 
  angle = Serial2.parseFloat();

  //read button state
  int buttonState_bottom = digitalRead(buttonPin_bottom); // read the state of the pushbutton
  int buttonState_left = digitalRead(buttonPin_left); // read the state of the pushbutton
  int buttonState_right = digitalRead(buttonPin_right); // read the state of the pushbutton
  
  if(buttonState_bottom == HIGH || buttonState_left == HIGH || buttonState_right == HIGH){
    mode =! mode;
  }
  
  if(mode){
    update_temp(angle);
    display_temp(temp);
  }
  else{
    update_intensity(angle);
    display_intensity(intensity);
  }
  delay(1000);
}
