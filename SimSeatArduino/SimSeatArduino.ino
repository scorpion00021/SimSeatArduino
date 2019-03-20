#include <Adafruit_NeoPixel.h>

//#define BUTTONNEXT_PIN   13  //debug bin
#define BUTTONNEXT_PIN   11
#define BUTTONPREV_PIN   10

#define BUTTONSMOKE_PIN 9
#define BUTTONSTREAK_PIN 8

#define PIXEL_PIN    8    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 183

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool oldState_BUTTONNEXT = HIGH;
bool oldState_BUTTONPREV = HIGH;
//bool oldState_BUTTONSMOKE = HIGH;
//bool oldState_BUTTONSTREAK = HIGH;
int brightnessSeed = 40;  //seed for limiting brightness of certain effects -- 255 MAX
int showType = 0;  //initialize starting show
int showOverride = 0; //overrides the show (for smoke and fire)
int maxShows = 7;  //index of highest show
int buttonChangeFlagged = 0;

//stored progress integers for light patterns
int prog_intakeRing = 0;
int prog_underIntakeBeacon = 0;
int prog_frontForwardStrips = 0; //wheel well
int prog_leftSidePanel = 0;
int prog_rightSidePanel = 0;
int prog_innerBodyStrips = 0;
int prog_tailRings = 0;

//ranges for various areas of the bike
const int intakeRing_min = 142;
const int intakeRing_max = 158;
const int underIntakeBeacon_min = 159;
const int underIntakeBeacon_max = 166;
const int frontForwardStrips_min = 167;
const int frontForwardStrips_max = 182;
const int leftSidePanel_min = 0;
const int leftSidePanel_max = 30;
const int rightSidePanel_min = 31;
const int rightSidePanel_max = 61;
const int innerBodyStrips_min = 62;
const int innerBodyStrips_max = 109;
const int tailRings_min = 110;
const int tailRings_max = 141;

void setup() {  
  Serial.begin(9600);
  
  pinMode(BUTTONNEXT_PIN, INPUT);
  pinMode(BUTTONPREV_PIN, INPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  //Serial.println(oldState_BUTTONNEXT);
  //Serial.println(oldState_BUTTONPREV);
  updateButtonState(BUTTONNEXT_PIN, oldState_BUTTONNEXT, 1);
  updateButtonState(BUTTONPREV_PIN, oldState_BUTTONPREV, 2);
  updateButtonHeld(BUTTONSMOKE_PIN, 3);
  updateButtonHeld(BUTTONSTREAK_PIN, 4);

  handleButtonFlags();
  
  colorFill(20, strip.Color(0, 0, 0), 0, strip.numPixels());  //clears all lights from last loop before writing a new sequence set.
  
  if(showOverride == 0){
    startShow(showType);
  }else if(showOverride == 1){ //smoke
    startShow(50);
  }else if(showOverride == 2){ //fire
    startShow(51);
  }
  strip.show(); //pushes colors to leds.  ONLY USE THIS HERE!!!
}

void updateButtonState(int pin, bool &oldState, int changeFlagged){
  // Get current button state.
  bool newState = digitalRead(pin);
  
  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    //delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(pin);
    if (newState == LOW) {
      buttonChangeFlagged = changeFlagged;
    }
  }
  // Set the last button state to the old state.
  oldState = newState;
}

void updateButtonHeld(int pin, int changeFlagged){
  bool newState = digitalRead(pin);
  if (newState == HIGH){
    buttonChangeFlagged = changeFlagged;
  }
}

void handleButtonFlags(){
  showOverride = 0;
  if(buttonChangeFlagged == 1){
    //Serial.println("NEXT");
    showType++;
      if (showType > maxShows){
        showType=0;
    }
  }
  else if(buttonChangeFlagged == 2){
    //Serial.println("PREV");
    
    if (showType == 0){
      showType=maxShows;     
    }else{
      showType -= 1;
    }
   }else if(buttonChangeFlagged == 3){
     showOverride = 1;
   }else if(buttonChangeFlagged == 4){
     showOverride = 2;  
   }
  buttonChangeFlagged = 0;
}

void startShow(int i) {
  switch(i){
    case 0: colorFill(20, strip.Color(0, 0, 0), 0, strip.numPixels());    // Black/off
            break;
    case 1: colorFill(20, strip.Color(brightnessSeed, 0, 0), 0, strip.numPixels());  // Red
            break;
    case 2: colorFill(20, strip.Color(0, brightnessSeed, 0), 0, strip.numPixels());  // Green
            break;
    case 3: colorFill(20, strip.Color(0, 0, brightnessSeed), 0, strip.numPixels());  // Blue
            break;
    case 4: rainbowCycle(10, prog_innerBodyStrips, 0, strip.numPixels()); // Rainbow Cycle, full body
            break;
    case 5: randSpasm(0, strip.numPixels()); //entire body
            break;
    case 6: policeLights(prog_innerBodyStrips, 0, strip.numPixels());  //police lights, full body
            break;         
    case 7: /*delay(20);
            colorFill(0, strip.Color(255, 255, 255), intakeRing_min, intakeRing_max);
            pulseStrobe(80, 27, 0, frontForwardStrips_min, frontForwardStrips_max, prog_frontForwardStrips);
            pulseStrobe(80, 0, 0, leftSidePanel_min, leftSidePanel_max, prog_leftSidePanel);
            pulseStrobe(0, 80, 0, rightSidePanel_min, rightSidePanel_max, prog_rightSidePanel);
            beacon(0, prog_underIntakeBeacon, strip.Color(40, 40, 0), underIntakeBeacon_min, underIntakeBeacon_max);
            rainbowCycle(0, prog_innerBodyStrips, innerBodyStrips_min, innerBodyStrips_max);
            colorFill(0, strip.Color(80, 0, 0), tailRings_min, tailRings_max);
            break;*/
            //progressWheel(20, prog_innerBodyStrips, strip.Color(0, brightnessSeed, brightnessSeed), 0, 23);
            fourPointActive(200, prog_innerBodyStrips, strip.Color(0, brightnessSeed, 0), 0, 23);
            break;
    /*case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;*/
    case 50: strobe(0, 100, 0, strip.numPixels(), prog_innerBodyStrips);
             break; 
    case 51: //colorFill(20, strip.Color(brightnessSeed, brightnessSeed/3, 0), 0, strip.numPixels());  // Orange
             fireEffect(prog_innerBodyStrips, 0, strip.numPixels());  //cool fire effect :)
             break;
  }
}


/*void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}*/

void colorFill(int wait, uint32_t c, int minValue, int maxValue) {
  for(uint16_t i=minValue; i<=maxValue; i++) {
      strip.setPixelColor(i, c);
  }
  delay(wait); 
}

void rainbowCycle(uint8_t wait, int &progress, int minVal, int maxVal) {
  uint16_t i, j;
  if(progress > 255){
    progress = 0;
  }
  for(i=minVal; i<= maxVal; i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / maxVal) + progress) & 255));
  }
  progress++;
  delay(wait);
}

void policeLights(int &progress, int minVal, int maxVal){
  int pause1 = 0;
  int pause2 = 80;
  
  switch(progress){
  case 0: colorFill(0, strip.Color(0, 0, brightnessSeed), minVal, maxVal);
          delay(pause1);
          progress++;
          break;
  case 1: colorFill(0, strip.Color(0, 0, 0), minVal, maxVal);
          delay(pause2);
          progress++;
          break;
  case 2: colorFill(0, strip.Color(0, 0, brightnessSeed), minVal, maxVal);
          delay(pause1);
          progress++;
          break;
  case 3: colorFill(0, strip.Color(0, 0, 0), minVal, maxVal);
          delay(pause2);
          progress++;
          break;
  case 4: colorFill(0, strip.Color(0, 0, brightnessSeed), minVal, maxVal);
          delay(pause1);
          progress++;
          break;
  case 5: colorFill(0, strip.Color(0, 0, 0), minVal, maxVal);
          delay(pause2);
          progress++;
          break;
  case 6: colorFill(0, strip.Color(brightnessSeed, brightnessSeed, brightnessSeed), minVal, maxVal);
          delay(pause1);
          progress++;
          break;
  case 7: colorFill(0, strip.Color(0, 0, 0), minVal, maxVal);
          delay(pause2);
          progress++;
          break;
  case 8: colorFill(0, strip.Color(brightnessSeed, 0, 0), minVal, maxVal);
          delay(pause1);  
          progress++;
          break;
  case 9: colorFill(0, strip.Color(0, 0, 0), minVal, maxVal);
          delay(pause2);   
          progress++;
          break;
  default: progress = 0;
  }
}

void strobe(int flashtime, int blacktime, int minVal, int maxVal, int &progress){
  if (progress == 0){
    colorFill(100, strip.Color(255, 255, 255), minVal, maxVal);
    progress++;
  }else{
    colorFill(0, strip.Color(0, 0, 0), minVal, maxVal);
    progress = 0;
  }
  
}

void pulseStrobe(int r, int g, int b, int minVal, int maxVal, int &progress){
  //BRIGHTEST VALUE TAKEN AS COLOR
  //int pulseSpeed = 1;
  
  if(progress>41){
    progress = 0;
  }
  if(progress<20){
    //colorFill(0, strip.Color(r/(progress/20), g/(progress/20), b/(progress/20)), minVal, maxVal);
    colorFill(0, strip.Color(r/(progress+1), g/(progress+1), b/(progress+1)), minVal, maxVal);
  }else if(progress < 40){
    colorFill(0, strip.Color(r/(40-progress), g/(40-progress), b/(40-progress)), minVal, maxVal);
    }else if(progress==41){
    colorFill(0, strip.Color(255, 255, 255), minVal, maxVal);
  }
  progress++;
}

void beacon(int wait, int &progress, uint32_t c, int minVal, int maxVal){
  delay(wait);
  if (minVal + progress > maxVal){
    progress = 0; 
  }
  strip.setPixelColor(minVal + progress, c);
  progress++; 
}

void progressWheel(int wait, int &progress, uint32_t c, int minVal, int maxVal){
  delay(wait);
  if (minVal + progress > maxVal){
    progress = 0; 
  }
  for(int i=minVal; i<= progress; i++){
      strip.setPixelColor(minVal + i, c); 
  }
  progress++; 
}

void fourPointActive(int wait, int &progress, uint32_t c, int minVal, int maxVal){
  int separationVal = (maxVal - minVal + 1) / 4;
  delay(wait);
  if ((minVal + progress) * 4 >= maxVal){
    progress = 0; 
  }
  strip.setPixelColor(minVal + progress, c);
  strip.setPixelColor(minVal + progress + separationVal, c);
  strip.setPixelColor(minVal + progress + (2 * separationVal), c);
  strip.setPixelColor(minVal + progress + (3 * separationVal), c);
  //strip.setPixelColor(minVal + progress + 6, c);
  //strip.setPixelColor(minVal + progress + 12, c);
  //strip.setPixelColor(minVal + progress +18, c);
  progress++; 
}

void fireEffect(int &progress, int minVal, int maxVal){
  //uint32_t color = strip.Color(5, 5, 5);
  //uint32_t orange = strip.Color(0, 0, 0);
  int i = minVal;
  bool colorFlag = false;
  int intensity1 = randNum(brightnessSeed/1.5, brightnessSeed);
  while (i <= maxVal){
    int sectionLength = randNum(0, 7);     
      if(colorFlag){
        colorFlag = false;
        for(int j=0; j<=sectionLength; j++){
          int intensity2 = randNum(1, intensity1);
          strip.setPixelColor(j+i, strip.Color(intensity2, intensity2/3, 0));
        }
      }else{
        if(sectionLength == 2){
          for(int j=0; j<=sectionLength; j++){
          colorFlag = true;
          strip.setPixelColor(j+i, strip.Color(0, 0, 0));
        }
        }else{
        int tempred = randNum(1, brightnessSeed);
        uint32_t tempcolor = strip.Color(tempred, randNum(1, tempred/3), 0);
        for(int j=0; j<=sectionLength; j++){
          colorFlag = true;
          strip.setPixelColor(j+i, tempcolor);
        }
        }
      } 
      i+=sectionLength;
    }
    delay(0);
  }

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

uint32_t Wheel(byte WheelPos) { //Needed for 'Rainbow' effects
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void randSpasm(int minVal, int maxVal){
  uint32_t color = strip.Color(5, 5, 5);
  uint32_t black = strip.Color(0, 0, 0);
  int i = minVal;
  bool colorFlag = false;
  int intensity1 = randNum(1, brightnessSeed);
  while (i <= maxVal){
    int sectionLength = randNum(0, 7);     
      if(colorFlag){
        colorFlag = false;
        for(int j=0; j<=sectionLength; j++){
          int intensity2 = randNum(1, intensity1);
          strip.setPixelColor(j+i, strip.Color(intensity2, intensity2, intensity2));
        }
      }else{
        for(int j=0; j<=sectionLength; j++){
        colorFlag = true;
        strip.setPixelColor(j+i, black);
        }
      } 
      i+=sectionLength;
    }
    //strip.show();
    delay(randNum(20, 100));
  }


int randNum(int minimum, int maximum){
  return (int)random(minimum, maximum);
  //return (int)1;
}
