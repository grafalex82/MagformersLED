#include "Ai_WS2811.h"
#include "avr/sleep.h"
#include "avr/wdt.h"
#include <avr/eeprom.h>

// Number of total LEDs on the board. Mine has 4x4 LEDs
#define NUM_HW_PIXELS 16
// Pin number where LED data pin is attached
#define DATA_PIN 0
// Pin number where mode switch button is attached
#define BUTTON_PIN 2
// Power Enabled pin
#define POWER_EN_PIN 1
// Max brightness (dimming the light for debugging)
#define MAX_VAL 250

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

//TODO: Use PROGMEM
// LED indexes for different patterns
uint8_t circleLEDIndexes[] = {0, 1, 2, 3, 4, 11, 12, 13, 14, 15, 8, 7};
uint8_t beaconLEDIndexes[] = {6, 5, 10, 9};
uint8_t policeLEDIndexes[] = {7, 6, 10, 11, 4, 5, 9, 8};

// Driver
Ai_WS2811 ws2811;

void setup()
{
    // Set up power
    pinMode(POWER_EN_PIN, OUTPUT);
    digitalWrite(POWER_EN_PIN, HIGH);

	// initialize LED data pin
	pinMode(LED_PIN, OUTPUT);

    // Initialize button pin
    pinMode(BUTTON_PIN, INPUT);

    // Initialize WS8211 library
    static CRGB ledsBuf[NUM_HW_PIXELS];
	ws2811.init(DATA_PIN, NUM_HW_PIXELS, ledsBuf);

    // Set the watchdog timer to 2 sec
    wdt_enable(WDTO_2S);
}

/*
ISR (PCINT0_vect) 
{    
    // Nothing to do. Just need to wake up CPU
}
*/
void setRgb(uint8_t led_idx, uint8_t r, uint8_t g, uint8_t b)
{
    CRGB * leds = ws2811.getRGBData();
    leds[led_idx].r = r;
    leds[led_idx].g = g;
    leds[led_idx].b = b;
}

/**
* HVS to RGB conversion (simplified to the range 0-255)
**/
void setHue(uint8_t led_idx, int hue, int brightness)
{
	//this is the algorithm to convert from RGB to HSV
	double r = 0;
	double g = 0;
	double b = 0;

	double hf = hue/42.6; // Not /60 as range is _not_ 0-360

	int i=(int)floor(hue/42.6);
	double f = hue/42.6 - i;
	double qv = 1 - f;
	double tv = f;

	switch (i)
	{
		case 0:
		r = 1;
		g = tv;
		break;
		case 1:
		r = qv;
		g = 1;
		break;
		case 2:
		g = 1;
		b = tv;
		break;
		case 3:
		g = qv;
		b = 1;
		break;
		case 4:
		r = tv;
		b = 1;
		break;
		case 5:
		r = 1;
		b = qv;
		break;
	}

    brightness = constrain(brightness, 0, MAX_VAL);

    setRgb(led_idx, 
        constrain(brightness*r, 0, MAX_VAL),
        constrain(brightness*g, 0, MAX_VAL),
	    constrain(brightness*b, 0, MAX_VAL)
    );
}

void rainbow()
{
    static uint8_t hue = 0;
    hue++;
    
    for (int led = 0; led < NUM_HW_PIXELS; led++)
        setHue(led, hue, MAX_VAL);
   
    ws2811.sendLedData();
    delay(80);
}

void slidingRainbow()
{
    static uint8_t pos = 0;
    pos++;
    
	for (int led = 0; led < ARRAY_SIZE(circleLEDIndexes); led++)
	{
		int hue = (pos + led*256/ARRAY_SIZE(circleLEDIndexes)) % 256;
		setHue(circleLEDIndexes[led], hue, MAX_VAL);
	}
   
	ws2811.sendLedData();
	delay(10);
}

int brightnessByPos(int pos, int ledPos, int delta)
{
    int diff = abs(pos - ledPos);
    if(diff > 127)
        diff = abs(256-diff);
    int brightness = MAX_VAL - constrain(MAX_VAL*diff/delta, 0, MAX_VAL);
    return brightness;
}

void orangeBeacon()
{
    const int ORANGE_HUE = 17;

    static uint8_t pos = 0;
    pos+=3;
    
    for (int led = 0; led < ARRAY_SIZE(circleLEDIndexes); led++)
    {
        int brightness = brightnessByPos(pos, led*255/ARRAY_SIZE(circleLEDIndexes), 70);
        setHue(circleLEDIndexes[led], ORANGE_HUE, brightness);
    }
   
    ws2811.sendLedData();
    delay(1);
}

void policeBeacon()
{
    const int RED_HUE = 0;
    const int BLUE_HUE = 170;
    
    static uint8_t pos = 0;
    pos += 2;
    
    const int delta = 70;
    for (int led = 0; led < ARRAY_SIZE(policeLEDIndexes); led++)
    {
        int ledPos = led*255/ARRAY_SIZE(policeLEDIndexes);
        int brightness = brightnessByPos(pos, ledPos, 50);
        setHue(policeLEDIndexes[led], RED_HUE, brightness);

        if(brightness == 0)
        {
            brightness = brightnessByPos((pos+100) % 256, ledPos, 50);
            setHue(policeLEDIndexes[led], BLUE_HUE, brightness);
        }
    }
   
    ws2811.sendLedData();
    delay(1);
}

void clearPixels()
{
    for(int i=0; i<NUM_HW_PIXELS; i++)
    {
        setRgb(i, 0, 0, 0);
    }
}

void redTrafficLights()
{
    setRgb(0, MAX_VAL, 0, 0);
    setRgb(1, MAX_VAL, 0, 0);
    setRgb(2, MAX_VAL, 0, 0);
    setRgb(3, MAX_VAL, 0, 0);
    ws2811.sendLedData();
}

void yellowTrafficLights()
{
    setRgb(4, MAX_VAL, MAX_VAL, 0);
    setRgb(5, MAX_VAL, MAX_VAL, 0);
    setRgb(6, MAX_VAL, MAX_VAL, 0);
    setRgb(7, MAX_VAL, MAX_VAL, 0);
    ws2811.sendLedData();
}

void greenTrafficLights()
{
    for(int i=8; i<16; i++)
        setRgb(i, 0, MAX_VAL, 0);
    ws2811.sendLedData();
}

enum TRAFFIC_LIGHTS
{
    NONE, RED, YELLOW, GREEN
};

struct trafficLightState
{
    uint8_t state;
    uint16_t duration;
};

const trafficLightState trafficLightStates[] = {
    {NONE, 1},         // clear yellow
    {RED, 7000},     // red
    {YELLOW, 2000},      // red + yellow
    {NONE, 1},         // clear red+yellow
    {GREEN, 7000},     // green
    {NONE, 300},       // Blinking green
    {GREEN, 300},       // Blinking green
    {NONE, 300},       // Blinking green
    {GREEN, 300},       // Blinking green
    {NONE, 300},       // Blinking green
    {GREEN, 300},       // Blinking green
    {NONE, 1},         // clear green
    {YELLOW, 2000},      // yellow
};

void trafficLights()
{
    static uint8_t curStateIdx = 0;
    static unsigned long curStateTimeStamp = 0;

    // Switch to a new state when time comes
    if(millis() - curStateTimeStamp > (unsigned long)trafficLightStates[curStateIdx].duration)
    {
        curStateIdx++;
        curStateIdx %= ARRAY_SIZE(trafficLightStates);
        curStateTimeStamp = millis();
    }
    
    switch(trafficLightStates[curStateIdx].state)
    {
        case NONE:
            clearPixels();
            ws2811.sendLedData();
            break;
        case RED:
            redTrafficLights();
            break;
        case YELLOW:
            yellowTrafficLights();
            break;
        case GREEN:
            greenTrafficLights();
            break;
        default:
            break;
    }

    // Just waiting
    delay(10);
}

void stars()
{
    const uint8_t numleds = 5;
    static uint8_t ledIndexes[numleds] = {0};    
    static uint8_t curVal[numleds] = {0};
    static uint8_t maxVal[numleds] = {0};

    for(int i=0; i<numleds; i++)
    {
        if(ledIndexes[i] == 0)
        {
            uint8_t led = rand() % (NUM_HW_PIXELS+1);
            CRGB * leds = ws2811.getRGBData();
            if(leds[led].r == 0)
            {
                ledIndexes[i] = led;
                maxVal[i] = rand() % (MAX_VAL-1) + 1;
                curVal[i] = 0;
            }
        }
        else
        {
            uint8_t led = ledIndexes[i];
            if(curVal[i] < maxVal[i])
                curVal[i]++;
            else
            if(curVal[i] == maxVal[i])
                maxVal[i] = 0;
            else
            if(curVal[i] == 0 || --curVal[i] == 0)
                ledIndexes[i] = 0;

            setRgb(led-1, curVal[i], curVal[i], curVal[i]);
        }
    }

    ws2811.sendLedData();
    delay(80);    
}

void randomColorsFadeInOut()
{
    static uint8_t color = 0;
    static bool goesUp = false;
    static uint8_t curLevel = 0;

    if(curLevel == 0 && !goesUp)
    {
        color = rand() % 256;
        goesUp = true;
    }

    if(curLevel == MAX_VAL && goesUp)
    {
        goesUp = false;
    }

    for(int led = 0; led < NUM_HW_PIXELS; led++)
        setHue(led, color, curLevel);

    if(goesUp)
        curLevel++;
    else
        curLevel--;

    ws2811.sendLedData();
    delay(10);    
}

void shutDown()
{
    clearPixels();
    ws2811.sendLedData();

    wdt_disable();
    digitalWrite(POWER_EN_PIN, LOW);

    // No power after this point
    while(true)
        ;
}

void shutdownOnTimeOut(bool resetTimer = false)
{
    static unsigned long periodStartTime = 0;

    if(periodStartTime == 0 || resetTimer)
    {
        periodStartTime = millis();
        return;
    }

    if(millis() - periodStartTime >= 90000UL)
    {   
        periodStartTime = 0;
        shutDown();
    }
}

void blinkLed()
{
    static unsigned long lastBlinkTime = 0;

    if(lastBlinkTime == 0)
        lastBlinkTime = millis();

    if(millis() - lastBlinkTime >= 500)
    {
        lastBlinkTime = millis();

        digitalWrite(0, !digitalRead(0));
    }
}

// List of pointers to functions that serve different modes
void (*Modes[])() = 
{
    rainbow,
    slidingRainbow,
    orangeBeacon,
    policeBeacon,
    trafficLights,
    stars,
    randomColorsFadeInOut
};

void loop()
{
    static uint8_t mode = eeprom_read_byte( (uint8_t*) 10 );
    static bool waitingForBtnUp = false;
    static long btnPressTimeStamp;

    // Button switches mode
    if(digitalRead(BUTTON_PIN) == HIGH && !waitingForBtnUp)
    {
        delay(20);
        if(digitalRead(BUTTON_PIN) == HIGH)
        {
            mode++;
            mode %= ARRAY_SIZE(Modes); // num modes
            
            clearPixels();
            ws2811.sendLedData();
            delay(1);

            eeprom_write_byte( (uint8_t*) 10, mode );

            waitingForBtnUp = true;
            btnPressTimeStamp = millis();

            shutdownOnTimeOut(true);
        }
    }

    // Shut down on long press over 5s
    if(digitalRead(BUTTON_PIN) == HIGH && waitingForBtnUp && millis() - btnPressTimeStamp > 5000)
        shutDown();

    // Detect button release
    if(digitalRead(BUTTON_PIN) == LOW && waitingForBtnUp)
        waitingForBtnUp = false;

    // display LEDs according to current mode
    Modes[mode]();

    // pong shutdown timer
    shutdownOnTimeOut();

    // Yes, we still alive
    wdt_reset();
}


