#include <Arduino.h>
#line 1 "/Users/simone/Desktop/Tiles/tiles-bolierplate/boilerplate/boilerplate.ino"
#line 1 "/Users/simone/Desktop/Tiles/tiles-bolierplate/boilerplate/boilerplate.ino"
/********************************************************
  # NAME: boilerplate.ino
  # AUTHOR: Simone Mora (simonem@ntnu.no)
  # DATE: --
  # LICENSE: Apache V2.0
********************************************************/

#include <Wire.h>
#include <WInterrupts.h>
#include <bluefruit.h>
#include <string.h>
#include "BLE_Handler.h"
#include "Feedbacks_Handler.h"
#include "Sensors_Handler.h"
#include "CONFIG.h"
#include "STATUS_LED.h"
#include "BUZZER.h"

//Variables for timing
uint_fast16_t volatile number_of_ms = 10;    

// Variables for BLE
BLE_Handler BLE;

// Variables for Sensors
Sensors_Handler sensor_handle(&BLE);
ADXL345 *ACCELEROMETER = NULL;
LSM9DS0 *IMU = NULL;
CAP1188 *TOUCH = NULL;

// Variables for Feedbacks 
Feedbacks_Handler feedback_handle;
Haptic *HapticMotor;
DRV2605 *HapticIC;
RGB_LED *LED;
NEO_STRIP *STRIP = NULL;
MATRIX *M_MATRIX = NULL;
//STATUS_LED status_led;
BUZZER buzz;


int ledState = HIGH;         
int buttonState = LOW;             
int lastButtonState = LOW;  
int i = 0; 
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 1000;    // the debounce time; increase if the output flickers

#line 49 "/Users/simone/Desktop/Tiles/tiles-bolierplate/boilerplate/boilerplate.ino"
void setup(void);
#line 119 "/Users/simone/Desktop/Tiles/tiles-bolierplate/boilerplate/boilerplate.ino"
void loop(void);
#line 163 "/Users/simone/Desktop/Tiles/tiles-bolierplate/boilerplate/boilerplate.ino"
void timer_config(unsigned int ms);
#line 191 "/Users/simone/Desktop/Tiles/tiles-bolierplate/boilerplate/boilerplate.ino"
void TIMER1_Interrupt(void);
#line 49 "/Users/simone/Desktop/Tiles/tiles-bolierplate/boilerplate/boilerplate.ino"
void setup(void)
{
    

    if(!xLED_RGB){
        Serial.begin(9600);
    }        
    interrupts(); 

    Serial.println("START SETUP");
    
    //status_led = new STATUS_LED();
    //status_led.on(); 
    
    //Configure pushbutton to wake up the device
    pinMode(BUTTON_PIN, INPUT);
    digitalWrite(BUTTON_PIN,HIGH);

    //Initialization of SENSORS
    //Accelerometer
    if(xAccelerometer_ADXL345){
    ACCELEROMETER = new ADXL345(ACC_INT1_PIN);
    sensor_handle.setAccelerometer(ACCELEROMETER);
    }
    //IMU
    if(xIMU_LSM9DS0){
    IMU = new LSM9DS0();
    sensor_handle.setInertialCentral(IMU);
    }
    //Touch 
    if(xTouch_CAP1188){
    TOUCH = new CAP1188(0);
    sensor_handle.setTouchSensor(TOUCH);
    }
    //Intitialization of FEEDBACKS
    //DotMatrix
    if(xDOTM_MATRIX){
    M_MATRIX = new MATRIX(0);
    feedback_handle.setMATRIX(M_MATRIX);
    }
    //HapticMotor (Analog)
    if(xHAPTIC_analog){
    HapticMotor = new Haptic(VIBRATING_M_PIN);
    feedback_handle.setHapticMotor(HapticMotor);
    }
    //HapticMotor (Analog)
    if(xHAPTIC_IC){
    HapticIC = new DRV2605();
    feedback_handle.setHapticMotor(HapticIC);
    }
    //RGB LED
    if(xLED_RGB){
    LED = new RGB_LED(0, 1, 2);
    feedback_handle.setRGB_LED(LED);
    }  
    //NeoPixels LED/STRIP
    if(xLED_NEO){
    STRIP = new NEO_STRIP();
    feedback_handle.setNEO_STRIP(STRIP);
    //feedback_handle.setColor("blue");
    //delay(500);
    //feedback_handle.setColor("off");
    }

    Serial.println("**Setup Complete0**");
    BLE.start();
    timer_config(1000);
    Serial.println("**Setup Complete**");  
}

void loop(void)
{   
  
    //int readingA = digitalRead(16);
    //Serial.print("READING: "); Serial.println(readingA);
  /*
    if (readingA != lastButtonState) {
      // reset the debouncing timer
      //Serial.print("Button PRESSED: "); Serial.println(readingA);
      lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {    
      if (readingA != buttonState) {
        buttonState = readingA;
        if(buttonState == LOW){
          Serial.print("DEBUG: "); Serial.println(i);
          i++;
          delay(100);
          //BLE.shoutdown();
        }
      }
    }
    lastButtonState = readingA;
    */

    //Enable serial input of commands
    while(Serial.available()){
    String serialCommand = Serial.readString();
    serialCommand.trim();
    Token *command = new Token(serialCommand);
    BLE.ReceivedStack.push(command);
    }

    //Refresh values
   // status_led.RefreshValues();
    sensor_handle.pollEvent();
    feedback_handle.UpdateFeedback();
    BLE.ProcessEvents();
    delay(10); // 10ms Important delay, do not delete it
}


/*******************INTERNAL FUNCTIONS********************/
#define TRIGGER_INTERVAL 1000      // ms
void timer_config(unsigned int ms)
{
  Serial.println("Configuring timer");
    NRF_TIMER1->TASKS_STOP = 1;                                      // Stop timer
    NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;                        // sets the timer to TIME mode (doesn't make sense but OK!)
    NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_16Bit;               // with BLE only Timer 1 and Timer 2 and that too only in 16bit mode
    NRF_TIMER1->PRESCALER = 9;                                       // Prescaler 9 produces 31250 Hz timer frequency => t = 1/f =>  32 uS
                                                                     // The figure 31250 Hz is generated by the formula (16M) / (2^n)
                                                                     // where n is the prescaler value
                                                                     // hence (16M)/(2^9)=31250
    NRF_TIMER1->TASKS_CLEAR = 1;                                     // Clear timer
   
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //        Conversion to make cycle calculation easy
    //        Since the cycle is 32 uS hence to generate cycles in mS we need 1000 uS
    //        1000/32 = 31.25  Hence we need a multiplication factor of 31.25 to the required cycle time to achive it
    //        e.g to get a delay of 10 mS      we would do
    //        NRF_TIMER2->CC[0] = (10*31)+(10/4);
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   
    NRF_TIMER1->CC[0] = (number_of_ms * 31) + (number_of_ms / 4);                                                              // CC[0] register holds interval count value i.e your desired cycle
    NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;                                     // Enable COMAPRE0 Interrupt
    NRF_TIMER1->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);                             // Count then Complete mode enabled
    attachInterrupt(TIMER1_IRQn, TIMER1_Interrupt, RISING);                                                                    // also used in variant.cpp in the RFduino2.2 folder to configure the RTC1
    NRF_TIMER1->TASKS_START = 1;                                                                                               // Start TIMER
    Serial.println("Timer configured");
  }

void TIMER1_Interrupt(void)
{
    if (NRF_TIMER1->EVENTS_COMPARE[0] != 0)
    {   
        //Serial.println("TIMER UPDATED");
        sensor_handle.HandleTime(number_of_ms);
        feedback_handle.HandleTime(number_of_ms);
       // status_led.HandleTime(number_of_ms);
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    }
}

