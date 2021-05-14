/*
Wire Connection to the IBT-2 board(Driver):
IBT-2 pin 1 (RPWM) to Arduino pin 5(PWM)
IBT-2 pin 2 (LPWM) to Arduino pin 6(PWM)
IBT-2 pins 3 (R_EN), 4 (L_EN), 7 (VCC) to Arduino 5V pin
IBT-2 pin 8 (GND) to Arduino GND
IBT-2 pins 5 (R_IS) and 6 (L_IS) not connected
*/

////////////////////////////////////////////////// FOR TUNING ////////////////////////////////////////////////////////////////////////////////////////////////

#define RPM_Lvl_1 190           //RPM for level 1 vortex (Minnimum 0, Maximum 600)
#define RPM_Lvl_2 266           //RPM for level 2 vortex (Minnimum 0, Maximum 600)
#define RPM_Lvl_3 342           //RPM for level 3 vortex (Minnimum 0, Maximum 600)
#define RPM_Lvl_4 418           //RPM for level 4 vortex (Minnimum 0, Maximum 600)
#define RPM_Lvl_5 494           //RPM for level 5 vortex (Minnimum 0, Maximum 600)
#define RPM_Lvl_6 570           //RPM for level 6 vortex (Minnimum 0, Maximum 600)

#define Rise_Time_Lvl_1 1500    //Time(ms) to rise motor RPM from 0 to level 1 RPM ; 1500(Default)
#define Rise_Time_Lvl_2 2010    //Time(ms) to rise motor RPM from 0 to level 2 RPM ; 2010(Default)
#define Rise_Time_Lvl_3 2580    //Time(ms) to rise motor RPM from 0 to level 3 RPM ; 2580(Default)
#define Rise_Time_Lvl_4 3155    //Time(ms) to rise motor RPM from 0 to level 4 RPM ; 3155(Default)
#define Rise_Time_Lvl_5 3730    //Time(ms) to rise motor RPM from 0 to level 5 RPM ; 3730(Default)
#define Rise_Time_Lvl_6 4400    //Time(ms) to rise motor RPM from 0 to level 6 RPM ; 4400(Default)

#define Stop_Time_Lvl_1 667     //Time(ms) to stop motor from level 1 RPM to 0 ; 667(Default)
#define Stop_Time_Lvl_2 934     //Time(ms) to stop motor from level 2 RPM to 0 ; 934(Default)
#define Stop_Time_Lvl_3 1200    //Time(ms) to stop motor from level 3 RPM to 0 ; 1200(Default)
#define Stop_Time_Lvl_4 1467    //Time(ms) to stop motor from level 4 RPM to 0 ; 1467(Default)
#define Stop_Time_Lvl_5 1734    //Time(ms) to stop motor from level 5 RPM to 0 ; 1734(Default)
#define Stop_Time_Lvl_6 2000    //Time(ms) to stop motor from level 6 RPM to 0 ; 2000(Default)

#define Spin_Time 10000         //Motor continuous spin time in millisecond(ms)

#define RPWM_Output 5           // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
#define LPWM_Output 6           // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
byte Resolution = 20;   //Number of Steps to change motor speed from 0 to target RPM or target RPM to 0

///////////////////////////////////////////////// FOR INITIALIZE TUNING VARIABLES  /////////////////////////////////////////////////////////////////////////////

int RPM[6] = {RPM_Lvl_1, RPM_Lvl_2, RPM_Lvl_3, RPM_Lvl_4, RPM_Lvl_5, RPM_Lvl_6};
int Rise_Time[6] = {Rise_Time_Lvl_1, Rise_Time_Lvl_2, Rise_Time_Lvl_3, Rise_Time_Lvl_4, Rise_Time_Lvl_5, Rise_Time_Lvl_6};
int Stop_Time[6] = {Stop_Time_Lvl_1, Stop_Time_Lvl_2, Stop_Time_Lvl_3, Stop_Time_Lvl_4, Stop_Time_Lvl_5, Stop_Time_Lvl_6};


//////////////////////////////////////////////// VARIABLES FOR RUN_MOTOR FUNCTION  /////////////////////////////////////////////////////////////////////////////

unsigned long Previous_Time = 0;
bool Motor_Enable = 0;
byte Motor_Speed = 1;
bool Rise_Enable = 0;
bool Stop_Enable = 0;
int Rise_Count = 1;
int Stop_Count = 1;
int PWM_Speed;

unsigned long Test_Time = 0;
////////////////////////////////////////////////////////// VOID SETUP  /////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);
  Serial.print(19200);
}
 
///////////////////////////////////////////////////////// VOID MOTOR() ////////////////////////////////////////////////////////////////////////////////////////////
int Motor(bool Y, int X)
{
  Rise_Enable = Y;
  if(Rise_Count <= Resolution && Rise_Enable == 1)                            // Rising Stage
  {
    if(millis() >= Previous_Time + ((Rise_Time[X-1]) / (Resolution)))
    {
      Previous_Time = millis();
      PWM_Speed = map(RPM[X-1],0,600,0,255);
      analogWrite(LPWM_Output, 0);
      analogWrite(RPWM_Output, (PWM_Speed * Rise_Count) / Resolution);
//      Serial.print("Rising ");
//      Serial.println(Rise_Count);
      Rise_Count++;
    }
  }

  else if (Rise_Count > Resolution && Stop_Enable == 0)                       // Stable Stage
  {
    if(millis() >= Previous_Time + Spin_Time)
    {
      Stop_Enable = 1;
    }
//    Serial.println("Stable");
  }

  else if (Stop_Count <= Resolution && Stop_Enable == 1)                      // Braking Stage
  {
    if(millis() >= Previous_Time + ((Stop_Time[X-1])/(Resolution)))
    {
      Previous_Time = millis();
      PWM_Speed = map(RPM[X-1],0,600,0,255);
      analogWrite(LPWM_Output, 0);
      analogWrite(RPWM_Output, (PWM_Speed * (Resolution - Stop_Count)) / Resolution);
//      Serial.print("Braking ");
//      Serial.println(Stop_Count);
      Stop_Count++;
    }
  }

  if(Rise_Count > Resolution && Stop_Count > Resolution)                      //Prepare for next run
  {
    Rise_Count = 1;
    Stop_Count = 1;
    Rise_Enable = 0;
    Stop_Enable = 0;
//    Serial.println("Finish");
    return 0;
  }
  if(Y == 1)
  {
    return 1;
  }
  
}
 
////////////////////////////////////////////////////////// VOID LOOP ////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  Motor_Enable = Motor(Motor_Enable,Motor_Speed);   // Please keep this line of code na kub P'Lek, It has to run everyloop. If you want motor to run, You just set Motor_Enable = 1. There are 6 motor speed in this function.
                                                    //Motor_Enable (0 = Disable(Default), 1 = Enable); Motor_Speed Range is 1(Min) to 6(Max).  ;When motor finish 1 run, This function will automatically return 0 to Motor_Enable.

//You can put your cade here!!!
////////////////////////////////////////////////////////////////////////////////////
if(millis() >= Test_Time + 30000) //Test Condition; Motor will run every 30 seconds with Motor Speed Lvl 3. ;You can delete This!!
{
  Motor_Enable = 1;
  Motor_Speed = 3;
}
/////////////////////////////////////////////////////////////////////////////////////
}
