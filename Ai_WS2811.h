#include <Arduino.h>
#include <util/delay.h>

// Assume Arduino Uno, digital pin 8 lives in Port B

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PINB
#define LED_BIT _BV(0)
#define NOP __asm__("nop\n\t")

struct CRGB
{
    unsigned char g;
    unsigned char r;
    unsigned char b;
};


class Ai_WS2811 
{
  private:
    int m_nLeds;
    unsigned char m_nDataRate;
    unsigned long m_nCounter;
    CRGB *m_pData;
    CRGB *m_pDataEnd;

  public:
    byte _r, _g, _b;
    uint8_t *led_arr;
    void init(uint8_t pin,uint16_t nPixels, CRGB * buf);
    void sendLedData(void);
    CRGB *getRGBData() { return m_pData; } 
};
