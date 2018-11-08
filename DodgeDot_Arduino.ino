#include <EEPROM.h> //Storage 1,024 bytes
#include <MaxMatrix.h>
#include "pitches.h" //add Equivalent frequency for musical note
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

uint8_t EEPROMhighscore = 101;

int DIN = 7;  // DIN pin of MAX7219 module
int CS  = 6;  // CS pin of MAX7219 module
int CLK = 5;  // CLK pin of MAX7219 module
int maxInUse = 1;

unsigned long timeNow = 0; //Used for pausing score stats at end of game

int joyX  = A1;
int joySw = A2;
int valX  = 0;
int valSw = 0;
int posX  = 3; // Sets random start postition
int foeY  = 0; //Starts enemy at top
int foeX  = posX;
int foeY2 = 8; //Starts enemy at bottom to reset its postion
int foeX2 = -1;
int foeY3 = 8; //Starts enemy at bottom to reset its postion
int foeX3 = -1;
int foeY4 = 8; //Starts enemy at bottom to reset its postion
int foeX4 = -1;
int spd   = 80;
int sense = 50;
int score = 0;
int highScore;

bool anim = 1; //Value used in start animation
bool spdUp = 0; //Value if the game is to speed up
bool play = 0; //Value of play the game

MaxMatrix m(DIN, CS, CLK, maxInUse); 

char num1[] = {3, 7,
            0x24,
            0x3E,
            0x20,
            };
            
char num2[] = {3, 7,
            0x3A,
            0x2A,
            0x2E,
            };
char num3[] = {3, 7,
            0x2A,
            0x2A,
            0x3E,
            };

char num4[] = {3, 7,
            0x0E,
            0x08,
            0x3E,
            };

char num5[] = {3, 7,
            0x2E,
            0x2A,
            0x3A,
            };

char num6[] = {3, 7,
            0x3E,
            0x2A,
            0x3A,
            };

char num7[] = {3, 7,
            0x02,
            0x02,
            0x3E,
            };
char num8[] = {3, 7,
            0x3E,
            0x2A,
            0x3E,
            };

char num9[] = {3, 7,
            0x2E,
            0x2A,
            0x3E,
            };

char num0[] = {3, 7,
            0x3E,
            0x22,
            };

char smileNo[] = {8, 8,
                  B00111100,
                  B01000010,
                  B10100101,
                  B10010001,
                  B10010001,
                  B10100101,
                  B01000010,
                  B00111100
                 };

char smileYes[] = {8, 8,
                  B00111100,
                  B01000010,
                  B10010101,
                  B10100001,
                  B10100001,
                  B10010101,
                  B01000010,
                  B00111100
                 };
            
void setup() {
  m.init(); // MAX7219 initialization
  m.setIntensity(1); // initial led matrix intensity, 0-15
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW); // SETS PIN 2 to GND
  pinMode(A2,INPUT_PULLUP);

  pinMode(12, OUTPUT); //SETS PIN 12 to 5V
  digitalWrite(12, HIGH);
  
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Press joystick");
  lcd.setCursor(0,1);
  lcd.print("to play...");

  Serial.begin(9600);
  
}


//MAIN PART OF PROGRAM
void loop() 
{
  valSw = digitalRead(A2); 
 
  if (valSw == 0)
  {
    play = 1;
    lcd.clear();
    
  }
  
  while (play) //Starts loop
  {
    while (anim) //Start Countdown
    {
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.print("Starting game...");
      m.clear();
      m.writeSprite(3,0, num3);
      tone (3, NOTE_E4, 750);
      delay(1000);
      m.writeSprite(3,0, num2);
      tone (3, NOTE_E4, 750);
      delay(1000);
      m.writeSprite(3,0, num1);
      tone (3, NOTE_G5, 1000);
      delay(1000);
      m.setDot(posX,7,true); // SETS START POSITION
      anim = 0;

      lcd.clear();
      lcd.print("High Score:");
      lcd.setCursor(0,1);
      lcd.print("Score:");
    }

    //Prints info on LCD
    highScore = EEPROM.read(EEPROMhighscore);
    lcd.setCursor(11,0);
    lcd.print(highScore);
    lcd.setCursor(6,1);
    lcd.print(score);
  
  //Sends Enemy
    sendEnemy();
    if (score > 9)
    {
      sendEnemy2();
      if (score > 19)
      {
        sendEnemy3();
        if (score > 29)
        {
          sendEnemy4();
          if (score > 39)
          {
            spdUp = 1;
          }
        } 
      }
    }
  
  //Delay works as the speed of the game
    delay(spd);
  
  }
}
//POSTITIONS/MOVES THE DOT
void dotPosition()
{
  //Joystic Xvalues: 1023 <-> 0
  valX = analogRead(joyX); // Reads value of joystick
  if (valX < 500) //Deadzone. More = Less Deadzone. Max = 507
  {
    if (posX != 7)
    {
      posX ++;  
    }
    m.setDot(posX, 7, true);
  } 
  else if (valX > 518) //Deadzone. Less = Less Deadzone. Min = 507
  {
    if (posX != 0)
    {
      posX --;  
    }
    m.setDot(posX, 7, true);
  }
  m.clear();
  m.setDot(posX, 7, true);
}

void sendEnemy()
{
  dotPosition();
  m.setDot(foeX, foeY, true);
  m.setDot(foeX, foeY-1, true);
  if (foeY > 6 && foeX == posX) //Cheks if enemy hits user
  {
    gameOver();
  }
  else if (foeY < 8)
  {
     foeY ++;
     tone (3, NOTE_D4, 10);
  }  
  else if (foeY > 7 && foeX != posX) //Cheks if enemy hits bootom, and resets enemy to top
  {
    foeX = posX; //Sets enemy to the same postion as the user to avaoid camping
    foeY = 0;
    score ++;
  }
}

void sendEnemy2()
{
  m.setDot(foeX2, foeY2, true);
  m.setDot(foeX2, foeY2-1, true);
  if (foeY2 > 6 && foeX2 == posX) //Cheks if enemy hits user
  {
    gameOver();
  }
  else if (foeY2 < 8)
  {
     foeY2 ++;
  }  
  else if (foeY2 > 7 && foeX2 != posX) //Cheks if enemy hits bootom, and resets enemy to top
  {
    foeX2 = random(0,8);
    while (foeX2 == posX) //Sets enemy1 to a random postion that does not confilct with enemy2 as enemy2s postion is set to posX
    {
      foeX2 = random(0,8);
    }
    foeY2 = 0;
  }
}

void sendEnemy3()
{
  m.setDot(foeX3, foeY3, true);
  m.setDot(foeX3, foeY3-1, true);
  if (foeY3 > 6 && foeX3 == posX) //Cheks if enemy hits user
  {
    gameOver();
  }
  else if (foeY3 < 8)
  {
     foeY3 ++;
  }  
  else if (foeY3 > 7 && foeX3 != posX) //Cheks if enemy hits bootom, and resets enemy to top
  {
    foeX3 = random(0,8);
    while (foeX == foeX3 || foeX2 == foeX3)
    {
      foeX3 = random(0,8);
    }
    foeY3 = 0;
  }
}

void sendEnemy4()
{
  m.setDot(foeX4, foeY4, true);
  m.setDot(foeX4, foeY4-1, true);
  if (foeY4 > 6 && foeX4 == posX) //Cheks if enemy hits user
  {
    gameOver();
  }
  else if (foeY4 < 8)
  {
     foeY4 ++;
  }  
  else if (foeY4 > 7 && foeX4 != posX) //Cheks if enemy hits bootom, and resets enemy to top
  {
    foeX4 = random(0,8);
    while (foeX == foeX4 || foeX2 == foeX4 || foeX3 == foeX4)
    {
      foeX4 = random(0,8);
    }
    foeY4 = 0;
    if (spd > 50 && spdUp)
    {
      spd --;
    }
  }
}

void gameOver()
{
  highScore = EEPROM.read(EEPROMhighscore); //Reads off the HighScore from local storage on Arduino
  m.clear();

  //Plays loosing og winning tone depending on if you beat the score
  if (score > highScore)
  {
    EEPROM.write(EEPROMhighscore, score); //Writes current score as highscore if it is higher than previous
    tone (3, NOTE_D5, 150);
    delay(150);
    tone (3, NOTE_E5, 150);
    delay(150);
    tone (3, NOTE_F5, 150);
    delay(150);
    tone (3, NOTE_G5,750);
    m.writeSprite(0,0,smileYes);
  } else if (score <= highScore)
  {
    tone (3, NOTE_G4, 150);
    delay(150);
    tone (3, NOTE_F4, 150);
    delay(150);
    tone (3, NOTE_E4, 150);
    delay(150);
    tone (3, NOTE_D4,750);
    m.writeSprite(0,0,smileNo);
  }

  valSw = digitalRead(A2); //Cheks if joystick is pressed
  while (valSw) //Goes into a loop until joystick is pressed
  {
    lcd.clear();
    
    timeNow = millis();
    valSw = digitalRead(A2); //Cheks if joystick is pressed
    while ((millis() < timeNow + 2000) && valSw) //Shows score for 2sec
    { 
      if (score > highScore)
      { 
        lcd.setCursor(0,0);
        lcd.print("NEW HIGH SCORE!");
        lcd.setCursor(3,1);
        lcd.print(score);
      } else if (score <= highScore)
      {
        lcd.setCursor(0,0);
        lcd.print("You got:");
        lcd.setCursor(8,0);
        lcd.print(score);
        lcd.setCursor(0,1);
        lcd.print("High score:");
        lcd.setCursor(11,1);
        lcd.print(highScore);
      }
      valSw = digitalRead(A2);
    }

    timeNow = millis();
    valSw = digitalRead(A2); //Cheks if joystick is pressed
    
    while ((millis() < timeNow + 2000) && valSw) //Shows reset instruction for 2sec
    {
      lcd.setCursor(0,0);
      lcd.print("Press joystick");
      lcd.setCursor(0,1);
      lcd.print("to play again...");
      valSw = digitalRead(A2);
    }
  }

  //resets game when joystick is pressed
  anim = 1;
  play = 1;
  score = 0;
  foeY  = 0; //Starts enemy at top
  foeX  = posX;
  foeY2 = 8; //Starts enemy at bottom to reset its postion
  foeX2 = -1;
  foeY3 = 8; //Starts enemy at bottom to reset its postion
  foeX3 = -1;
  foeY4 = 8; //Starts enemy at bottom to reset its postion
  foeX4 = -1;
  spd   = 80;
  loop();
}
