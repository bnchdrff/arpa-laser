#include <Stepper.h>
#include <MIDI.h>
#include <Note.h>

#define DEBUG_MODE false

/*----- Stepper-related Constants -----*/
#define STEPS_FOR_FULL_ROTATION 48
#define STEP_DISTANCE 1
#define STEPPER_SPEED 1000

/*----- Laser-related Constants -----*/
#define LASER_DELAY 18
// If this is too small, the stepper will freak out.
#define LASER_SHORTSTOP 4
#define LASER_PIN 7

#define LASER_THRESHOLD 65

/*----- Note-related Constants -----*/
#define TOTAL_NOTES 5
#define MIDI_BAUD 31250
#define DEBUG_BAUD 9600

/*----- Declare objects -----*/
Stepper stepper(STEPS_FOR_FULL_ROTATION, 8, 9, 10, 11);
MIDI midi;
Note note(NoteName::FSharp);

/*---- Setup Methods ----*/
void _setupLaserPin()
{
  pinMode(LASER_PIN, OUTPUT);
}

/*---- Utility Methods ----*/
void turnLaserON()
{
  delay(LASER_SHORTSTOP);
  digitalWrite(LASER_PIN, HIGH);
  delay(LASER_DELAY);
}

void turnLaserOFF()
{
  digitalWrite(LASER_PIN, LOW);
  delay(LASER_SHORTSTOP);
}

void printSensorValueToConsoleIfInDebug()
{
  if (DEBUG_MODE) {
    Serial.println(analogRead(A0));
  }
}

/*---- Overridden Methods ----*/
void setup()
{
  _setupLaserPin();
  stepper.setSpeed(STEPPER_SPEED);

  if (DEBUG_MODE) {
    midi.connectWithMode(MIDIConnectionMode::Debug);
  } else {
    midi.connectWithMode(MIDIConnectionMode::Play);
  }
}

bool playing1, playing2, playing3, playing4, playing5 = false;

void setPlaying(int noteNumber, bool playing)
{
  if (noteNumber == 1) {
    playing1 = playing;
  }
  else if (noteNumber == 2) {
    playing2 = playing;
  }
  else if (noteNumber == 3) {
    playing3 = playing;
  }
  else if (noteNumber == 4) {
    playing4 = playing;
  }
  else if (noteNumber == 5) {
    playing5 = playing;
  }
}

int isPlaying(int noteNumber)
{
  if (noteNumber == 1) {
    return playing1;
  }
  else if (noteNumber == 2) {
    return playing2;
  }
  else if (noteNumber == 3) {
    return playing3;
  }
  else if (noteNumber == 4) {
    return playing4;
  }
  else if (noteNumber == 5) {
    return playing5;
  }
  return false;
}

void loop()
{
  Note majorNoteNames[] = {C, DSharp, F, G, ASharp};

  int majorOctaves[] {4, 4, 4, 4, 4};
  
  // Reduce blur on the beginning note.
  delay(LASER_SHORTSTOP);

  for (int noteIndex = 0; noteIndex < TOTAL_NOTES; ++noteIndex)
  {
    turnLaserON();
    // read photoresistor value and play or turn note off

    delay(LASER_DELAY);
    int light = analogRead(0);
    printSensorValueToConsoleIfInDebug();

    Note note(majorNoteNames[noteIndex]);
    int octave = majorOctaves[noteIndex];
    
    if (light > LASER_THRESHOLD) {
      if (!isPlaying(noteIndex + 1)) {
        setPlaying(noteIndex + 1, true);
        midi.playNote(note, octave); 
      }
    }
    else {
      setPlaying(noteIndex + 1, false);
      midi.stopPlayingNote(note, octave);
    }

    turnLaserOFF();
    delay(2);
    stepper.step(STEP_DISTANCE);
    delay(4);
  }

  // Don't draw lasers on the downstroke, to reduce blur.
  for (int noteIndex = TOTAL_NOTES; noteIndex > 0; --noteIndex)
  {
    stepper.step(-STEP_DISTANCE);
    delay(LASER_SHORTSTOP);
  }
}
