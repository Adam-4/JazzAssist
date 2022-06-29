#include <Wire.h> 
#include <LiquidCrystal_I2C.h>`

LiquidCrystal_I2C lcd(0x27,20,4);  //LCD address

//remember commas. 
char *chordChanges[] = {
  "(Cn+6+9)---(Cn+6+9)---(Dn+7b5)---(Dn+7b5)---(dn+7)---(Gn+7)---(Cn+6+9)---(Cn+6+9)---(Fn+6)---(Fn+6)---(Fn+6)---(Fn+6)---(Dn+7)---(Dn+7)---(dn+7)---(Gn+7)-(Gn+7b9)-",
  "(dn+7)-(Gn+7)-(DnM7)---(a#00)---(an+7)---(Dn+7b9)---(GnM7)---(gn+6)---(F#+6)-(F#+7#5)-(Bn+9)-(Bn+7b9)-(En+4+9)-(En+9)-(Bn+9)-(An+7#5)-(dn+9)-(Gn+6)-(dn+9)---", 
  "chord 3"
};

char *songs[] = {
  "Take the 'A' Train", 
  "Wave",
  "Cantaloupe Island"
};

char *timeSignatures[] = {
  "4/4", 
  "4/4",
  "4/4"
};

char *format[] = {
  "AABA", 
  "AABA",
  "AABA"
};

char *by[] = { 
  "Duke Ellington", 
  "Antonio Jobim",
  "Herbie Hancock"
};

int lydian[] = {0,2,4,6,7,9,11,12};
int dorian[] = {0,2,3,5,7,9,10,12};
int altered[] = {0,1,3,4,6,8,10,12};
int mixolydian[] = {0,2,4,5,7,9,10,12};
int diminished[] = {0,1,3,4,6,7,9,10,12};
int whole[] = {0,2,4,6,8,10,12};
int melodicMin[] = {0,2,3,5,7,9,11,12};

String mode = "chordal";

int INbuttonPinInput = 13;
int OUTbuttonPinInput = 12;

byte dialPinInput = A6;
int interfaceZ = 0;

int INbuttonState = 0;
int OUTbuttonState = 0;

int potentialSongID = 0;
int potentialBPM = 0;

byte numsongs = (sizeof(songs) / sizeof(songs[0]));
boolean songPlaying = false;

int initialscrollerValue = 0;

void setup() {
  Serial.begin(9600);
  
  lcd.init(); // initialize the lcd 
  lcd.init();

  lcd.backlight();
  welcome();
  
  pinMode(INbuttonPinInput, INPUT_PULLUP);
  pinMode(OUTbuttonPinInput, INPUT_PULLUP);
  //dial analog pin doesn't need to be initialized. 
   
  for (int i=0;i<13;i++) {
    pinMode(26+i, OUTPUT);
  }

  
}

void displayCentered(String message, int y) {
  int strlength = message.length();
  int x = floor((21-strlength)/2);

  lcd.setCursor(x, y);
  lcd.print(message);
}

void welcome() {
  displayCentered("Jazzassist 1.0", 1);
  displayCentered("By Adam", 2);
}

void loop() {
  INbuttonState = digitalRead(INbuttonPinInput);
  
  OUTbuttonState = digitalRead(OUTbuttonPinInput);

  if (INbuttonState == HIGH) {
    interface("in");
  } else if (OUTbuttonState == HIGH) {
    interface("out");
  }

  interface("idle");
  //Serial.println(interfaceZ);
}

int rootCodeFn(String chord) {
  String root = chord.substring(0,2);
  if (root=="Cn" || root=="cn") {
    return 0;
  } else if (root =="C#" || root=="c#") {
    return 1;
  } else if (root =="Dn" || root=="dn") {
    return 2;
  } else if (root =="D#" || root=="d#") {
    return 3;
  } else if (root =="En" || root=="en") {
    return 4;
  } else if (root =="Fn" || root=="fn") {
    return 5;
  } else if (root =="F#" || root=="f#") {
    return 6;
  } else if (root =="Gn" || root=="gn") {
    return 7;
  } else if (root =="G#" || root=="g#") {
    return 8;
  } else if (root =="An" || root=="an") {
    return 9;
  } else if (root =="A#" || root=="a#") {
    return 10;
  } else if (root =="Bn" || root=="bn") {
    return 11;
  }
}

//removed [A] tag feature
void playSongChordal(int ID, float beatsperminute) {
  float delayPerBeat = 60.00 / beatsperminute;//division, so FLOAT arithmmatic
  delayPerBeat = delayPerBeat*1000;
  
  int songID = ID;
  String chordList = chordChanges[songID];
  String thisChar;
  int beat = 1;
  
  //represents the location of the "(" symbol for chords. First one is at 0. 
  int begMarker = 3;
  int endMarker = 0;

  //repeats for every element in the song string. 
  for (int i=0;i<chordList.length();i++) {
    
    if (beat > 4) {
      beat = 1;
    }
    
    thisChar = chordList.substring(i, i+1);
    
    if (thisChar==")") {

      endMarker = i;
      String thisChord = chordList.substring(begMarker+1, endMarker); //end not inclusive. (discard parentheses). beginning is inclusive. 

      displayCentered("Chord: " + thisChord, 1);
      
      Serial.println(thisChord);
      int thirdCode;
      int rootCode = rootCodeFn(thisChord);
      Serial.println("Root Code: " + String(rootCode));
      
      //deal with root/third
      String firstNtUpperCase = thisChord.substring(0,1);
      firstNtUpperCase.toUpperCase();
      
      String firstNt = thisChord.substring(0,1);
      
      //if lowercase...
      if (firstNt != firstNtUpperCase) {
         //lowercase (minor)
         Serial.println("minor");
         thirdCode = rootCode+3;
      } else {
         //upper (major)
         thirdCode = rootCode+4;
      }

      //for overflow. (if B)
      if (thirdCode>12) {
        thirdCode = thirdCode-12;
      }

      //plus 26 to shift to the lasers; thirteen lazers, pins 26--38)
      
      digitalWrite(thirdCode+26, HIGH);
      
      //deal with the tags
      int numoftags = (thisChord.length()-2)/2;
      String onlyTags = thisChord.substring(2);
      
      Serial.println(thisChord+ ": " + numoftags);
      
      //this highlights all the tag notes
      for (int x=0;x<numoftags;x++) {
        
        int tagCode;
        String tag = onlyTags.substring(x*2, (x*2)+2);
        Serial.println(tag);
        
        if (tag=="+7") {
          tagCode = rootCode + 10;
        } else if (tag=="M7") {
          tagCode = rootCode + 11;
        } else if (tag=="+9") {
          tagCode = rootCode + 2;
        } else if (tag=="+6") {
          tagCode = rootCode + 9;
        } else if (tag=="b9") {
          tagCode = rootCode + 1;
        } else if (tag=="#5") {
          tagCode = rootCode + 8;
        } else if (tag=="b5") {
          tagCode = rootCode + 6;
        } else if (tag=="00") {
          tagCode = rootCode + 9;
        } else if (tag=="0/") {
          tagCode = rootCode + 10;
        } else if (tag=="au") {
          tagCode = rootCode + 8;
        } else if (tag=="+4") {
          tagCode = rootCode + 6;
        }
        
        //"overflow"
        if (tagCode>12) {
          tagCode = tagCode-12;
        }
        digitalWrite(tagCode+26, HIGH);
      }

      displayCentered("Beat: " + String(beat), 2);
      beat++;
      delay(delayPerBeat);
      
      
    } else if (thisChar=="(") {
      begMarker = i;
      lcd.clear();
      
      //clear the lasers. 
      turnOffLasers();

    } else if (thisChar=="-") {
      displayCentered("Beat: " + String(beat), 2);
      beat++;
      delay(delayPerBeat);
    }
    
  }

  //when song is done. 
  turnOffLasers();
  lcd.clear();

  displayCentered("Song complete.", 1);
  
  while (digitalRead(OUTbuttonPinInput)==LOW) {
        //nothing. 
  }
      
  interfaceZ = 0;
  interface("out");
}

void playSongModal(int ID, float beatsperminute) {
  float delayPerBeat = 60.00 / beatsperminute;//division, so FLOAT arithmmatic
  delayPerBeat = delayPerBeat*1000;
  
  int songID = ID;
  String chordList = chordChanges[songID];
  String thisChar;
  int beat = 1;
  
  //represents the location of the "(" symbol for chords. First one is at 0. 
  int begMarker = 3;
  int endMarker = 0;

  //repeats for every element in the song string. 
  for (int i=0;i<chordList.length();i++) {
    
    if (beat > 4) {
      beat = 1;
    }
    
    thisChar = chordList.substring(i, i+1);
    
    if (thisChar==")") {

      endMarker = i;
      String thisChord = chordList.substring(begMarker+1, endMarker); //end not inclusive. (discard parentheses). beginning is inclusive. 

      displayCentered("Chord: " + thisChord, 1);
      
      int rootCode = rootCodeFn(thisChord);

      int numoftags = (thisChord.length()-2)/2;

      String firstNtUpperCase = thisChord.substring(0,1);
      firstNtUpperCase.toUpperCase();

      String firstNt = thisChord.substring(0,1);
      String firstChordTag = thisChord.substring(2,4);

      int listLength;
      int* ptrScaleMatrix;
        
      //if lowercase...
      if (firstNt != firstNtUpperCase) {
         //lowercase (minor)
         if (firstChordTag=="00") {
          ptrScaleMatrix = &diminished[0]; //diminished
          listLength = 9;
         } else if (firstChordTag=="+6") {
          ptrScaleMatrix = &melodicMin[0]; //melodic minor
          listLength = 8;
         } else {//if it's not diminished or min6, then just make it a dorian scale. This includes minor seven chords. (dn+7) for example.
          ptrScaleMatrix = &dorian[0]; //dorian
          listLength = 8;
         }
         
      } else {
         //upper (major)
        if (firstChordTag=="au") {
          ptrScaleMatrix = &whole[0];//whole tone
          listLength = 7;
        } else if (firstChordTag=="+7") {
          if (numoftags == 1) {
            ptrScaleMatrix = &mixolydian[0]; //mixolydian
            listLength = 8;
          } else {
            ptrScaleMatrix = &altered[0]; //altered
            listLength = 8;
          }
        } else {//if it's not dominant or augmented, then we'll just use lydian
          ptrScaleMatrix = &lydian[0];//lydian
          listLength = 8;
        }
        
      }

      //writing to the lasers!
      for (int z=0; z < listLength; z++) {
        int noteCode = rootCode + *(ptrScaleMatrix+z); //getting the array through pointer
        
        if (noteCode>12) {
          noteCode = noteCode-12;
        }

        digitalWrite(noteCode+26, HIGH);

        if (noteCode==12) {//if the c is in there somewhere, then make sure to hit the lower one (which is disfavored unless you're a c scale)
          digitalWrite(26, HIGH);
        }
      }

      displayCentered("Beat: " + String(beat), 2);
      beat++;
      delay(delayPerBeat);
      
      
    } else if (thisChar=="(") {
      begMarker = i;
      lcd.clear();
      
      //clear the lasers. 
      turnOffLasers();

    } else if (thisChar=="-") {
      displayCentered("Beat: " + String(beat), 2);
      beat++;
      delay(delayPerBeat);
    }
    
  }

  //when song is done. 
  turnOffLasers();
  lcd.clear();

  displayCentered("Song complete.", 1);
  
  while (digitalRead(OUTbuttonPinInput)==LOW) {
        //nothing. 
  }
      
  interfaceZ = 0;
  interface("out");
}

void turnOffLasers() {
  for (int b=0; b<13; b++) {
     digitalWrite(26+b, LOW);
  }
}

void interface(String action) {  
  if (action=="in") {
    lcd.clear();
    interfaceZ = interfaceZ + 1;
    if (interfaceZ==3) {
      Serial.println("This is getting sent: " + String(potentialBPM));
      
      if (mode == "chordal") {
        playSongChordal(potentialSongID, potentialBPM);
      } else if (mode == "modal") {
        playSongModal(potentialSongID, potentialBPM);
      }
    }
    
    initialscrollerValue = analogRead(dialPinInput);
  } else if (action=="out") {
    if (interfaceZ !=0) {//making sure that if we're on home screen, nothing happens
      lcd.clear();
      interfaceZ = interfaceZ - 1;
      initialscrollerValue = analogRead(dialPinInput);
    }
  } else if (action=="idle") {
    //skip
  }

  if (interfaceZ==0) {
    welcome();
    
  } else if (interfaceZ==1) {
    
    int scrollerValue = analogRead(dialPinInput);
 
    int higherbound;
    int lowerbound;

    if (initialscrollerValue+200>1023) {
      lowerbound = 823;
      higherbound = 1023;
    } else {
      lowerbound = initialscrollerValue;
      higherbound = initialscrollerValue+200;
    }
    
    if (scrollerValue >= lowerbound && scrollerValue <= higherbound) {

      //this would represent the index of the song the scroller is selecting
      byte mapped = map(scrollerValue, lowerbound, higherbound, 0, numsongs-1);

      //if it's different than the prev, clear lcd
      if (potentialSongID != mapped) {
        lcd.clear();
      }
      
      displayCentered(songs[mapped], 1);
      displayCentered(by[mapped], 2);

      potentialSongID = mapped;
    } else if (scrollerValue<lowerbound) {
      
      displayCentered(songs[0], 1);
      displayCentered(by[0], 2);
      potentialSongID = 0;
      
    } else if (scrollerValue>higherbound) { //numsongs-1 to get last element
      displayCentered(songs[numsongs-1], 1);
      displayCentered(by[numsongs-1], 2);
      potentialSongID = numsongs-1;
    }
    
    
  } else if (interfaceZ==2) {
      displayCentered(songs[potentialSongID], 1);
      displayCentered("at", 2);

      int scrollerValue = analogRead(dialPinInput);
      int lowerbound;
      int higherbound;

      if (initialscrollerValue+200>1023) {
        lowerbound = 823;
        higherbound = 1023;
      } else {
        lowerbound = initialscrollerValue;
        higherbound = initialscrollerValue+200;
      }
      
      if (scrollerValue >= lowerbound && scrollerValue <= higherbound) {
        
        int squishtoBPM = map(scrollerValue, lowerbound, higherbound, 20, 200);

        //if the BPM value is less than 100, tack on a 0 so we don't get bleed
        if (squishtoBPM<100) {
          displayCentered("0"+String(squishtoBPM)+"BPM", 3);
        } else {
          displayCentered(String(squishtoBPM)+"BPM", 3);
        }

        potentialBPM = squishtoBPM;
        Serial.println(potentialBPM);
        
      } else if (scrollerValue < lowerbound) {
        displayCentered("020BPM", 3);
        potentialBPM = 20;
      } else if (scrollerValue > higherbound) {
        potentialBPM = 200;
        displayCentered("200BPM", 3);
      }
      
  } 
}
