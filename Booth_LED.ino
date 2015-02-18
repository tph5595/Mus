/******************************************************
 *Booth Led Lighting System
 * Made by Taylor Henderson (Head Lights, go figure)
 *Summer 2013 --- Class of 2014
 * http://WeHakkon.com 
 * with a little sound knowledge help of 
 * Stuart Olivera(Head sound/STD)
 * Class 2014 
 *http://stuartolivera.com
 ******************************************************/


//arrays for all the different types of pins
// pins 0,7,14 all on the same led
int redPins[] = {0,1,2,3,4,5,6};
int bluePins[] = {7,8,9,10,11,12,13};
int greenPins[] = {14,15,16,17,18,19,20,21};

//sets up some defualt colours
int redA[] = {255, 0, 0};
int orangeA[] = {255, 125, 0};
int yellowA[] = {255, 255, 0};
int springGreenA[] = {125, 255, 0};
int greenA[] = {0, 255, 0};
int turquoiseA[] = {0, 255, 125};
int cyanA[] = {0, 255, 255};
int oceanA[] = {0, 125, 255};
int blueA[] = {0, 0, 255};
int violetA[] = {125, 0, 255};
int magentaA[] = {255, 0, 255};
int raspberryA[] = {255, 0, 125};

//make an array of the colour arrays for easy access
int* colour[] = {redA, orangeA, yellowA, springGreenA, greenA, turquoiseA, cyanA, oceanA, blueA, violetA, magentaA, raspberryA};

//holds the current value of every led
int redVal[7];
int blueVal[7];
int greenVal[7];

//holds the Previous value of every led
int redPrev[7];
int bluePrev[7];
int greenPrev[7];

//pins for all the different Buttons and switches
//buttons work the same as switchs(when hit will stay on until off)
//this is determined by the type of button and not the code!!!
int button1 = 8; // Cycle button
int button2 = 7; // Red button
int button3 = 6; // Blue button
int button4 = 5; // Green button
int switch1 = 4; // Brightness adjust switch
int switch2 = 2; // Pulse button, adjusts the values porportionitaly(spelling:P) to the value of the XLR input, makes a rave/pulsing effect(TLC5940 takes up pin 3)

int maxBright = 4095; // TLC5940 equalvilent to a normal 255 in RGB, allows for more control
int maxSound; // value will be set to the 5v equalvilent, should never need to change

//include the TLC5940 functionality
#include <Tlc5940.h>
#include <tlc_animations.h>
#include <tlc_config.h>
#include <tlc_fades.h>
#include <tlc_progmem_utils.h>
#include <tlc_servos.h>
#include <tlc_shifts.h>

//will only run once and is first thing to run
void setup(){
  //start up the TLC5940
  Tlc.init();
  
  //function runs through every pin in arrays(0-7)
  //sets that pin as output, and sets the current and
  //preious values to 0
  //pins are input by defualt so no need to set the
  //button and switch modes to input
  for(int i = 0; i < 7; i++){
    pinMode(redPins[i], OUTPUT);
    pinMode(bluePins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    
    redVal[i] = 0;
    blueVal[i] = 0;
    greenVal[i] = 0;
    
    redPrev[i] = 0;
    bluePrev[i] = 0;
    greenPrev[i] = 0;
  }
}

//main function(will run as long as arduino has power)
void loop(){
  
  if(digitalRead(switch2)){
    if(digitalRead(button2)){
      Red(true); // if both the pukse switch and red switch are on
    }
    else if(digitalRead(button3)){
      Blue(true);
    }
    else if(digitalRead(button4)){
      Green(true);
    }
    else{
      //boomBoomLightLight();//defualt is to cycle through the colors while pulsing
    }
  }
  else if(digitalRead(button1)){
    cycle();
  }
  else if(digitalRead(button2)){
    Red(false);
  }
  else if(digitalRead(button3)){
    Blue(false);
  }
  else if(digitalRead(button4)){
    Green(false);
  }
}

int calculateStep(int prevVal, int endVal){
  
  int Step = endVal - prevVal;//What is the overall gap?
  if(Step){                   //if its non-zero...
    Step = 1052/Step;         //divide by 1052 (255*4)
  }
  return Step;
}

int calculateVal(int Step, int val, int i){
  if((Step) && i % Step == 0){//If Step is non-zero and its time to change a value,
    if(Step > 0){             //  increment the value if positive
      val+=1;
    }
    else if(Step < 0){       //  decrement the value if negative
       val-=1;
    }
  }
  //Defensive driving: make sure val stays in the range 0-255
  if(val > 255){
    val = 255;
  }
  else if(val < 0){
    val = 0;
  }
  return val;
}

void Red(boolean pulse){
  int R = 255;
  int G = 0;
  int B = 0;
  int j = 0; // counter for loop
  double pp;
  int stepR[7];
  int stepG[7];
  int stepB[7];
  int currentR;
  int currentG;
  int currentB;
  int Bval;
  int Rval;
  int Gval;
  
  for(int i = 0; i < 7; i++){
    stepR[i] = calculateStep(redPrev[i], R);
    stepG[i] = calculateStep(greenPrev[i], G);
    stepB[i] = calculateStep(bluePrev[i], B);
  }
  
  while(digitalRead(button2) && j< 1052){
    
    Tlc.clear();
    
    for(int i = 0; i < 7; i++){
      redVal[i] = calculateVal(stepR[i], redVal[i], R);
      blueVal[i] = calculateVal(stepB[i], blueVal[i], B);
      greenVal[i] = calculateVal(stepG[i], greenVal[i], G);
      
      //change to the TLC5940 scale of 0-4095 or to maxBright
       Rval = map(redVal[i], 0, 255, 0, maxBright);
       Bval = map(blueVal[i], 0, 255, 0, maxBright);
       Gval = map(greenVal[i], 0, 255, 0, maxBright);
      
      if(digitalRead(switch2) != pulse){
        pulse = !pulse;
      }
      if(pulse){
        pp = analogRead(A0);
        pp = pp/maxSound;
      }
      else{
        pp = 1;
      }
      
      Rval = Rval * pp;
      Bval = Bval * pp;
      Gval = Gval * pp;
      
      currentR = redPins[i];
      currentG = greenPins[i];
      currentB = bluePins[i];
      
      
      Tlc.set(currentR, Rval);
      Tlc.set(currentG, Gval);
      Tlc.set(currentB, Bval);
    }
    j++;
    Tlc.update();
    brightAdjust();
  }
}
void Green(boolean pulse){
  int R = 0;
  int G = 255;
  int B = 0;
  int j = 0; // counter for loop
  double pp;
  int stepR[7];
  int stepG[7];
  int stepB[7];
  int currentR;
  int currentG;
  int currentB;
  int Bval;
  int Rval;
  int Gval;
  
  for(int i = 0; i < 7; i++){
    stepR[i] = calculateStep(redPrev[i], R);
    stepG[i] = calculateStep(greenPrev[i], G);
    stepB[i] = calculateStep(bluePrev[i], B);
  }
  
  while(digitalRead(button2) && j< 1052){
    
    Tlc.clear();
    
    for(int i = 0; i < 7; i++){
      redVal[i] = calculateVal(stepR[i], redVal[i], R);
      blueVal[i] = calculateVal(stepB[i], blueVal[i], B);
      greenVal[i] = calculateVal(stepG[i], greenVal[i], G);
      
      //change to the TLC5940 scale of 0-4095 or to maxBright
       Rval = map(redVal[i], 0, 255, 0, maxBright);
       Bval = map(blueVal[i], 0, 255, 0, maxBright);
       Gval = map(greenVal[i], 0, 255, 0, maxBright);
      
      if(digitalRead(switch2) != pulse){
        pulse = !pulse;
      }
      if(pulse){
        pp = analogRead(A0);
        pp = pp/maxSound;
      }
      else{
        pp = 1;
      }
      
      Rval = Rval * pp;
      Bval = Bval * pp;
      Gval = Gval * pp;
      
      currentR = redPins[i];
      currentG = greenPins[i];
      currentB = bluePins[i];
      
      
      Tlc.set(currentR, Rval);
      Tlc.set(currentG, Gval);
      Tlc.set(currentB, Bval);
    }
    j++;
    Tlc.update();
    brightAdjust();
  }
}
void Blue(boolean pulse){
  int R = 0;
  int G = 0;
  int B = 255;
  int j = 0; // counter for loop
  double pp;
  int stepR[7];
  int stepG[7];
  int stepB[7];
  int currentR;
  int currentG;
  int currentB;
  int Bval;
  int Rval;
  int Gval;
  
  for(int i = 0; i < 7; i++){
    stepR[i] = calculateStep(redPrev[i], R);
    stepG[i] = calculateStep(greenPrev[i], G);
    stepB[i] = calculateStep(bluePrev[i], B);
  }
  
  while(digitalRead(button2) && j< 1052){
    
    Tlc.clear();
    
    for(int i = 0; i < 7; i++){
      redVal[i] = calculateVal(stepR[i], redVal[i], R);
      blueVal[i] = calculateVal(stepB[i], blueVal[i], B);
      greenVal[i] = calculateVal(stepG[i], greenVal[i], G);
      
      //change to the TLC5940 scale of 0-4095 or to maxBright
       Rval = map(redVal[i], 0, 255, 0, maxBright);
       Bval = map(blueVal[i], 0, 255, 0, maxBright);
       Gval = map(greenVal[i], 0, 255, 0, maxBright);
      
      if(digitalRead(switch2) != pulse){
        pulse = !pulse;
      }
      if(pulse){
        pp = analogRead(A0);
        pp = pp/maxSound;
      }
      else{
        pp = 1;
      }
      
      Rval = Rval * pp;
      Bval = Bval * pp;
      Gval = Gval * pp;
      
      currentR = redPins[i];
      currentG = greenPins[i];
      currentB = bluePins[i];
      
      
      Tlc.set(currentR, Rval);
      Tlc.set(currentG, Gval);
      Tlc.set(currentB, Bval);
    }
    j++;
    Tlc.update();
    brightAdjust();
  }
}
void cycle(){
  int j = 0;
  while(digitalRead(button1)==HIGH){
    anyInput(colour[j][0], colour[j][1], colour[j][2]);
    j++;
    if(j > 11){
      j = 0;
    }
  }
}

void anyInput(int R, int G, int B){
  int j = 0; // counter for loop
  boolean pulse = false;
  double pp;
  int stepR[7];
  int stepG[7];
  int stepB[7];
  int currentR;
  int currentG;
  int currentB;
  int Bval;
  int Rval;
  int Gval;
  
  for(int i = 0; i < 7; i++){
    stepR[i] = calculateStep(redPrev[i], R);
    stepG[i] = calculateStep(greenPrev[i], G);
    stepB[i] = calculateStep(bluePrev[i], B);
  }
  
  while(digitalRead(button2) && j< 1052){
    
    Tlc.clear();
    
    for(int i = 0; i < 7; i++){
      redVal[i] = calculateVal(stepR[i], redVal[i], R);
      blueVal[i] = calculateVal(stepB[i], blueVal[i], B);
      greenVal[i] = calculateVal(stepG[i], greenVal[i], G);
      
      //change to the TLC5940 scale of 0-4095 or to maxBright
       Rval = map(redVal[i], 0, 255, 0, maxBright);
       Bval = map(blueVal[i], 0, 255, 0, maxBright);
       Gval = map(greenVal[i], 0, 255, 0, maxBright);
      
      if(digitalRead(switch2) != pulse){
        pulse = !pulse;
      }
      if(pulse){
        pp = analogRead(A0);
        pp = pp/maxSound;
      }
      else{
        pp = 1;
      }
      
      Rval = Rval * pp;
      Bval = Bval * pp;
      Gval = Gval * pp;
      
      
      currentR = redPins[i];
      currentG = greenPins[i];
      currentB = bluePins[i];
      
      
      Tlc.set(currentR, Rval);
      Tlc.set(currentG, Gval);
      Tlc.set(currentB, Bval);
    }
    j++;
    Tlc.update();
    brightAdjust();
  }
}

void brightAdjust(){
  int j = 0;
  while(digitalRead(switch1)){
    
    boolean cc = false;
    if(digitalRead(button1)){
      maxBright -= 410;
    }
    else if(digitalRead(button4)){
      maxBright += 410;
    }
    
    if(maxBright > 4095){
      maxBright = 4095;
    }
    else if(maxBright < 0){
      maxBright = 0;
    }
    
    if(digitalRead(button2)){
      anyInput(colour[j][0], colour[j][1], colour[j][2]);
    }
    if(!cc){
      for(int i = 0; i < 7; i++){
        anyInput(redVal[i], greenVal[i], blueVal[i]);
      }
    }
    cc = false;
    j++;
    if(j > 11){
      j = 0;
    }
    delay(2000);
    
  }
}

void boomBoomLightLight(){
  
  int j = 0; // counter for loop
  boolean pulse = false;
  double pp;
  int stepR[7];
  int stepG[7];
  int stepB[7];
  int randColour[7];
  
  for(int i = 0; i < 7; i++){
    randColour[i] = random(0, 11);
  }
  
  for(int i = 0; i < 7; i++){
    stepR[i] = calculateStep(redPrev[i], colour[randColour[i]][0]);
    stepG[i] = calculateStep(greenPrev[i], colour[randColour[i]][1]);
    stepB[i] = calculateStep(bluePrev[i], colour[randColour[i]][2]);
  }
  
  while(digitalRead(switch2) && j < 4095){
    Tlc.clear();
    
    for(int i = 0; i < 7; i++){
      theSlave(colour[randColour[i]][0], colour[randColour[i]][1], colour[randColour[i]][2], i, stepR[i], stepB[i], stepG[i]);
    }
    j++;
    Tlc.update();
    brightAdjust();
  }
}

void theSlave(int R, int G, int B, int i, int stepR, int stepB, int stepG){
     int currentR;
     int currentG;
     int currentB;
     int Bval;
     int Rval;
     int Gval;
     double pp;
  
     redVal[i] = calculateVal(stepR, redVal[i], R);
     blueVal[i] = calculateVal(stepB, blueVal[i], B);
     greenVal[i] = calculateVal(stepG, greenVal[i], G);
      
     //change to the TLC5940 scale of 0-4095 or to maxBright
     Rval = map(redVal[i], 0, 255, 0, maxBright);
     Bval = map(blueVal[i], 0, 255, 0, maxBright);
     Gval = map(greenVal[i], 0, 255, 0, maxBright);
    
     pp = analogRead(A0);
     pp = pp/maxSound;
     
     Rval = Rval * pp;
     Bval = Bval * pp;
     Gval = Gval * pp;
      
     currentR = redPins[i];
     currentG = greenPins[i];
     currentB = bluePins[i];
       
       
     Tlc.set(currentR, Rval);
     Tlc.set(currentG, Gval);
     Tlc.set(currentB, Bval);
}
