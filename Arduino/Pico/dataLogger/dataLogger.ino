// dataLogger_wAN_display: dataLogger w Alphanumberic display
// 10 second wait allows user to load new program onto Uno to read data
// AN Display will show STRT for startup delay or value while capturing data

// Add the funtions which control the EEPROM storage, I2C and AN Display
#include <EEPROM.h>
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>

//SparkFun Qwiic Alphanumeric Display - Green COM-18566
// https://learn.sparkfun.com/tutorials/sparkfun-qwiic-alphanumeric-display-hookup-guide
// Connect via QWIC connector to pin cable
// SDA to pin SDA (above AREF)
// SCLK to pin SCL (above SDA)
// 5V to 5V 
// GND to GND
HT16K33 AN_display_1;

// blink LED for status messages, blocking delay in seconds, 2n times for blinks
#define max_messages 5
#define red 25
#define blue 12
#define green 13
#define yellow 14
#define white 15
enum message {start, error, step1, step_2, step3};
struct blink
{
  unsigned int pin;     // GP of pin to blink
  unsigned int delay;   // seconds for message (also a delay)
  unsigned int times;   // number of blinks to indicate message
} ;
struct blink messages[max_messages];

// define all I2C devices with a name
// Use I2C_Scanner to confirm/get information on I2C devices
#define max_I2C 1
enum device {AN_1};
struct I2C_device 
{
  unsigned int SDA; // the GP for SDA
  unsigned int SCL; // the GP for SCL
  byte address;     // device address
} ;
struct I2C_device devices[max_I2C];

// AN display parms 
String sNumber = "";                // string version of number for AN display

// Alphanumeric display: right justify float values
void displayAN(float value, HT16K33 AN);

// init device used to make setup of I2C interface easier
// use same index for init_d and init_Wire
void init_d(unsigned int index, unsigned int SDA,
             unsigned int SCL, byte address);

// initialize the messages array
void init_m(unsigned int index, unsigned int delay, unsigned int times);

// init_Wire: setup wire interface for each time it is used
// previous Wire interface setup must be closed wire.end()
// Only call init_Wire once each time to setup, 
// even if there are >1 devices on the I2C interface
void init_Wire(unsigned int index, TwoWire &Wire);

/// global variables used in program
unsigned int address = 0;           // beginning address at which to store data
unsigned int ADCpin = A0;           // set the input pin (ADC) to be read
unsigned int ADCvalue = 0;          // variable to store the ADC value
float voltage = 0;                  // voltage conversion from ADC
const float conversion = 3.0/1024;  // conversion constant for ADC to voltage

// convert: convert ADC data to voltage
void convert();

// read: get the time and the ADC value
void read();

// status: delay for n seconds, blinking 2n times per second
void status(unsigned int msg);

// setup function for initializing uC, req'd by Arduino
// initialize serial/display, print a header for the data, delay start_time
void setup() 
{
    status(start);

    init_d(AN_1, 2, 3, 0x70);     // Alphanumeric display 2 on Wire

    init_m(start, blue, 2, 1);    // 2 sec delay w2 slow blinks for boot start
    init_m(error, red, 2, 16);   // 2 sec delay w32 fast blinks for I2C error
    init_m(step1, green, 1, 2);  // 2 sec delay w4 medium blinks for I2C success
    init_m(step_2, yellow, 1, 2);     // 1 sec delay w1 slow blink for I2C next
    init_m(step3, white, 1, 2);     // 1 sec delay w1 slow blink for I2C next


    // init displays and show BOOT message
    init_Wire(AN_1, Wire1);
    if (AN_display_1.begin(devices[AN_1].address,
                            DEFAULT_NOTHING_ATTACHED,
                            DEFAULT_NOTHING_ATTACHED,
                            DEFAULT_NOTHING_ATTACHED,
                             Wire1) == false)
    {
      status(error);
    }
    AN_display_1.print("BOOT");
    status(step1);
}

// infinite loop for executing code without stopping, req'd by Arduino
void loop() 
{
    status(step_2);
    read();
    convert();
    displayAN(voltage, AN_display_1);
    status(step3);
}

void convert()
{
    voltage = ADCvalue * conversion;
}

void read()
{    // read the value from the ADC pin and get the elapsed time:
    ADCvalue = analogRead(ADCpin);
}

void displayAN(float voltage,HT16K33 AN)
{
    sNumber = String(voltage*100,0);
    if (voltage < 1.0)
    {
        sNumber = "  " + sNumber;
    }
    else if (voltage < 10.0)
    {
        sNumber = " " + sNumber;
    }
    AN.print(sNumber);
}

void init_d(unsigned int index, unsigned int SDA,
             unsigned int SCL, byte address)
{
    devices[index].SDA = SDA;
    devices[index].SCL = SCL;
    devices[index].address = address;
}

void init_m(unsigned int index, unsigned int pin, unsigned int delay, unsigned int times)
{
    messages[index].pin = pin;
    messages[index].delay = delay;
    messages[index].times = times;
    pinMode(pin, OUTPUT);
}

void init_Wire(unsigned int index, TwoWire &Wire)
{
    Wire.setSDA(devices[index].SDA);
    Wire.setSCL(devices[index].SCL);
    Wire.begin();

}

void status(unsigned int msg)
{
    for (unsigned int i = 0; i < messages[msg].delay; i++) 
    {
        for (unsigned int j = 0; j < messages[msg].times; j++) 
        {    
            digitalWrite(messages[msg].pin, HIGH);   // turn the LED on 
            delay(1000/messages[msg].times/2); // delay seconds/2, blink n * 2 
            digitalWrite(messages[msg].pin, LOW);    // turn the LED off 
            delay(1000/messages[msg].times/2); // delay seconds/2, blink n * 2 
        }
    }
}
