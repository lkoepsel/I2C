// I2C_nInterfaces - Demo using n Wire interfaces
// Uses Wire and 2 Wire1 interfaces
// Uses Adafruit OLED and 2 Sparkfun Alphanumeric displays
// Setup to OL_display 3 lines of 12-14 characters per line
// For Pico, and uses both Wire and Wire1 interfaces
// Or change appropriately using the Wire commands below
// Uses Arduino String class for text 

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <SparkFun_Alphanumeric_Display.h>
//temp sensor
#include <Adafruit_MLX90614.h>

// Temp Sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Adafruit FeatherWing OLED 128x64 - 4650
// https://learn.adafruit.com/adafruit-128x64-oled-featherwing
Adafruit_SH1107 OL_display = Adafruit_SH1107(64, 128, &Wire);

//SparkFun Qwiic Alphanumeric Display - Green COM-18566
// https://learn.sparkfun.com/tutorials/sparkfun-qwiic-alphanumeric-display-hookup-guide
HT16K33 AN_display_10;
HT16K33 AN_display_11;

String line_1 = "first";
String line_2 = "second 78901";
String line_3 = "third";
unsigned int start_delay = 2;       // 2 slow blinks to indicate successful boot
unsigned int start_blinks = 1;      
unsigned int error_delay = 2;       // 32 fast blinks to indicate error
unsigned int error_blinks = 16;     // in finding either display
unsigned int success_delay = 2;     // 4 medium blinks to indicate success
unsigned int success_blinks = 2;    // in running
float IR_temperature = 0;           // IR temperature from MLX sensor
String sNumber = "";                // string version of number for AN display

// Adafruit - I2C interface and addresss
unsigned int SDA_00 = 8;
unsigned int SCL_00 = 9;
byte address_00 = 0x3C;

// Temp - I2C interface and addresss
unsigned int SDA_01 = 20;
unsigned int SCL_01 = 21;
byte address_01 = 0x5A;

// Sparkfun - 1st I2C interface and addresss
unsigned int SDA_10 = 18;
unsigned int SCL_10 = 19;
byte address_10 = 0x70;

// Sparkfun - 2nd I2C interface and addresss
unsigned int SDA_11 = 14;
unsigned int SCL_11 = 15;
byte address_11 = 0x70;

// display: show values on Alphanumeric display
void displayAN();

// blink: delay for n seconds, blinking n/2 times per second
void blink(unsigned int seconds, unsigned int times);

// init_Wire: setup wire interface, previous Wire interface must have end()
void init_Wire(unsigned int SDA, unsigned int SCL, TwoWire &Wire);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    blink(start_delay, start_blinks);

    // init Adafruit I2C interface
    init_Wire(SDA_00, SCL_00, Wire);

    // init Sparkfun 1st I2C interface
    init_Wire(SDA_10, SCL_10, Wire1);

    if (!OL_display.begin(address_00, true))
    {
        blink(error_delay, error_blinks);
    }
    else if (AN_display_10.begin(address_10,
                                DEFAULT_NOTHING_ATTACHED,
                                DEFAULT_NOTHING_ATTACHED,
                                DEFAULT_NOTHING_ATTACHED,
                                 Wire1) == false)
    {
      blink(error_delay, error_blinks);
    }
    // setup Adafruit parameters
    OL_display.setRotation(3);
    OL_display.setTextSize(1);
    OL_display.setTextColor(SH110X_WHITE);

    // Three steps to Adafruit, all must be done
    // 1. Clear buffer and diplay
    OL_display.clearDisplay();
    OL_display.display();

    // 2. Set start location and font
    OL_display.setCursor(0,10);
    OL_display.setFont(&FreeSans9pt7b);

    // 3. Build display buffer then display
    OL_display.println(line_1);
    OL_display.println(line_2);
    OL_display.println(line_3);

    // Finally, display lines above
    OL_display.display(); 

    // simple single line for Sparkfun
    AN_display_10.print("TEMP");
    Wire.end();
    Wire1.end();

    // Init and display on second Wire and Wire1 interfaces
    // start IR temp sensor 
    // init Temp I2C interface
    // Wire.setSDA(SDA_01);
    // Wire.setSCL(SCL_01);
    // Wire.begin();
    init_Wire(SDA_01, SCL_01, Wire);
    mlx.begin();

    // init Sparkfun 1st I2C interface
    init_Wire(SDA_11, SCL_11, Wire1);
    // Wire1.setSDA(SDA_11);
    // Wire1.setSCL(SCL_11);
    // Wire1.begin();

    if (AN_display_11.begin(address_11,
                            DEFAULT_NOTHING_ATTACHED,
                            DEFAULT_NOTHING_ATTACHED,
                            DEFAULT_NOTHING_ATTACHED,
                             Wire1) == false)
    {
      blink(error_delay, error_blinks);
    }

    // finish with success blink
    blink(success_delay, success_blinks);
}

void loop() 
{
    IR_temperature = mlx.readObjectTempF();
    displayAN();
    delay(1000);
}

// displayAN: right align number based on value, X 100 to remove decimal point
void displayAN()
{
    sNumber = String(IR_temperature,1);
    if (IR_temperature < 10.0)
    {
        sNumber = "  " + sNumber;
    }
    else if (IR_temperature < 100.0)
    {
        sNumber = " " + sNumber;
    }
    AN_display_11.print(sNumber);
}

void blink(unsigned int seconds, unsigned int times)
{
    for (unsigned int i = 0; i < seconds; i++) 
    {
        for (unsigned int j = 0; j < times; j++) 
        {    
            digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(1000/times/2); // wait for seconds/2, blinking n * 2 times
            digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
            delay(1000/times/2); // wait for seconds/2, blinking n * 2 times
        }
    }
}

void init_Wire(unsigned int SDA, unsigned int SCL, TwoWire &Wire)
{
    Wire.setSDA(SDA);
    Wire.setSCL(SCL);
    Wire.begin();

}

