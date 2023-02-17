// blink_w_2struct - blink 2 leds without delay using a struct

struct blinker
{
  uint8_t ledPin;           // the number of the LED pin
  uint8_t state;          // the state (HIGH/LOW) the LED pin
  uint16_t interval;      // interval at which to blink (milliseconds)
  uint16_t previousMillis;   // will store last time LED was updated
} ;

struct blinker LED0;
struct blinker LED1;

void setup ()
{
  LED0.ledPin = LED_BUILTIN;
  LED0.state = LOW;
  LED0.interval = 1000;
  LED0.previousMillis = 0;
  pinMode(LED0.ledPin, OUTPUT);      

  LED1.ledPin = 5;
  LED1.state = LOW;
  LED1.interval = 500;
  LED1.previousMillis = 0;
  pinMode(LED1.ledPin, OUTPUT);      

}
 
void loop()
{
    uint16_t currentMillis = millis();
    if(currentMillis - LED0.previousMillis > LED0.interval) 
    {
        LED0.previousMillis = currentMillis;   
        LED0.state = !LED0.state;
        digitalWrite(LED0.ledPin, LED0.state);
    }

    if(currentMillis - LED1.previousMillis > LED1.interval) 
    {
        LED1.previousMillis = currentMillis;   
        LED1.state = !LED1.state;
        digitalWrite(LED1.ledPin, LED1.state);
    }
}
