/*    
    Mozzi documentation/API
    https://sensorium.github.io/Mozzi/doc/html/index.html

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
    Tim Barrass 2013, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include "pitches.h"
#include <Oscil.h> // oscillator template

// Import the three waveforms we want: sine, saw, triangle
#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/triangle2048_int8.h> 


#include <RollingAverage.h>
#include <ControlDelay.h>
#include <Line.h>

#define INPUT_PIN_GAIN A0 // analog control input (up/down of joystick)
#define INPUT_PIN_FREQ A1 // analog control input (left/right of joystick)

#define INPUT_ECHO_CELL_1 A0 // reused for a different mode
#define INPUT_ECHO_CELL_2 A1 // resued for a different mode

#define MODE_BTN 13
#define EFFECT_BTN 11 // toggle audio effects
#define WAVEFORM_BTN 12 // toggle waveform

#define CONTROL_RATE 64

int btns[] = {2, 3, 4, 5, 6, 7}; // Digital buttons for playing notes
int btns_states[] = {LOW, LOW, LOW, LOW, LOW}; 

// Define all the scales you want to play here -- currently capped at 6 because of limited buttons
int penta_minor[][6] = {
  {NOTE_B3, NOTE_D4, NOTE_E4, NOTE_FS4, NOTE_A4, NOTE_B4}, // A minor pentatonic
  {NOTE_AS3, NOTE_CS4, NOTE_DS4, NOTE_F4, NOTE_GS4, NOTE_AS4}, // A# minor pentatonic
  {NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_AS4, NOTE_C5}, // C minor pentatonic
  {NOTE_D4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_C5, NOTE_D5}, //  D minor pentatonic
  {NOTE_E4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_D5, NOTE_E5}, // E minor pentatonic
  {NOTE_FS4, NOTE_A4, NOTE_B4, NOTE_CS5, NOTE_E5, NOTE_FS5}, // F# minor pentatonic
};

// These are the notes associated with each of the 6 keys
// We'll initialize them to the A minor pentatonic
int btns_notes[6] = {
  penta_minor[0][0],
  penta_minor[0][1],
  penta_minor[0][2],
  penta_minor[0][3],
  penta_minor[0][4],
  penta_minor[0][5]
  };

int notes_2_play[] = {0, 0, 0, 0, 0, 0}; // polyphonic mode
int mono_note = 0; // monophonic mode

/*
 * ECHO?
 * 
 */
unsigned int echo_cells_1 = 32;
unsigned int echo_cells_2 = 64;
unsigned int echo_cells_3 = 128;

ControlDelay <128, int> kDelay; // 2seconds
// Echoey oscillators
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin6(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin7(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin8(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin9(SIN2048_DATA);

// Echoey oscillators
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw6(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw7(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw8(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw9(SAW2048_DATA);

/*
 * TREMOLO?
 * 
 */
Line <unsigned int> aGain;
Oscil<SIN2048_NUM_CELLS, CONTROL_RATE> kTremelo(SIN2048_DATA);

// Sine oscillators set by buttons
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin0(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin1(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin2(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin3(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin4(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin5(SIN2048_DATA);

// Saw oscillators set by buttons
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw0(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw1(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw2(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw3(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw4(SAW2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw5(SAW2048_DATA);

// Triangle oscillators set by buttons
Oscil <TRIANGLE2048_NUM_CELLS, AUDIO_RATE> aTri0(TRIANGLE2048_DATA);
Oscil <TRIANGLE2048_NUM_CELLS, AUDIO_RATE> aTri1(TRIANGLE2048_DATA);
Oscil <TRIANGLE2048_NUM_CELLS, AUDIO_RATE> aTri2(TRIANGLE2048_DATA);
Oscil <TRIANGLE2048_NUM_CELLS, AUDIO_RATE> aTri3(TRIANGLE2048_DATA);
Oscil <TRIANGLE2048_NUM_CELLS, AUDIO_RATE> aTri4(TRIANGLE2048_DATA);
Oscil <TRIANGLE2048_NUM_CELLS, AUDIO_RATE> aTri5(TRIANGLE2048_DATA);

// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 32> kAverage; // how_many_to_average has to be power of 2
int averaged_f;

boolean saw = false;
boolean sine = false;
boolean tri = false;

int effect = 0;

void setup(){
  Serial.begin(115200);
  kTremelo.setFreq(5.0f);
  kDelay.set(echo_cells_1);
  startMozzi(CONTROL_RATE);
  sine = true;
}

int mode = 0;
int effects = 0;
int last_effect_btn_state = 0;
int last_wave_btn_state = 0;

boolean no_press = true;

// for debouncing mode changing buttons
long time_now = 0;
long mode_btn_time = 0;
long wave_btn_time = 0;
long debounce = 1000000;


void updateControl(){
  no_press = true;

  time_now = mozziMicros();

  /**
   * SWITCH WAVEFORM IF BUTTON IS PRESSED
   */
  int wave_btn_state = digitalRead(WAVEFORM_BTN);
  if (wave_btn_state && !last_wave_btn_state && (time_now - wave_btn_time > debounce)) {
    if (sine) {
      sine = false;
      tri = true;
    }else if (tri) {
      tri = false;
      saw = true;
    }else if (saw) {
      saw = false;
      sine = true;
    }
    last_wave_btn_state = wave_btn_state;
    wave_btn_time = time_now;
  }else {
    last_wave_btn_state = 0;
  }


  /**
   * SWITCH EFFECT MODE IF BUTTON IS PRESSED 
   */
  int effect_btn_state = digitalRead(EFFECT_BTN);
  if (effect_btn_state && !last_effect_btn_state && (time_now - mode_btn_time > debounce))
  {
    if (effects == 0) {
      effects = 1;
    }else if (effects == 1) { // tremolo
      effects = 2; 
    }else if (effects == 2) {
      effects = 0;
    } // add states as necessary for additional effects
    last_effect_btn_state = effect_btn_state;
    mode_btn_time = time_now;
  }else {
    last_effect_btn_state = 0;
  }

  /**<
   * CHANGE SCALES IF JOYSTICK IS PRESSED IN
   */
  mode = digitalRead(MODE_BTN);
  if (mode == LOW) // LOW means joystick is pushed in
  {
    Serial.println("Updating scale...");
    // Find which key to switch to
    for (int i = 0; i < 6; ++i)
    {
        btns_states[i] = digitalRead(btns[i]);
        if (btns_states[i] == HIGH)
        {
          btns_notes[0] = penta_minor[i][0];
          btns_notes[1] = penta_minor[i][1];
          btns_notes[2] = penta_minor[i][2];
          btns_notes[3] = penta_minor[i][3];
          btns_notes[4] = penta_minor[i][4];
          btns_notes[5] = penta_minor[i][5];
          // Perform no further updates this control cycle
          return;
        }
      }
    }
    

    /**
     * EFFECT 1: TREMOLO
     * If tremolo: find the Low Frequency Oscillator (LFO) rate from the joystick and set it
     */
    if (effects == 1) {
      int lfo_Freq = mozziAnalogRead(INPUT_PIN_FREQ);
      int gainIn = mozziAnalogRead(INPUT_PIN_GAIN);
      
      averaged_f = map(kAverage.next(lfo_Freq), 0, 1023, 0, 10);
      kTremelo.setFreq(averaged_f);
      
      unsigned int gain = (128u + kTremelo.next())<<8;
      aGain.set(gain, AUDIO_RATE / CONTROL_RATE);
    }

  
    /*
     * DETERMINE THE PRESSED NOTE KEYS
     */
     mono_note = 0;
     
      for (int i = 0; i < 6; ++i) {
        btns_states[i] = digitalRead(btns[i]);
        if (btns_states[i] == HIGH)
        {
            no_press = false;
            if (mono_note == 0) {
              mono_note = btns_notes[i];
            }
            notes_2_play[i] = btns_notes[i];
        }else {
          notes_2_play[i] = 0;
        }
      }


    /*
     * SET OSCILLATORS BASED ON SELECTED NOTES
     */
     
   /**
    * EFFECT 2: ECHO
    */
    if (effects == 2) {
        int analog_2_chamber = mozziAnalogRead(INPUT_ECHO_CELL_1);
        int analog_3_chamber = mozziAnalogRead(INPUT_ECHO_CELL_2);
        echo_cells_2 = (int)analog_2_chamber/8;
        echo_cells_3 = (int)analog_3_chamber/4;
        if (sine) {
          aSin6.setFreq(mono_note);
          aSin7.setFreq(kDelay.next(mono_note));
          aSin8.setFreq(kDelay.read(echo_cells_2));
          aSin9.setFreq(kDelay.read(echo_cells_3));
        } else if(saw) {
          aSaw6.setFreq(mono_note);
          aSaw7.setFreq(kDelay.next(mono_note));
          aSaw8.setFreq(kDelay.read(echo_cells_2));
          aSaw9.setFreq(kDelay.read(echo_cells_3));
        }
    } else {
      if (saw)
      {
        aSaw0.setFreq(notes_2_play[0]);
        aSaw1.setFreq(notes_2_play[1]);
        aSaw2.setFreq(notes_2_play[2]);
        aSaw3.setFreq(notes_2_play[3]);
        aSaw4.setFreq(notes_2_play[4]);
        aSaw5.setFreq(notes_2_play[5]);
      } else if (sine)
      {
        aSin0.setFreq(notes_2_play[0]);
        aSin1.setFreq(notes_2_play[1]);
        aSin2.setFreq(notes_2_play[2]);
        aSin3.setFreq(notes_2_play[3]);
        aSin4.setFreq(notes_2_play[4]);
        aSin5.setFreq(notes_2_play[5]);
      } else if (tri)
      {
        aTri0.setFreq(notes_2_play[0]);
        aTri1.setFreq(notes_2_play[1]);
        aTri2.setFreq(notes_2_play[2]);
        aTri3.setFreq(notes_2_play[3]);
        aTri4.setFreq(notes_2_play[4]);
        aTri5.setFreq(notes_2_play[5]);
      }

    }

    
    if (no_press) {
          aGain.set(0, AUDIO_RATE / CONTROL_RATE);
    }
}


int updateAudio(){
  if (saw)
  {
    if (effects == 1) {
      return ((int) // cast everything to be an int
                    (
                       ((long)(aSaw0.next() // add up all the frequencies from the keyboard
                        + aSaw1.next() 
                        + aSaw2.next() 
                        + aSaw3.next() 
                        + aSaw4.next() 
                        + aSaw5.next()) * aGain.next()) >> 10
                     )
              ) ; // shift the final result up 4 bits to get better volume
     } else if(effects == 2) {
      return 3*((int)
                    aSaw6.next()
                    +aSaw7.next()
                    +(aSaw8.next() >>1)
                    +(aSaw9.next()>>2)
                );
    } else {
        return ((int)
                        (aSaw0.next()
                        + aSaw1.next() 
                        + aSaw2.next() 
                        + aSaw3.next() 
                        + aSaw4.next() 
                        + aSaw5.next()) 
                ) << 4;
     }
  } else if (tri)
  {
    if (effects == 1) {
      return ((int) // cast everything to be an int
                    (
                       ((long)(aTri0.next() // add up all the frequencies from the keyboard
                        + aTri1.next() 
                        + aTri2.next() 
                        + aTri3.next() 
                        + aTri4.next() 
                        //+ aTri5.next()
                        ) * aGain.next()) >> 10
                     )
              ) ; // shift the final result up 4 bits to get better volume
      }else {
        return ((int)
                        (aTri0.next()
                        + aTri1.next() 
                        + aTri2.next() 
                        + aTri3.next() 
                        + aTri4.next() 
                        //+ aTri5.next()
                        ) 
                ) << 4;
      }
  } else if (sine)
  {
    if (effects == 1) {
      return ((int) // cast everything to be an int
                    (
                       ((long)(aSin0.next() // add up all the frequencies from the keyboard
                        + aSin1.next() 
                        + aSin2.next() 
                        + aSin3.next() 
                        + aSin4.next() 
                        + aSin5.next()) * aGain.next()) >> 12
                     )
              ) ; // shift the final result up 4 bits to get better volume
    } else if(effects == 2) {
      return 3*((int)
                    aSin6.next()
                    +aSin7.next()
                    +(aSin8.next() >>1)
                    +(aSin9.next()>>2)
                );
    } else {
        return ((int)
                      (aSin0.next() 
                        + aSin1.next() 
                        + aSin2.next() 
                        + aSin3.next() 
                        + aSin4.next() 
                        + aSin5.next()) 
                ) << 2;
    }
  }
}


void loop(){
  audioHook();
}

