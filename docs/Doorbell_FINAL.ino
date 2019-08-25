#include <RunningAverage.h>                                                             //Including necessary libraries      
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "Statistic.h"
#include "Pitches.h"
#include "MusicSamples.h"

short led = 13;                                                                         //default LED pin for Lilypad Arduino USB
short switchMode = A3;                                                                  //Pins for switch and push button
short pushButton = A4;

bool debug = false;                                                                     //Enable to output calibration and pulse data to the serial monitor
bool plotDer = false;                                                                   //Enable to plot the derivative of the voltage data from the ADC
bool plotVolt = false;                                                                  //Enable to plot the voltage from the ADC
bool isCalibrating = false;
bool modeState;

unsigned long sendTime = 0;                                                             //The time (in microseconds) that a pulse is sent at
unsigned long recTime = 0;                                                              //The time (in microseconds) that a pulse is sent at
unsigned long travelTime = 0;                                                           //The difference between sendTime and receiveTime in milliseconds, used to capture the travel time for pulses

Adafruit_ADS1115 ads(0x48);                                                             //Initilizing 16 bit I2C ADC (ADS1115)
float* voltArr = new float[2];                                                          //Used to store the last two inputs, used to calculate the derivative
float stdev = 0;                                                                        //Standard deviation of the calibration data, used for filtering out noise
float avgVolt = 0;

RunningAverage rAvg(30);                                                                //Creating a running average
float avgPer = 0;                                                                       //Variables for average calculation
float countAvg = 0;
float sumPer = 0;

float currentTime = 0;                                                                  //Variables for time tracking
float lastTime = 0;

short playCounter = 0;


void setup() {
  pinMode(led, OUTPUT);                                                                 //Declaring digital pins
  pinMode(pinSpeaker, OUTPUT);

  pinMode(switchMode, INPUT);
  pinMode(pushButton, INPUT);

  randomSeed(analogRead(A5));                                                           //Creating random seed of numbers from unused pin

  if (debug) {
    Serial.begin(9600);
  }
  ads.begin();

  Statistic myStats;                                                                    //Calibration sequence
  isCalibrating = true;
  short sampleSize = 2048;
  for (int i = 0; i < sampleSize; i++) {                                                //Use the derivative function to create a sample of ambient conditions
    myStats.add(derivative(-1));
    digitalWrite(led, HIGH);
  }
  avgPer /= countAvg;
  isCalibrating = false;
  stdev = myStats.pop_stdev();                                                          //Using statistics to calculate the standard deviation and average to serve as ambient conditions
  avgVolt = myStats.average();



  if (debug) {
    Serial.print(F("Standard Deviation: "));
    Serial.println(stdev);
  }
  rAvg.fillValue(-10000000, 1);
  digitalWrite(led, LOW);
}

void loop(void) {
  digitalWrite(pinSpeaker, LOW);
  modeState = digitalRead(switchMode);                                                  //Reading digital state of switch
  if (modeState) {                                                                      //If high, run the sendPulse function
    sendPulse();
  }
  else {                                                                                //If low, run the tap music function
    tapMusic();
  }
  if (digitalRead(pushButton)) {                                                        //In either mode, play music if the push button is pressed
    playMusic();
  }
}


void tapMusic() {                                                                       //Run the derivative function with a std coeffiction of 20
  derivative(20);
}

void sendPulse() {                                                                      //Sending pulses using transmitting piezo
  digitalWrite(pinSpeaker, LOW);                                                        //Low signal to transmitting piezo
  if (debug) {
    Serial.println(F("LOW"));
  }

  for (int i = 0; i < 100; i++) {                                                       //Sending a low signal for 100 iterations of a for loop
    derivative(2.5);
  }

  digitalWrite(pinSpeaker, HIGH);                                                       //High signal to transmitting piezo
  sendTime = micros();
  if (debug) {
    Serial.println(F("HIGH"));
  }

  for (int i = 0; i < 10; i++) {                                                        //Sending a high signal for 10 iterations of a for loop
    derivative(2.5);
  }

  if (debug) {
    Serial.println();
  }
  delayMicroseconds(10);
}

void playMusic() {                                                                      //Playing music from the created Music Samples library
  digitalWrite(led, HIGH);
  short randNum = random(0, 4);                                                         //Generating a random song choice
  switch (randNum) {
    case (0):
      furElise();                                                                       //Playing Fur Elise
      break;
    case (1):
      odeToJoy();                                                                       //Playing Ode to Joy
      break;
    case (2):
      CiDM();                                                                           //Playing Canon in D major
      break;
    case (3):
      turkishMarch();                                                                   //Playing bilgens fav song
      break;
    default:
      for (int i = 0; i < 5; i++)                        
      {
        digitalWrite(led, HIGH);
        delay(500);
        digitalWrite(led, LOW);
      }
  }
  delay(5000);                                                                          //Delay 5 seconds after the song ends
  digitalWrite(led, LOW);                                                               //Turn off led
}

float derivative(float stdevCo) {
  lastTime = currentTime;
  int16_t rxAccel = ads.readADC_SingleEnded(1);                                         //Reading recieving piezo voltage via the 16 bit ADC
  currentTime = millis();
  float dt = currentTime - lastTime;                                                    //Finding the difference in time between loops

  if (debug) {
    Serial.print(F("dt: "));
    Serial.println(dt);
  }

  voltArr[0] = voltArr[1];                                                              //Incrementing the values of the array each time it goes through the loop
  voltArr[1] = rxAccel;
  float dx = (voltArr[1] - voltArr[0]);                                                 //Setting dx to the difference between incremental voltage readings
  float dxdt = 0;
  if (dt != 0) {
    dxdt = abs(dx / dt);                                                                //Setting dxdt equal to the magnitude of dx/dt
  }

  if (plotDer) {
    Serial.println(dxdt, 9);                                                            //Printing dxdt and comparing with average voltage + standard deviation
    Serial.println(avgVolt + stdev * stdevCo);
  }
  if (plotVolt) {
    Serial.println(rxAccel, 9);                                                         //Printing voltage values from recieving piezo
  }
  if (debug) {
    Serial.print(F("dx/dt: "));                                                         //Printing dxdt along with average voltage bounds
    Serial.println(dxdt, 9);
    Serial.print(F("Upper limit: "));
    Serial.println(avgVolt + stdev * stdevCo);
    Serial.print(F("Lower limit: "));
    Serial.println(avgVolt - stdev * stdevCo);
  }

  if (dxdt > avgVolt + stdev * stdevCo || dxdt < avgVolt - stdev * stdevCo) {           //If dxdt is not within calibration values and standard deviation range 
    if (modeState && playCounter > 100) {                                               //If switch is high and counter is > 100, track time(pulse detection)                                       
      recTime = micros();
      travelTime = recTime - sendTime;                                                  //Time difference between sent and recieved pulses
      recTime = 0;
      sendTime = 0;
      rAvg.addValue(travelTime);
      sumPer += travelTime;                                                             //Sum of travel times
      countAvg++;
      avgPer = sumPer / countAvg;                                                       //Average travel time

      if (debug) {
        Serial.print(F("travelTime: "));                                                //Printing travel time
        Serial.println(travelTime);
      }
      Serial.println(rAvg.getAverage() - avgPer * 1.2);

      if (!isCalibrating && rAvg.getAverage() > 1.2 * avgPer && playCounter > 1000) {   //If not calibrating and running average is greater than 1.2 * average travel time and counter > 1000
        digitalWrite(led, HIGH);                                                        //Turn led on
        playMusic();                                                                    //Play music
        playCounter = 0;                                                                //Reset counter
      }
    }
    if (!isCalibrating && playCounter > 100 && !modeState) {                            //If not calibrating and counter > 100 and switch is low(tapping)
      digitalWrite(led, HIGH);                                                          //Turn led on
      playMusic();                                                                      //Play music
      playCounter = 0;                                                                  //Reset counter
    }
  }
  else {
    digitalWrite(led, LOW);                                                             //If dxdt is not outside standard deviation range, set led to low
  }

  playCounter++;                                                                        //Increase counter each loop

  return (dxdt);                                                           
}
