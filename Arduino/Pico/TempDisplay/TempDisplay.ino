// TempDisplay - Demo using a struct interface to simplify I2C on Pico
// Uses 2 Wire and 2 Wire1 interfaces
// Uses 2 temp sensors and 2 Sparkfun Alphanumeric displays
// Temp and display could be combined to one Wire interface,
// allowing up to 10 pairs of temp/displays (via multiplexing)

#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>
#include <Adafruit_MLX90614.h>

// 2 Temp Sensors
Adafruit_MLX90614 mlx_1 = Adafruit_MLX90614();
Adafruit_MLX90614 mlx_2 = Adafruit_MLX90614();

// 2 SparkFun Qwiic Alphanumeric Displays - Green COM-18566
// https://learn.sparkfun.com/tutorials/sparkfun-qwiic-alphanumeric-display-hookup-guide
HT16K33 AN_display_1;
HT16K33 AN_display_2;

// blink LED for status messages, blocking delay in seconds, 2n times for blinks
#define max_messages 4
enum message {start, error, success, next};
struct blink
{
  unsigned int delay;   // seconds for message (also a delay)
  unsigned int times;   // number of blinks to indicate message
} ;
struct blink messages[max_messages];

// define all I2C devices with a name
// Use I2C_Scanner to confirm/get information on I2C devices
#define max_I2C 4
enum device {temp_1, temp_2, AN_1, AN_2};
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

// show temp 1
void showTemp_1();

// show temp 2
void showTemp_2();

// status: delay for n seconds, blinking 2n times per second
void status(unsigned int msg);

void setup() {
    init_d(temp_1, 20, 21, 0x5A);   // IR temp sensor on Wire
    init_d(AN_2, 18, 19, 0x70);     // Alphanumeric display 1 on Wire1
    init_d(temp_2, 16, 17, 0x5A);   // IR temp sensor on Wire
    init_d(AN_1, 20, 21, 0x70);     // Alphanumeric display 2 on Wire

    init_m(start, 2, 1);    // 2 sec delay w2 slow blinks for boot start
    init_m(error, 2, 16);   // 2 sec delay w32 fast blinks for I2C error
    init_m(success, 2, 2);  // 2 sec delay w4 medium blinks for I2C success
    init_m(next, 1, 4);     // 1 sec delay w1 slow blink for I2C next

    pinMode(LED_BUILTIN, OUTPUT);
    status(start);

    // init displays and show BOOT message
    init_Wire(AN_1, Wire);
    if (AN_display_1.begin(devices[AN_1].address,
                            DEFAULT_NOTHING_ATTACHED,
                            DEFAULT_NOTHING_ATTACHED,
                            DEFAULT_NOTHING_ATTACHED,
                             Wire) == false)
    {
      status(error);
    }
    AN_display_1.print("BOOT");
    Wire.end();

    init_Wire(AN_2, Wire1);
    if (AN_display_2.begin(devices[AN_2].address,
                            DEFAULT_NOTHING_ATTACHED,
                            DEFAULT_NOTHING_ATTACHED,
                            DEFAULT_NOTHING_ATTACHED,
                             Wire1) == false)
    {
      status(error);
    }
    AN_display_2.print("BOOT");
    Wire1.end();
}

void loop() 
{
    showTemp_1();
    delay(1000);
    showTemp_2();
    status(next);
}

// displayAN: right align number based on value, X 100 to remove decimal point
void displayAN(float value,HT16K33 AN)
{
    sNumber = String(value,1);
    if (value < 10.0)
    {
        sNumber = "  " + sNumber;
    }
    else if (value < 100.0)
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

void init_m(unsigned int index, unsigned int delay, unsigned int times)
{
    messages[index].delay = delay;
    messages[index].times = times;
}

void init_Wire(unsigned int index, TwoWire &Wire)
{
    Wire.setSDA(devices[index].SDA);
    Wire.setSCL(devices[index].SCL);
    Wire.begin();

}

void showTemp_1()
{
    init_Wire(AN_1, Wire);
    status(next);
    mlx_1.begin();
    float IR_temperature = mlx_1.readObjectTempF();
    displayAN(IR_temperature, AN_display_1);
    Wire.end();
}

void showTemp_2()
{
    init_Wire(AN_2, Wire1);
    init_Wire(temp_2, Wire);
    mlx_2.begin();
    float IR_temperature = mlx_2.readObjectTempF();
    displayAN(IR_temperature, AN_display_2);
    Wire.end();
    Wire1.end();
}

void status(unsigned int msg)
{
    for (unsigned int i = 0; i < messages[msg].delay; i++) 
    {
        for (unsigned int j = 0; j < messages[msg].times; j++) 
        {    
            digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on 
            delay(1000/messages[msg].times/2); // delay seconds/2, blink n * 2 
            digitalWrite(LED_BUILTIN, LOW);    // turn the LED off 
            delay(1000/messages[msg].times/2); // delay seconds/2, blink n * 2 
        }
    }
}
