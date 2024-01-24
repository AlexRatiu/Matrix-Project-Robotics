#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(9, 8, 7, 3, 5, 4);

//Define The Snake as a Struct
typedef struct Snake Snake;
struct Snake{
  int head[2];     // the (row, column) of the snake head
  int body[40][2]; //An array that contains the (row, column) coordinates
  int len;         //The length of the snake 
  int dir[2];      //A direction to move the snake along
};

//Define The Apple as a Struct
typedef struct Apple Apple;
struct Apple{
  int rPos; //The row index of the apple
  int cPos; //The column index of the apple
};

//MAX72XX led Matrix
const int DIN =12;
const int CS =10;
const int CLK = 11;
LedControl lc = LedControl(DIN, CLK, CS,1);

const int varXPin = A0;//X Value  from Joystick
const int varYPin = A1;//Y Value from Joystick
const int pinSw = A2;
bool buttonState = 1;
bool lastButtonState = 1;
int debounceDelay = 150;
long lastDebounceTime = 0;

int currentScore = 0;
int livesCount = 3;

int score1 = 0;
int score2 = 0;
int score3 = 0;

int isPlaying = 0;

bool appleBlink = true;

int LCDBrightness = 5;
int MatrixBrightness;
int Sound;


byte pic[8] = {0,0,0,0,0,0,0,0};//The 8 rows of the LED Matrix

Snake snake = {{1,5},{{0,5}, {1,5}}, 2, {1,0}};//Initialize a snake object
Apple apple = {(int)random(0,8),(int)random(0,8)};//Initialize an apple object

//Variables To Handle The Game Time
float oldTime = 0;
float timer = 0;
float updateRate = 2;

bool introWasShowed = false;
int menuLine = 0;
int submenuLine = 0;
int submenuLine2 = 0;
int currentMenu = 0;



int tempo = 180;
int buzzer = 2;
int melody[] = {659, 8, 587, 8, 370, 4, 415, 4, 554, 8, 494, 8, 294, 4, 330, 4, 494, 8, 440, 8, 277, 4, 330, 4, 440, 2, };
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;


byte customChar1[8] = {
  B00000,
  B11101,
  B10000,
  B11110,
  B00010,
  B11110,
  B11100,
  B00100
};

byte customChar2[8] = {
  B00000,
  B01111,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte customChar4[8] = {
  B01110,
  B10001,
  B00001,
  B00010,
  B00100,
  B00100,
  B00000,
  B00100
};

byte customChar3[8] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11011,
  B10001,
  B00000,
  B00000
};

byte customChar5[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte customChar6[8] = {
  B00010,
  B00100,
  B11011,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000
};

byte customChar7[8] = {
  B00000,
  B01000,
  B11110,
  B01001,
  B00001,
  B01110,
  B00000,
  B00000
};

// byte customChar8[8] = {
//   B00000,
//   B10101,
//   B01110,
//   B11111,
//   B01110,
//   B10101,
//   B00000,
//   B00000
// };

// byte customChar9[8] = {
//   B00000,
//   B00001,
//   B00111,
//   B11111,
//   B11111,
//   B00111,
//   B00001,
//   B00000
// };

byte start[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,1,1,0,1},
  {0,0,0,0,1,0,0,0},
  {0,1,1,1,1,0,0,0},
  {0,1,0,0,0,0,0,0},
  {0,1,1,1,0,0,0,0},
  {0,0,0,1,0,0,0,0},
  {0,0,0,1,0,0,0,0}
};

byte settings[8][8] = {
  {0,0,0,1,1,1,1,0},
  {0,0,1,1,1,1,1,0},
  {0,1,1,1,1,1,1,0},
  {0,0,0,1,1,0,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,0,1,1,0,0,0}
};

byte how_to[8][8] = {
  {0,1,1,1,1,1,1,0},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,0,0,1,1,1},
  {1,1,0,0,0,0,1,1},
  {1,1,0,0,0,0,1,1},
  {0,0,0,0,0,0,0,0}
};

byte about[8][8] = {
  {0,0,1,1,1,0,0,0},
  {0,1,0,0,0,1,0,0},
  {0,1,0,0,0,1,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,1,0,0,0,0},
  {0,0,0,1,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,1,0,0,0,0}
};

int i,j;//Counters
void setup() {
  // put your setup code here, to run once:
    /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
  Serial.begin(9600);

  LCDBrightness = EEPROM.read(0);
  MatrixBrightness = EEPROM.read(1);
  Sound = (bool)EEPROM.read(2);

  analogWrite(6, LCDBrightness * 25);
  lc.setIntensity(0, MatrixBrightness);

  //Set Joystick Pins as INPUTs
  pinMode(varXPin, INPUT);
  pinMode(varYPin, INPUT);
  pinMode(pinSw, INPUT);

  lcd.createChar(1, customChar1);
  lcd.createChar(2, customChar2);
  lcd.createChar(3, customChar3);
  lcd.createChar(4, customChar4);
  lcd.createChar(5, customChar5);
  lcd.createChar(6, customChar6);
  lcd.createChar(7, customChar7);
  // lcd.createChar(8, customChar8);
  // lcd.createChar(9, customChar9);
}

void song(){
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
}


void playGame() {
    // put your main code here, to run repeatedly:
  float deltaTime = calculateDeltaTime();
  timer += deltaTime;

  //Check For Inputs
  int xVal = analogRead(varXPin);
  int yVal = analogRead(varYPin);
  
  if(xVal<100 && snake.dir[1]==0){
    snake.dir[0] = 0;
    snake.dir[1] = -1;
  }else if(xVal >920 && snake.dir[1]==0){
    snake.dir[0] = 0;
    snake.dir[1] = 1;
  }else if(yVal<100 && snake.dir[0]==0){
    snake.dir[0] = 1;
    snake.dir[1] = 0;
  }else if(yVal >920 && snake.dir[0]==0){
    snake.dir[0] = -1;
    snake.dir[1] = 0;
  }
  
  //Update
  if(timer > 1000/updateRate){
    timer = 0;
    Update();
  }
  
  //Render
  Render();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(currentScore);
  lcd.print(" ");
  lcd.write((byte)5);
  lcd.setCursor(0, 1);
  lcd.print("Lives: ");
  lcd.print(livesCount);
  lcd.print(" ");
  lcd.write((byte)6);
}

void showIntro() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Snake");
  lcd.setCursor(0, 1);
  lcd.print("  by Alex Ratiu");

  song();
  lcd.clear();

  introWasShowed = true;
}

void light() {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      lc.setLed(0, r, c, 1);
    }
  }
}

void dark() {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      lc.setLed(0, r, c, 0);
    }
  }
}



void switchBetweenMenuOptions() {
  int yVal = analogRead(varYPin);

  
  if (yVal < 100)
    menuLine ++;
  else if (yVal > 1000)
    menuLine --;

  if (menuLine == 4)
    menuLine = 0;
  else if (menuLine == -1)
    menuLine = 3;

  if (yVal < 100)
    submenuLine ++;
  else if (yVal > 1000)
    submenuLine --;

  if (submenuLine == 2)
    submenuLine = 0;
  else if (submenuLine == -1)
    submenuLine = 1;

   if (yVal < 100)
    submenuLine2 ++;
  else if (yVal > 1000)
    submenuLine2 --;

  if (submenuLine2 == 3)
    submenuLine2 = 0;
  else if (submenuLine2 == -1)
    submenuLine2 = 2;
  
  if (yVal < 100 && isPlaying == 0 && Sound == 1){
    tone(buzzer,100);
    delay(100);
    noTone(buzzer);
  }
  else if (yVal > 1000 && isPlaying == 0 && Sound == 1){
    tone(buzzer,100);
    delay(100);
    noTone(buzzer);
  }

  
}

void showMenu() {
  lcd.setCursor(0, 0);
  buttonState = analogRead(pinSw);
  Serial.print(buttonState);
  Serial.print(" ");
  Serial.println(lastButtonState);
  
  switch (currentMenu){
    case 0:
    {
      switch (menuLine) {
        case 0:
        {
          lcd.clear();
          
          lcd.setCursor(0, 0);
          lcd.print("> Start game ");
          lcd.write((byte)1);
          lcd.setCursor(0, 1);
          lcd.print("  Settings ");
          lcd.write((byte)2);

          for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
              lc.setLed(0, row, col, start[row][col]);
            }
          }

          
          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            dark();
            currentMenu = 1;
            menuLine = 0;
          }
          lastButtonState = buttonState;
          
          break;
        }
        case 1:
        {
          lcd.clear();
          
          lcd.setCursor(0, 0);
          lcd.print("> Settings ");
          lcd.write((byte)2);
          lcd.setCursor(0, 1);
          lcd.print("  About ");
          lcd.write((byte)4);

          for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
              lc.setLed(0, row, col, settings[row][col]);
            }
          }

          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            dark();
            menuLine = 0;
            currentMenu = 2;
          }
          lastButtonState = buttonState;

          break;
        }
        case 2:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("> About ");
          lcd.write((byte)4);
          lcd.setCursor(0, 1);
          lcd.print("  How to play ");
          lcd.write((byte)3);

          for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
              lc.setLed(0, row, col, about[row][col]);
            }
          }

          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            dark();
            submenuLine = 0;
            currentMenu = 3;
          }
          lastButtonState = buttonState;

          break;
        }
        case 3:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("> How to play ");
          lcd.write((byte)3);
          lcd.setCursor(0, 1);
          lcd.print("  Start game ");
          lcd.write((byte)1);

          for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
              lc.setLed(0, row, col, how_to[row][col]);
            }
          }

          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            dark();
            submenuLine = 0;
            currentMenu = 4;
          }
          lastButtonState = buttonState;

          break;
        }
      }
      break;
    }

    case 1:
    {
      lcd.clear();
      isPlaying = 1;
      playGame();
      if(livesCount == 0 && score1 == 7 && score2 == 7 && score3 == 7){
        menuLine = 0;
        currentMenu = 9;
      }
      else if(livesCount == 0){
        menuLine = 0;
        currentMenu = 8;
      }

      break;
    }

    case 2:
    {
      lcd.clear();
      switch (menuLine) {
        case 0:
        {  
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">LCD Brightness");
          //lcd.write((byte)8);
          lcd.setCursor(0, 1);
          lcd.print(" Mx Brightness");
          //lcd.write((byte)8);

          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            currentMenu = 5;
          }
          lastButtonState = buttonState;

          break;
        }
        case 1:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">Mx Brightness");
          //lcd.write((byte)8);
          lcd.setCursor(0, 1);
          lcd.print(" Sound");
          //lcd.write((byte)9);


          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            currentMenu = 6;
          }
          lastButtonState = buttonState;

          break;
        }
        case 2:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">Sound");
          //lcd.write((byte)9);
          lcd.setCursor(0, 1);
          lcd.print(" Back ");
          lcd.write((byte)7);

          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            currentMenu = 7;
          }
          lastButtonState = buttonState;

          break;
        }
        case 3:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">Back ");
          lcd.write((byte)7);
          lcd.setCursor(0, 1);
          lcd.print(" LCD Brightness");
          //lcd.write((byte)8);

          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            menuLine = 0;
            currentMenu = 0;
          }
          lastButtonState = buttonState;

          break;
        }
      }
      break;
    }

    case 3:
    {
      lcd.clear();
      switch (submenuLine) {
        case 0:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("  This is the game Snake.");
          for (int positionCounter = 0; positionCounter <20; positionCounter ++) {
            lcd.scrollDisplayLeft ();
            delay (350);
            if(positionCounter == 19)
              break;
          }
          lcd.clear();
          lcd.print("  Developed by Alex Ratiu.");
          for (int positionCounter = 0; positionCounter <18; positionCounter ++) {
            lcd.scrollDisplayLeft ();
            delay (350);
            if(positionCounter == 17)
              break;
          }
          lcd.clear();
          lcd.print("  Github: github.com/AlexRatiu ");
          for (int positionCounter = 0; positionCounter <25; positionCounter ++) {
            lcd.scrollDisplayLeft ();
            delay (350);
            if(positionCounter == 24)
              break;
          }
          lcd.clear();
          submenuLine = 1;         
          break;         
        }
        case 1:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("  This is the game Snake.");
          lcd.setCursor(0, 1);
          lcd.print("> Back ");
          lcd.write((byte)7);

          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            menuLine = 0;
            currentMenu = 0;
          }
          lastButtonState = buttonState;

          break;
        }
      }
      break;
    }

    case 4:
    {
      lcd.clear();
      switch (submenuLine) {
        case 0:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("  1. Move using the joystick.");
          for (int positionCounter = 0; positionCounter <16; positionCounter ++) {
            lcd.scrollDisplayLeft ();
            delay (350);
            if(positionCounter == 15)
              break;
          }
          lcd.clear();
          lcd.print("  2. Eat apples to grow.");
          for (int positionCounter = 0; positionCounter <11; positionCounter ++) {
            lcd.scrollDisplayLeft ();
            delay (350);
            if(positionCounter == 10)
              break;
          }
          lcd.clear();
          lcd.print("  3. Be carefull not to eat yourself!");
          for (int positionCounter = 0; positionCounter <24; positionCounter ++) {
            lcd.scrollDisplayLeft ();
            delay (350);
            if(positionCounter == 23)
              break;
          }
          lcd.clear();
          lcd.print("  4. The speed increases with the score.");
          for (int positionCounter = 0; positionCounter <26; positionCounter ++) {
            lcd.scrollDisplayLeft ();
            delay (350);
            if(positionCounter == 25)
              break;
          }
          lcd.clear();
          submenuLine = 1;         
          break;
        }
        case 1:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("  Move using the joy");
          lcd.setCursor(0, 1);
          lcd.print("> Back ");
          lcd.write((byte)7);
          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            menuLine = 0;
            currentMenu = 0;
          }
          lastButtonState = buttonState;

          break;
        }
      }
      break;
    }

    case 5:
    { 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LCD Brightness");
      lcd.setCursor(0, 1);
      lcd.print("  < ");
      lcd.print(LCDBrightness);
      lcd.print(" >");

      int xVal = analogRead(varXPin);
      if (xVal < 100)
        LCDBrightness --;
      else if (xVal > 1000)
        LCDBrightness ++;

      if (LCDBrightness == 11)
        LCDBrightness = 10;
      else if (LCDBrightness == -1)
        LCDBrightness = 0;

      EEPROM.write(0, LCDBrightness);
      analogWrite(6, LCDBrightness * 25);

      lastButtonState = 1;
      if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
        lastDebounceTime = millis();
        menuLine = 0;
        currentMenu = 2;
      }
      lastButtonState = buttonState;

      break;
    }

    case 6:
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Matrix Brightness");
      lcd.setCursor(0, 1);
      lcd.print("  < ");
      lcd.print(MatrixBrightness);
      lcd.print(" >");

      int xVal = analogRead(varXPin);
      if (xVal < 100)
        MatrixBrightness --;
      else if (xVal > 1000)
        MatrixBrightness ++;

      if (MatrixBrightness == 11)
        MatrixBrightness = 10;
      else if (MatrixBrightness == -1)
        MatrixBrightness = 0;

      EEPROM.write(1, MatrixBrightness);

      light();
      lc.setIntensity(0, MatrixBrightness);
      
      lastButtonState = 1;
      if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
        lastDebounceTime = millis();
        dark();
        menuLine = 0;
        currentMenu = 2;
      }
      lastButtonState = buttonState;

      break;
    }

    case 7:
    {
      if(Sound == 0){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sound");
        lcd.setCursor(0, 1);
        lcd.print("  < OFF > ");
      }
      else if(Sound == 1){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sound");
        lcd.setCursor(0, 1);
        lcd.print("  < ON > ");
      }

      int xVal = analogRead(varXPin);
      if (xVal < 100)
        Sound --;
      else if (xVal > 1000)
        Sound ++;

      if (Sound > 1)
        Sound = 1;
      else if (Sound < 0)
        Sound = 0;

      EEPROM.write(2, Sound);
      
      lastButtonState = 1;
      if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
        lastDebounceTime = millis();
        menuLine = 0;
        currentMenu = 2;
      }
      lastButtonState = buttonState;

      break;
    }

    case 8:
    {
      lcd.clear();
      switch (submenuLine2) {
        case 0:
        { 
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">Score: ");
          lcd.print(score1); 
          lcd.print(" ");
          lcd.print(score2); 
          lcd.print(" ");
          lcd.print(score3);
          lcd.setCursor(0, 1);
          lcd.print(" Restart");     
          break;
        }
        case 1:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">Restart");
          lcd.setCursor(0, 1);
          lcd.print(" MainMenu"); 

          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            livesCount = 3;
            currentScore = 0;
            score1 = 0;
            score2 = 0;
            score3 = 0;
            currentMenu = 1;
          }
          lastButtonState = buttonState;

          break;
        }
        case 2:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">MainMenu");
          lcd.setCursor(0, 1);
          lcd.print(" Score: ");
          lcd.print(score1); 
          lcd.print(" ");
          lcd.print(score2); 
          lcd.print(" ");
          lcd.print(score3); 

          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            livesCount = 3;
            score1 = 0;
            score2 = 0;
            score3 = 0;
            currentScore = 0;
            currentMenu = 0;
            menuLine = 0;
            isPlaying = 0;
          }
          lastButtonState = buttonState;

          break;
        }
      }
      break;
    }

    case 9:
    {
      lcd.clear();
      switch (submenuLine2) {
        case 0:
        { 
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">LUCKY NR: ");
          lcd.print(score1); 
          lcd.print(" ");
          lcd.print(score2); 
          lcd.print(" ");
          lcd.print(score3);
          lcd.setCursor(0, 1);
          lcd.print(" Restart");     
          break;
        }
        case 1:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">Restart");
          lcd.setCursor(0, 1);
          lcd.print(" MainMenu"); 

          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            livesCount = 3;
            currentScore = 0;
            score1 = 0;
            score2 = 0;
            score3 = 0;
            currentMenu = 1;
          }
          lastButtonState = buttonState;

          break;
        }
        case 2:
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">MainMenu");
          lcd.setCursor(0, 1);
          lcd.print(" LUCKY NR: ");
          lcd.print(score1); 
          lcd.print(" ");
          lcd.print(score2); 
          lcd.print(" ");
          lcd.print(score3); 

          lastButtonState = 1;
          if((millis() - lastDebounceTime > debounceDelay)&&(buttonState != lastButtonState)){
            lastDebounceTime = millis();
            livesCount = 3;
            score1 = 0;
            score2 = 0;
            score3 = 0;
            currentScore = 0;
            currentMenu = 0;
            menuLine = 0;
            isPlaying = 0;
          }
          lastButtonState = buttonState;

          break;
        }
      }
      break;
    }
  }

  switchBetweenMenuOptions();
  delay(200);
}

void loop() {
  if (!introWasShowed)
    showIntro();
  else
    showMenu();
}

float calculateDeltaTime(){
  float currentTime = millis();
  float dt = currentTime - oldTime;
  oldTime = currentTime;
  return dt;
}

void reset(){
  for(int j=0;j<8;j++){
    pic[j] = 0;
  }
}
void Update(){
  reset();//Reset (Clear) the 8x8 LED matrix
  
  int newHead[2] = {snake.head[0]+snake.dir[0], snake.head[1]+snake.dir[1]};

  //Handle Borders
  if(newHead[0]==8){
    newHead[0]=0;
  }else if(newHead[0]==-1){
    newHead[0] = 7;
  }else if(newHead[1]==8){
    newHead[1]=0;
  }else if(newHead[1]==-1){
    newHead[1]=7;
  }
  
  //Check If The Snake hits itself
   for(j=0;j<snake.len;j++){
    if(snake.body[j][0] == newHead[0] && snake.body[j][1] == newHead[1]){
      //Pause the game for 1 sec then Reset it
      if(Sound == 1){
      tone(buzzer, 200);
      delay(1000);
      noTone(buzzer);
      }
      
      snake = {{1,5},{{0,5}, {1,5}}, 2, {1,0}};//Reinitialize the snake object
      apple = {(int)random(0,8),(int)random(0,8)};//Reinitialize an apple object
      livesCount --;
      updateRate = 2;
      if(score1 == 0){
        score1 = currentScore;
        currentScore = 0;
        break;
      }
      else if(score2 == 0){
        score2 = currentScore;
        currentScore = 0;
        break;
      }
      else if(score3 == 0){
        score3 = currentScore;
        currentScore = 0;
        break;
      }
      
      return;
    }
  }

  //Check if The snake ate the apple
  if(newHead[0] == apple.rPos && newHead[1] ==apple.cPos){
    if(Sound == 1){
    tone(buzzer, 1000);
    delay(300);
    noTone(buzzer);
    }

    currentScore ++;
    snake.len = snake.len+1;
    apple.rPos = (int)random(0,8);
    apple.cPos = (int)random(0,8);
    updateRate += 0.2;
  }else{
    removeFirst();//Shifting the array to the left
  }
  
  snake.body[snake.len-1][0]= newHead[0];
  snake.body[snake.len-1][1]= newHead[1];
  
  snake.head[0] = newHead[0];
  snake.head[1] = newHead[1];
  
  //Update the pic Array to Display(snake and apple)
  for(j=0;j<snake.len;j++){
    pic[snake.body[j][0]] |= 128 >> snake.body[j][1];
  }
  if(appleBlink){
    pic[apple.rPos] |= 128 >> apple.cPos;
    appleBlink = false;
  }
  else 
    appleBlink = true;
  
  
}

void Render(){
  
   for(i=0;i<8;i++){
    lc.setRow(0,i,pic[i]);
   }
}

void removeFirst(){
  for(j=1;j<snake.len;j++){
    snake.body[j-1][0] = snake.body[j][0];
    snake.body[j-1][1] = snake.body[j][1];
  }
}