#include <SparkFun_MMA8452Q.h>

MMA8452Q accel;
const int ROW1 = 2;
const int ROW2 = 3;
const int ROW3 = 4;
const int ROW4 = 5;
const int ROW5 = 6;
const int ROW6 = 7;
const int ROW7 = 8;
const int COL1 = 9;
const int COL2 = 10;
const int COL3 = 11;
const int COL4 = 12;
const int COL5 = 13;
const int victoryLength = 4000;
const int roundLength = 15000;
const int deltaShow = 2;
const int deltaPred = 125; // 1000/8=125, flash on&off is one cycle, one 4 times, off 4 times, total of 8
const int deltaPrey = 150;
unsigned long time;
unsigned long nextTimeShow;
unsigned long nextTimePred;
unsigned long nextMovePrey;
unsigned long lastVictory;
unsigned long counter;
boolean right; // first time is true
boolean resetBoard = true;
int preyX;
int preyY;
int predX;
int predY;
int countPred;
int countPrey;
int overallScore;
int scoreFirst;
int scoreSecond;

void setup() {
  Serial.begin(9600);
  pinMode(ROW1, OUTPUT);
  pinMode(ROW2, OUTPUT);
  pinMode(ROW3, OUTPUT);
  pinMode(ROW4, OUTPUT);
  pinMode(ROW5, OUTPUT);
  pinMode(ROW6, OUTPUT);
  pinMode(ROW7, OUTPUT);
  pinMode(COL1, OUTPUT);
  pinMode(COL2, OUTPUT);
  pinMode(COL3, OUTPUT);
  pinMode(COL4, OUTPUT);
  pinMode(COL5, OUTPUT);
  turnOff();
  accel.init();
  setPos();
}
void loop () {
  if (accel.available()) {
    accel.read(); // read the accelerometer data
  }
  char predCount = Serial.read(); //passing in data from Java
  byte preyCount = accel.readPL(); // passing in "orientation" data from accelerometer. To read the portrait/landscape data from the accelerometer, use the readPL() function. This function returns a byte, which will either be equal to PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L, or LOCKOUT.
  time = millis(); // time starts running
  if (resetBoard) { // starts true
    setPos(); // sets random positions
    resetBoard = false; // sets to false
  }
  if (time > nextMovePrey) { // using delta time to move prey here (nextMovePrey starts 0)
    movePrey(preyCount); //move prey based off of data retrieved from accelerometer
    nextMovePrey += deltaPrey; // increase time difference by this much so prey can't move until after this amount of time has passed
  }
  movePred(predCount); // using passed in data from Java to move pred 
  if (time > nextTimeShow) { // (nextTimeShow) starts 0
    turnOff();
    showCol();
    nextTimeShow += deltaShow;
  }
  detectVictory();
}

//Turns all LEDs off
void turnOff() {
  digitalWrite(ROW1, LOW);
  digitalWrite(ROW2, LOW);
  digitalWrite(ROW3, LOW);
  digitalWrite(ROW4, LOW);
  digitalWrite(ROW5, LOW);
  digitalWrite(ROW6, LOW);
  digitalWrite(ROW7, LOW);
  digitalWrite(COL1, HIGH);
  digitalWrite(COL2, HIGH);
  digitalWrite(COL3, HIGH);
  digitalWrite(COL4, HIGH);
  digitalWrite(COL5, HIGH);
}

// Sets random locations pred and prey
void setPos() {
  preyX = random(0,5);
  preyY = random(0,7);
  predX = random(0.4);
  predY = random(0,6);
  while ((preyX == predX || preyX == predX+1 || preyX == predX + 2 || preyX == predX - 1)
      && (preyY == predY || preyY == predY + 1 || preyY == predY + 2 || preyY == predY - 1)) {
        predX = random(0, 4);
        predY = random(0, 6);
      }
}

// Draws actual pred and prey of game
void showCol() {
  digitalWrite(COL1 + (counter % 5), LOW);

  //draw the prey!
  if (preyX == (counter % 5))
  digitalWrite(ROW1 + preyY, HIGH);

  //draw the predator!
  if (time > nextTimePred) {
    //turns on
    if (predX == (counter % 5) || predX + 1 == (counter % 5)) {
      digitalWrite(ROW1 + predY, HIGH);
      digitalWrite(ROW2 + predY, HIGH);
      // turns off
      if (right && time - nextTimePred > deltaPred) {
        nextTimePred = time;
        nextTimePred += deltaPred;
      }
      right = !right;
    }
  }
  counter++;
}

// Moves prey
void movePrey(byte control) {
  switch (control) {
    // Down
    case PORTRAIT_D:
      if (preyY != 0) {
        preyY--;
      }
      break;

    // Up
    case PORTRAIT_U:
      if (preyY != 6) {
        preyY++;
      }
      break;

    // Left
    case LANDSCAPE_L:
      if (preyX != 0) {
        preyX--;
      }
      break;

    //Right
    case LANDSCAPE_R:
      if (preyX != 4) {
        preyX++;
      }
      break;

    //No movement
    case LOCKOUT:
       break;
  }
}

// Moves just predator depending on bytes sent from java to arduino
void movePred(char control) {
  switch (control) {
    case 'w': // up
      if (predY != 0)
        predY--;
        break;

    case 's': // down
      if (predY != 5)
        predY++;
        break;

    case 'a': // left
      if (predX != 0)
        predX--;
        break;

    case 'd': //right
      if (predX != 3)
        predX++;
        break;
  }
}

//Detects Victory
void detectVictory() {
  time = millis();
  // Pred wins
  if ((preyX == predX || preyX == predX + 1) && (preyY == predY || preyY == predY + 1) && (time - lastVictory) < roundLength) {
    countPred++;
    lastVictory = time;
    predX = 1;
    predY = 2;
    preyX = 5;
    preyY = 7;
    while (true) {
      turnOff();
      showScore();
      delay(2);
      if (millis() - lastVictory > victoryLength)
        break;
    }
    resetBoard = true;
  }
  //Prey wins
  else if (time - lastVictory > roundLength) {
    countPrey++;
    lastVictory = time;
    preyX = 2;
    preyY = 2;
    predX = 5;
    predY = 7;
    while (true) {
      turnOff();
      showScore();
      delay(2);
      if (millis() - lastVictory > victoryLength) {
        break;
      }
    }
    resetBoard = true;
  }
}

// Sets positions of scores
int calcScore() {
  // Predator Victory
  if (predX == 1) {
    overallScore = countPred - countPrey;
    //Winning 1-2
    if (overallScore > 0 && overallScore < 3) {
      scoreFirst = 1;
      scoreSecond = 0;
    }
    //Losing 1-2
    else if (overallScore < 0 && overallScore > -3) {
      scoreFirst = -1;
      scoreSecond = 0;
    }
    //Winning 3+
    else if (overallScore > 2) {
      scoreFirst = 1;
      scoreSecond = 2;
    }
    //Losing 3+
    else if (overallScore < -2) {
      scoreFirst = -1;
      scoreSecond = -2;
    }
    //Tied
    else {
      scoreFirst = 0;
      scoreSecond = 0;
    }
  }
  //Prey Victory
  else {
    overallScore = countPrey - countPred; // this calculates the difference between the score (the ratio)
    //Winning 1-2
    if (overallScore > 0 && overallScore < 3) {
      scoreFirst = 1;
      scoreSecond = 0;
    }
    // Losing 1-2
    else if (overallScore < 0 && overallScore > -3) {
      scoreFirst = -1;
      scoreSecond = 0;
    }
    //Winning 3+
    else if (overallScore > 2) {
      scoreFirst = 1;
      scoreSecond = 2;
    }
    // Losing 3+
    else if (overallScore < -2) {
      scoreFirst = -1;
      scoreSecond = -2;
    }
    //Tied
    else {
      scoreFirst = 0;
      scoreSecond = 0;
    }
  }
}

// Drawing during victory/score screen
void showScore() {
  overallScore = calcScore();
  digitalWrite(COL1 + (counter % 5), LOW);
  if (preyX == (counter % 5)) {
    digitalWrite(ROW3, HIGH);
  }
  else if(predX == (counter % 5) || predX + 1 == (counter % 5)) {
    digitalWrite(ROW3, HIGH);
    digitalWrite(ROW4, HIGH);
  }
  if (2 == (counter % 5) || scoreFirst + 2 == (counter % 5) || scoreSecond + 2 == (counter % 5))
    digitalWrite(ROW7, HIGH);
    counter++;
}

