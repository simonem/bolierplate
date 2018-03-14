#include "Feedbacks_Handler.h"

Feedbacks_Handler::Feedbacks_Handler()
{
    HapticAvailable = false;
    HapticICAvailable = false;
    RGB_LEDAvailable = false;
    NEO_STRIPAvailable = false;
    MATRIX_Available = false;
    BUZZER_Available = false;
}

// RGB LED
void Feedbacks_Handler::setRGB_LED(RGB_LED *pLED)
{
  LED = pLED;
  RGB_LEDAvailable = true;
}

// NEOPIXEL LED/STRIP
void Feedbacks_Handler::setNEO_STRIP(NEO_STRIP *pSTRIP)
{
 STRIP = pSTRIP;
 NEO_STRIPAvailable = true;
}

// Common LED methods
void Feedbacks_Handler::setColor(String color)
{
  if(RGB_LEDAvailable)
    LED->setColor(color);
  else if(NEO_STRIPAvailable)
    STRIP->setColor(color);
}

void Feedbacks_Handler::blink(String color)
{
    if(NEO_STRIPAvailable)
    STRIP->blink(color);
}

void Feedbacks_Handler::fade(String color)
{
    if(NEO_STRIPAvailable)
    STRIP->fade(color);
}

// MATRIX
void Feedbacks_Handler::setMATRIX(MATRIX *pMATRIX)
{
  MATRIXX = pMATRIX;
  MATRIX_Available = true;
}

void Feedbacks_Handler::startGazing()
{
  MATRIXX->start_gazing();
}

void Feedbacks_Handler::stopGazing()
{
  MATRIXX->stop_gazing();
}

void Feedbacks_Handler::showFace(String type)
{
  MATRIXX->face(type);
}

//Haptic Motor
void Feedbacks_Handler::setHapticMotor(Haptic *pHapticMotor)
{
    HapticMotor = pHapticMotor;
    HapticAvailable = true;
}

void Feedbacks_Handler::setHapticMotor(DRV2605 *pHapticIC) {
    HapticIC = pHapticIC;
    HapticICAvailable = true;
}

void Feedbacks_Handler::Vibrate(String Type) {
    if(HapticICAvailable) HapticIC->Vibrate(Type.toInt());
}

void Feedbacks_Handler::burst() {
    if(HapticICAvailable) HapticIC->burst();
}

void Feedbacks_Handler::shortv() {
    if(HapticAvailable) HapticMotor->VibrateShort();
    else if(HapticICAvailable) HapticIC->shortv();
}

void Feedbacks_Handler::longv() {
    if(HapticAvailable) HapticMotor->VibrateLong();
    else if(HapticICAvailable) HapticIC->longv();
}

void Feedbacks_Handler::raise() {
    if(HapticICAvailable) HapticIC->raise();
}

void Feedbacks_Handler::fall() {
    if(HapticICAvailable) HapticIC->fall();
}

void Feedbacks_Handler::raiseFall() {
    if(HapticICAvailable) HapticIC->raiseFall();
}

// Buzzer
void Feedbacks_Handler::setBuzzer(BUZZER *pBuzzer)
{
    Buzz = pBuzzer;
    BUZZER_Available = true;
}

void Feedbacks_Handler::buzz()
{
    Buzz->play_melody();
}


//Service Methods
String Feedbacks_Handler::UpdateFeedback()
{
  if(HapticAvailable)
    HapticMotor->RefreshValues();
  if(MATRIX_Available)
    MATRIXX->RefreshValues();
  if(NEO_STRIPAvailable)
    STRIP->RefreshValues();
}

void Feedbacks_Handler::HandleTime(unsigned int ElapsedTime)
{
  if(HapticAvailable)
    HapticMotor->HandleTime(ElapsedTime);
  if(MATRIX_Available)
    MATRIXX->HandleTime(ElapsedTime);
  if(NEO_STRIPAvailable)
    STRIP->HandleTime(ElapsedTime);
    //Serial.print("Elapsed Time: "); Serial.println(ElapsedTime);
} 

