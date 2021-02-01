#include <Adafruit_NeoPixel.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/
#include "AudioSampleCashregister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;
AudioPlayMemory    sound2;
AudioPlayMemory    sound3;
AudioMixer4        mix;
AudioOutputI2S     headphones;
AudioOutputAnalog  dac;

// Create Audio connections between the components
//
AudioConnection c1(sound0, 0, mix, 0);
AudioConnection c2(sound1, 0, mix, 1);
AudioConnection c3(sound2, 0, mix, 2);
AudioConnection c4(sound3, 0, mix, 3);
AudioConnection c5(mix, 0, dac, 0);
AudioConnection c6(mix, 0, dac, 1);
AudioConnection c7(mix, 0, headphones, 0);
AudioConnection c8(mix, 0, headphones, 1);


// Create an object to control the audio shield.
//
AudioControlSGTL5000 audioShield;


#define NUMBEROFSTEPS 2//8
#define NUMBEROFINPUTS 4
#define LED_PIN    2
Adafruit_NeoPixel strip(NUMBEROFSTEPS, LED_PIN, NEO_GRB + NEO_KHZ800);

int _inputs[NUMBEROFSTEPS][NUMBEROFINPUTS] = {{24, 25, 26, 27}, {28, 29, 30, 31}};
uint32_t _colors[] = {strip.Color(255, 0, 0), strip.Color(0, 0, 255), strip.Color(0, 255, 0), strip.Color(255, 0, 255)};


int _values[NUMBEROFINPUTS];

int _currentStep = 0;



// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  //  for (auto i = 0; i < NUMBEROFSTEPS; i++) {
  //    for (auto j = 0; j < NUMBEROFINPUTS; j++) {
  //      pinMode(_inputs[i][j], INPUT);
  //    }
  //  }

  //  for (auto i = 0; i < NUMBEROFSTEPS; i++) {
  //    strip.setPixelColor(i, _colors[0]);
  //  }

  strip.begin();
  strip.show();
  //  strip.setBrightness(50);


  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);

  // turn on the output
  audioShield.enable();
  audioShield.volume(0.5);

  // reduce the gain on mixer channels, so more than 1
  // sound can play simultaneously without clipping
  mix.gain(0, 0.4);
  mix.gain(1, 0.4);
  mix.gain(2, 0.4);
  mix.gain(3, 0.4);

  Serial.println("setup done");

}

// the loop routine runs over and over again forever:
void loop() {
  //  return;


  strip.clear();

  for (auto i = 0; i < NUMBEROFSTEPS; i++) {
    if (i == _currentStep) {
      for (auto j = 0; j < NUMBEROFINPUTS; j++) {
        int value = digitalRead(_inputs[i][j]);
        Serial.println(String(i) + " " + String(j) + " " + value);

        if (value == 0) {
          //        Serial.println(String(i)+" " + String(j) + " " + value);
          strip.setPixelColor(i, _colors[j]);
          usbMIDI.sendNoteOn(24 + j, 127, 1);
          switch (j) {
            case 0: sound0.play(AudioSampleKick); break;
            case 1: sound1.play(AudioSampleSnare); break;
            case 2: sound2.play(AudioSampleHihat); break;
            case 3: sound3.play(AudioSampleTomtom); break;
            default: break;
          }


          delay(50);
          usbMIDI.sendNoteOff(24 + j, 0, 1);

        }
      }
    }
  }

  strip.show();

  _currentStep++;
  _currentStep = _currentStep % NUMBEROFSTEPS;
//  TODO: calculate sleep time, based on bpm
  delay(1000);
}
