/* 
 * //////////////////////////////////////////////////
 * //making sense of the Parallax PIR sensor's output
 * //////////////////////////////////////////////////
 *
 * Switches a LED according to the state of the sensors output pin.
 * Determines the beginning and end of continuous motion sequences.
 *
 * @author: Kristian Gohlke / krigoo (_) gmail (_) com / http://krx.at
 * @date:   3. September 2006 
 *
 * kr1 (cleft) 2006 
 * released under a creative commons "Attribution-NonCommercial-ShareAlike 2.0" license
 * http://creativecommons.org/licenses/by-nc-sa/2.0/de/
 *
 *
 * The Parallax PIR Sensor is an easy to use digital infrared motion sensor module. 
 * (http://www.parallax.com/detail.asp?product_id=555-28027)
 *
 * The sensor's output pin goes to HIGH if motion is present.
 * However, even if motion is present it goes to LOW from time to time, 
 * which might give the impression no motion is present. 
 * This program deals with this issue by ignoring LOW-phases shorter than a given time, 
 * assuming continuous motion is present during these phases.
 *  
 */

#include <SoftwareSerial.h>


/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int tempPin = 0;
int lcdPin = 8;
SoftwareSerial mySerial = SoftwareSerial(255, lcdPin);

/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);
  pinMode(lcdPin, OUTPUT);
  digitalWrite(lcdPin, HIGH);
  
  //getting the voltage reading from the temperature sensor
  float temperature = getVoltage(tempPin);
  //converting mv to temp
  temperature = (((temperature - .5) * 100) * 1.8) + 32;
  mySerial.write(12);
  mySerial.print("Starting Temp ");
  mySerial.write(13);
  mySerial.print(temperature);
  Serial.println("---");
  Serial.print("Starting Temp is ");
  Serial.print(temperature);
  Serial.println("F");
  Serial.println("---");
  delay(3000);
  
  //give the sensor some time to calibrate
  mySerial.write(12);
  mySerial.print("calibrating sensor ");
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      mySerial.write(12);
      mySerial.print("calibrating...");
      delay(1000);
      }
    Serial.println(" done");
    mySerial.write(13);
    mySerial.print("Done");
    delay(2000);
    mySerial.write(12);
    mySerial.print("Done");
    mySerial.write(13);
    mySerial.print("SENSOR ACTIVE");
    Serial.println("SENSOR ACTIVE");
    delay(50);
  }

////////////////////////////
//LOOP
void loop(){
  float temperature = getVoltage(tempPin);
  temperature = (((temperature - .5) * 100) * 1.8) + 32;
  
     if(digitalRead(pirPin) == HIGH){
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.print(" sec");
         Serial.print(" and the temperature is ");
         Serial.print(temperature);
         Serial.println("F");
         mySerial.write(12);
         mySerial.print("motion detected");
         mySerial.write(13);
         mySerial.print("temp is ");
         mySerial.print(temperature);
         mySerial.print("F");
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       
       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.print(" sec");
           Serial.print(" and the temperature is ");
           Serial.print(temperature);
           Serial.println("F");
           mySerial.write(12);
           mySerial.print("motion ended");
           mySerial.write(13);
           mySerial.print("temp is ");
           mySerial.print(temperature);
           mySerial.print("F");           
           delay(50);
           }
       }
  }
  
/*
 * getVoltage() - return the voltage on teh analog input defined by pin
*/

float getVoltage(int pin){
  //converiting from a 0 to 1023 digital range
  //to 0 to 5 volts (each 1 reading equals ~ 5 millivolts
  return(analogRead(pin) * .004882814);
}
