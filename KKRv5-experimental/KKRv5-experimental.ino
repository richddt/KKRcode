
/*******************************************************************************

 Bare Conductive Random Touch MP3 player
 ---------------------------------------
 
 Random_Touch_MP3.ino - touch triggered MP3 playback taken randomly from microSD

 You need twelve folders named E0 to E11 in the root of the microSD card. Each 
 of these folders can have as many MP3 files in as you like, named however you 
 like (as long as they have a .mp3 file extension).

 When you touch electrode E0, a random file from the E0 folder will play. When 
 you touch electrode E1, a random file from the E1 folder will play, and so on. 
 You should note that this is not the same file structure as for Touch_MP3.

  SD card    
  │
  ├──E0
  │    some_mp3.mp3  
  │    another_mp3.mp3
  │
  ├──E1
  │    dog-barking-1.mp3
  │    dog-barking-2.mp3
  │    dog-growling.mp3
  │    dog-howling.mp3
  │
  ├──E2
  │    1.mp3
  │    2.mp3
  │    3.mp3
  │    4.mp3
  │    5.mp3
  │    6.mp3
  │
  └──...and so on for other electrodes   
 
 Based on code by Jim Lindblom and plenty of inspiration from the Freescale 
 Semiconductor datasheets and application notes.
 
 Bare Conductive code written by Stefan Dzisiewski-Smith and Peter Krige.
 
 This work is licensed under a MIT license https://opensource.org/licenses/MIT
 
 Copyright (c) 2016, Bare Conductive
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

*******************************************************************************/

// compiler error handling
//#include "Compiler_Errors.h"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

// mp3 includes
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <SFEMP3Shield.h>

// mp3 variables
SFEMP3Shield MP3player;
byte result;
int lastPlayed = 0;
int makeyPin = 10; //DDT
int makeyTouch = 0; //DDT

// sd card instantiation
SdFat sd;
SdFile file;

// define LED_BUILTIN for older versions of Arduino
#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// mp3 behaviour defines
#define REPLAY_MODE TRUE  // By default, touching an electrode repeatedly will 
                          // play the track again from the start each time.
                          //
                          // If you set this to FALSE, repeatedly touching an 
                          // electrode will stop the track if it is already 
                          // playing, or play it from the start if it

void setup(){  
  Serial.begin(192000);
  
  pinMode(LED_BUILTIN, OUTPUT);
   
  //while (!Serial) ; {} //uncomment when using the serial monitor 
  Serial.println("Bare Conductive Random Touch MP3 player");

  // initialise the Arduino pseudo-random number generator with 
  // a bit of noise for extra randomness - this is good general practice
  randomSeed(analogRead(0));

  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);


  MPR121.setTouchThreshold(5);
  MPR121.setReleaseThreshold(50);

  result = MP3player.begin();
  MP3player.setVolume(10,10);
 
  if(result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to start MP3 player");
   }
   
}

void loop(){
 //readTouchInputs();
  readMakey();


  
}


void readMakey(){

 makeyTouch = digitalRead(makeyPin);   // read the input pin
 //digitalWrite(LED_BUILTIN, makeyTouch);    // sets the LED to the button's value
////////////// if 
   if(makeyTouch){

    digitalWrite(LED_BUILTIN, HIGH);
    if(MP3player.isPlaying()){
      MP3player.stopTrack();
      Serial.println("stopping track");
      }
    else{
      //select new trakc
      playRandomTrack(2);
      
      }
   }
   else{
    MP3player.stopTrack();
    digitalWrite(LED_BUILTIN, LOW);
    }

}

/**
      for (int i=0; i < 12; i++){  // Check which electrodes were pressed
        if(makeyTouch==i){        
            //pin i was just touched
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" was just touched");
            digitalWrite(LED_BUILTIN, HIGH);
            if(MP3player.isPlaying()){
              if(lastPlayed==i && !REPLAY_MODE){
                // if we're already playing from the requested folder, stop it
                MP3player.stopTrack();
                Serial.println("stopping track");
              } else {
                // if we're already playing a different track, stop that 
                // one and play the newly requested one
                MP3player.stopTrack();
                Serial.println("stopping track");
                playRandomTrack(i);
                
                // don't forget to update lastPlayed - without it we don't
                // have a history
                lastPlayed = i;
              }
            } else {
              // if we're playing nothing, play the requested track 
              // and update lastplayed
              playRandomTrack(i);
              lastPlayed = i;
            }     
        } else {
          if(!makeyTouch){
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" is no longer being touched");
            digitalWrite(LED_BUILTIN, LOW);
          } 
        }
      }
    }
    else{
       digitalWrite(LED_BUILTIN, LOW);
      }
 ////////////////

  
}

void readTouchInputs(){
  if(MPR121.touchStatusChanged()){
    
    MPR121.updateTouchData();

    // only make an action if we have one or fewer pins touched
    // ignore multiple touches
    
    if(MPR121.getNumTouches()<=1){
      for (int i=0; i < 12; i++){  // Check which electrodes were pressed
        if(MPR121.isNewTouch(i)){        
            //pin i was just touched
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" was just touched");
            digitalWrite(LED_BUILTIN, HIGH);
            if(MP3player.isPlaying()){
              if(lastPlayed==i && !REPLAY_MODE){
                // if we're already playing from the requested folder, stop it
                MP3player.stopTrack();
                Serial.println("stopping track");
              } else {
                // if we're already playing a different track, stop that 
                // one and play the newly requested one
                MP3player.stopTrack();
                Serial.println("stopping track");
                playRandomTrack(i);
                
                // don't forget to update lastPlayed - without it we don't
                // have a history
                lastPlayed = i;
              }
            } else {
              // if we're playing nothing, play the requested track 
              // and update lastplayed
              playRandomTrack(i);
              lastPlayed = i;
            }     
        } else {
          if(MPR121.isNewRelease(i)){
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" is no longer being touched");
            digitalWrite(LED_BUILTIN, LOW);
          } 
        }
      }
    }
  }
}

void playRandomTrack(int electrode){

  // build our directory name from the electrode
  char thisFilename[14]; // allow for 8 + 1 + 3 + 1 (8.3 with terminating null char)
  // start with "E00" as a placeholder
  char thisDirname[] = "E00";

  if(electrode<10){
    // if <10, replace first digit...
    thisDirname[1] = electrode + '0';
    // ...and add a null terminating character
    thisDirname[2] = 0;
  } else {
    // otherwise replace both digits and use the null
    // implicitly created in the original declaration
    thisDirname[1] = (electrode/10) + '0';
    thisDirname[2] = (electrode%10) + '0';
  }

  sd.chdir(); // set working directory back to root (in case we were anywhere else)
  if(!sd.chdir(thisDirname)){ // select our directory
    Serial.println("error selecting directory"); // error message if reqd.
  }

  size_t filenameLen;
  char* matchPtr;
  unsigned int numMP3files = 0;

  // we're going to look for and count
  // the MP3 files in our target directory
  while (file.openNext(sd.vwd(), O_READ)) {
    file.getFilename(thisFilename);
    file.close();

    // sorry to do this all without the String object, but it was
    // causing strange lockup issues
    filenameLen = strlen(thisFilename);
    matchPtr = strstr(thisFilename, ".MP3");
    // basically, if the filename ends in .MP3, we increment our MP3 count
    if(matchPtr-thisFilename==filenameLen-4) numMP3files++;
  }

  // generate a random number, representing the file we will play
  unsigned int chosenFile = random(numMP3files);

  // loop through files again - it's repetitive, but saves
  // the RAM we would need to save all the filenames for subsequent access
  unsigned int fileCtr = 0;

  sd.chdir(); // set working directory back to root (to reset the file crawler below)
  if(!sd.chdir(thisDirname)){ // select our directory (again)
    Serial.println("error selecting directory"); // error message if reqd.
  } 

  while (file.openNext(sd.vwd(), O_READ)) {
    file.getFilename(thisFilename);
    file.close();

    filenameLen = strlen(thisFilename);
    matchPtr = strstr(thisFilename, ".MP3");
    // this time, if we find an MP3 file...
    if(matchPtr-thisFilename==filenameLen-4){
      // ...we check if it's the one we want, and if so play it...
      if(fileCtr==chosenFile){
        // this only works because we're in the correct directory
        // (via sd.chdir() and only because sd is shared with the MP3 player)
        Serial.print("playing track ");
        Serial.println(thisFilename); // should update this for long file names
        MP3player.playMP3(thisFilename);
        return;
      } else {
          // ...otherwise we increment our counter
        fileCtr++;
      }
    }
  }  
}
**/
