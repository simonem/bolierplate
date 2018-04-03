#include <Arduino.h>
#include "CONFIG.h"

#define LED_OFF HIGH
#define LED_ON LOW
  
class RGB_LED
{
    public:
    		RGB_LED(int R, int G, int B);
        void RefreshValues();
        void HandleTime(unsigned int ElapsedTime);
        void parseColorString(String color, int& red, int& green, int& blue);
        void setColor(String color);
        void setRGBaColor(uint8_t R, uint8_t G, uint8_t B, float a = 1.0);	
	  private:
        unsigned int  RGB_LED_Timing;
        bool State;
        int _Common;
        int _R_LED;
        int _G_LED;
        int _B_LED;
};
