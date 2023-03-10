#include <IRremote.h>             //added necessary libraries 
#include <IRremoteInt.h>
#include <DS3231.h>
#include <LiquidCrystal.h>



const int buzzer = 9;        //buzzer pin

const int RECV_PIN = 7;             //IR remote pin
IRrecv irrecv(RECV_PIN);            //IR remote class
decode_results results;             //IR remoe object
unsigned long key_value = 0;        //needed to reset IR

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;        //pins for LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Time t;                                //create object for time storage
const int Contrast = 85;              //set contrast

DS3231 rtc(SDA, SCL);                 //initialize rtc

unsigned int alarm_hour = 0;         //alarm hour
unsigned int alarm_min = 0;          //alarm minutes

bool alarm_on = false;               //alarm states
bool minSet = false;                 
bool hourSet = false;

const int pinPot = 2;                //pin for potentiometer

void setup()
{
  
  analogWrite(6,Contrast);           //set contrast for display
  
  Serial.begin(9600);                //enable each compound
  rtc.begin();  
  lcd.begin(16, 2);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  
  pinMode(buzzer,OUTPUT);
}

void loop()
{
  
int val;
val = analogRead(pinPot);                  //read value from potentiometer to control the alarm mode
Serial.println(val);
delay(50);
if (val > 900) {                           //if joystick (potentiometer) is pressed, alarm mode turns off
  alarm_on = false;
  alarm_hour = 0;
  alarm_min = 0;
  minSet = false;
  hourSet = false;
  results.value = key_value;
}

//For debugging purposes
//Serial.println("hours: ");
//Serial.println(alarm_hour);
//Serial.println("min: ");
//Serial.println(alarm_min);
//delay(100);


  if (alarm_on == false) {

     lcd.setCursor(0,0);                    //will just print current time and date
     lcd.print("Time:  ");
     lcd.print(rtc.getTimeStr());
   
     lcd.setCursor(0,1);
     lcd.print("Date: ");
     lcd.print(rtc.getDateStr());

      delay(50);
    
    if (irrecv.decode(&results)) {            //if the ir receiver module receives data

          lcd.clear();
          delay(100);
          lcd.setCursor(0, 0);
          lcd.print("Set Alarm:");             //alarm mode turns on
          lcd.setCursor(0, 1);
          lcd.print("00:00:00");
          delay(200);
          alarm_on = true;
          irrecv.resume(); // Receive the next value  
     } 
   }
   else if (alarm_on == true) {                              

         if (minSet == false) {                         //displays current alarm setting
             lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("Set Alarm:");
             lcd.setCursor(0, 1);
             lcd.print(alarm_hour);
             lcd.print(":");
             if (alarm_min < 10) {
                lcd.print("0");
                lcd.print(alarm_min);
             } else {
                lcd.print(alarm_min);
             }   
             lcd.print(":00"); 
             delay(500);
    
          }
          
         if (hourSet == false) {
            if (irrecv.decode(&results)){
          
                if (results.value == 0xFF906F) {            //set hours, arrow up, +1 hour
                    if (alarm_hour < 24) {
                       alarm_hour = alarm_hour + 1;
                    }
                    else {
                       alarm_hour = 0;
                    }        
               } else if (results.value == 0xFFE01F) {              //set hours, arrow down, -1 hour
                    if (alarm_hour > 0) {
                       alarm_hour = alarm_hour - 1;
                    }
                    else {
                       alarm_hour = 23;
                    }    
                } else if (results.value == 0xFFE21D) {            //enter
                       hourSet = true;
                }
              }
                irrecv.resume(); 
         } else if ((hourSet == true) && (minSet == false)){
              results.value = 0;
              if (irrecv.decode(&results)){
          
                  if (results.value == 0xFF906F) {                  //set minutes, arrow up, +1 minute
                        if (alarm_min < 59) {
                           alarm_min = alarm_min + 1;
                        } else {
                           alarm_min = 0;
                        }        
                  } else if (results.value == 0xFFE01F) {            //set minutes, arrow down, -1 minute
                        if (alarm_min > 0) {
                           alarm_min = alarm_min - 1;
                        } else {
                           alarm_min = 59;
                        }    
                  } else if (results.value == 0xFFE21D) {            //enter
                       minSet = true;
                    }
             }
             irrecv.resume();   
           }

           if (minSet == true) {                                     //will just display time and date until alarm kicks in
                lcd.setCursor(0,0);
                lcd.print("Time:  ");
                lcd.print(rtc.getTimeStr());
   
                lcd.setCursor(0,1);
                lcd.print("Date: ");
                lcd.print(rtc.getDateStr());

                
                t = rtc.getTime();
                if ((t.hour == alarm_hour) && (t.min == alarm_min)) {   //will sound badly until you press the joystick (turn potentiometer)
                   while (val < 900) {
                    lcd.clear();
                    buzzerOn();
                    val = analogRead(pinPot);
                    Serial.println(val); //prints the values of potentiometer for debugging
                   }
                }
                
            }

            if (irrecv.decode(&results)) {                       //you can check current alarm setting
                if (results.value == 0xFFB04F) {
                    checkAlarm();
                 }
            }
            irrecv.resume();
         
    }

}

void message() {                                         //function for alarm message (can put different notes)
  lcd.setCursor(0,0);
  lcd.print("Wake up!"); 
  delay(100);
  lcd.print("");
  }

void checkAlarm() {                                //function for alarm check
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alarm set for:");
  lcd.setCursor(0,1);
  lcd.print(alarm_hour);
  lcd.print(":");
  if (alarm_min < 10) {
  lcd.print("0");
  lcd.print(alarm_min);
  } else {
  lcd.print(alarm_min);
  }   
  lcd.print(":00"); 
  delay(2000);
  }
  
void buzzerOn () {                          //function for buzzer to work
  tone(buzzer, 800);
  message();
  delay(100);
  tone(buzzer, 200);
  message();
  delay(100);
  noTone(buzzer);
  message();
  delay(100);
  tone(buzzer, 1500);
  message();
  delay(100);
  noTone(buzzer);
  }
