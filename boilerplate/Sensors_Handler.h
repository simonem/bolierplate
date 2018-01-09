#include "ADXL345.h"
//#include "CAP1188.h"
#include "BLE_Handler.h"

class Sensors_Handler
{
    public:
      Sensors_Handler(BLE_Handler *Handler);      //default constructor
      String pollEvent();                         // If an event has occured returns the event code
      void HandleTime(unsigned int ElapsedTime);   
      void setAccelerometer(ADXL345 *Acc);  // Set the private member _Accelerometer with an existing instance of an Accelerometer object
//      void setTouchSensor(CAP1188 *Touch);  

    private:
      bool EventTriggered;      // True if an event has occured, else false. Reset on read with pollEvent();
      String EventString;       // Event code according to protocol.h
      bool _AccelerometerAvailable;
//      bool _TouchSensorAvailable;
      BLE_Handler *BLE;
      int states;
      void state_change();

      //Accelrometer
      ADXL345 *_Accelerometer;            // Handle an accelerometer object
      #define ACCELEROMETER_UPDATE  100   //Accelerometer refresh period
      unsigned int Accelerometer_Timing;

      //Sensors source of TokenSoloEvent
//      CAP1188 *_TouchSensor;
//      #define TOUCHSENSOR_UPDATE  100   //Accelerometer refresh period
//      unsigned int TouchSensor_Timing;
};

