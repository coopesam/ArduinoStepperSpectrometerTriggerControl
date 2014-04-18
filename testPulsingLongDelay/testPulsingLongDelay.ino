/* 2D Stepper Motor Pulsed Spectrometer
 * 
 * Drives two stepper motors for a two-dimensional reach.
 * Also sends pulses to spectrometer for data collection,
 * spectrometer = Avaspec-3648
 *
 * Authors:
 * - Alex Beaman
 * - Samuel Cooper
 */

int xDirectPin = 7;      //This pin sets the x Direction Pin for the x motor to move.
int zDirectPin = 9;      //This pin sets the z Direction Pin for the z motor
int xMotor = 6;         //This pin is the pulse output for the X-direction motor.
int zMotor = 8;
int xSteps = 0;          //This is the number of steps we'll move the motor.  Default to zero steps for safety.
int zSteps = 0;
// 0 = 0th interrupt, but on pin 2.
// 1 = 1st interrupt, but on pin 3. [didn't work until using correct circuit - pull up resistors and such.
int limitSwitchWhite = 0;   //We will be using this pin for the limit switch motor stop function.  
int limitSwitchBlue = 1;
int trigger = 13;       //This pin will be used to trigger the spectrometer. 
int xDirectVar = 0;      //This variable is either 0 or 1 or true or false, which will be read as the direction of the motor.
int zDirectVar = 0;

int xDataPoints = 0;
int zScans = 0;

volatile boolean systemOkay = true; // volatile means variable could change at any moment.


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SETUP
 
 Setting up the Arduino board and program for use.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void setup() {
  // Direction pins:
  pinMode(xDirectPin,OUTPUT);
  pinMode(zDirectPin,OUTPUT);
  // Stepper motor pins:
  pinMode(xMotor,OUTPUT);
  pinMode(zMotor,OUTPUT);
  // Interrupts for limit switches:
  //attachInterrupt(limitSwitchBlue, closeLimitHit, FALLING);
  //attachInterrupt(limitSwitchWhite, farLimitHit, FALLING);
  // Spectrometer pin:
  pinMode(trigger,OUTPUT);
  digitalWrite(trigger,LOW);
  delay(1000);
  Serial.begin(115200);
  while(!Serial);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 BODY
 
 The body of our Arduino action.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void loop() {
  ExecuteMeasurement();
}

void closeLimitHit() {
  systemOkay = false;
  Serial.println("\n\n========Close Limit Hit!!!!!!!==========\n\n");
}

void farLimitHit() {
  systemOkay = false;
  Serial.println("\n\n========Far Limit Hit!!!!!!!==========\n\n");
}

  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   EXECUTE CODE
   
   This code combines all of the commands together and runs a program.
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void ExecuteMeasurement() {
  boolean execute = false;
    dataPoints();
    setZScans();
    selectXDirection();
    selectZDirection();
    setXSteps();
    setZSteps();
    while (!execute) {
      Serial.println("Run. . .? (Y/n)");
      while (!(Serial.available() > 0)) {
        //wait 
      }
      int c = Serial.read();
      int g = 1;
      int h = 1;
      if (c == 'y' || c == 'Y') {
        for (int i = zScans; i > 0; i--) {
          for (int i = xDataPoints; i > 0; i--) {
            g = moveMotor(xDirectVar,xDirectPin,xSteps,xMotor);
            delay(100);
            specTrigger();
            delay(100);
          }
          
          h = returnMotor(xDirectVar,xDirectPin,xSteps,xDataPoints,xMotor);
          if (g == 0 || h == 0) {
            Serial.println("\n\n\n ERROR: The movement was exitted due to a failure to communicate.");
          }
          moveMotor(zDirectVar,zDirectPin,zSteps,zMotor);
        }
        
        returnMotor(zDirectVar,zDirectPin,zSteps,zScans,zMotor);
        
        execute = true;
      } 
      else if (c == 'n' || c == 'N') {
        Serial.println("Why not? You should close the serial port and start over now. \n\n");
      } 
      else {
        Serial.println("Unrecognized command");
      }
    } 
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 MOVE MOTOR
 
 This function is what pulses the motor.  The arguments are
 the direction to move, the number of steps, and which motor we
 are moving.  For now the only motor we will control is the
 x-direction (xMotor).  When this function is called, it will
 first write the direction, then begin then move the motor
 by the set number of steps.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

int moveMotor(int dir, int dirPin, int stepNumber, int motorChoice) {
  digitalWrite(dirPin,dir);
  Serial.print("Moving the stepper motor in the ");
  Serial.print(dir);
  Serial.println(" direction.");
  delay(80);
  for (int i = stepNumber; i > 0; i--) {
    if (systemOkay) { // systemOkay is the variable to make sure the motor is not going to hit and end.
      digitalWrite(motorChoice,HIGH);
      delay(10);
      digitalWrite(motorChoice,LOW);
      delay(10);
    }
  }
  return 1;
}
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 MOTOR RETURN
 
 The next function returns the motor the the previous position
 before measurements were taken.
 When we are able to get a distance measurement feedback,
 we should create a return home position.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

int returnMotor(int dir,int dirPin, int stepNumber, int dataPoints, int motorChoice) {
  boolean returnCondition = false;
  /*while(!returnCondition) {
    Serial.println("\n\n\nReady to return motor?");
    while(!(Serial.available() > 0)) {
      //wait
    }
    char c = Serial.read();
    if (c=='y' || c=='Y') {
      returnCondition = true;
    }
    else {
      Serial.println('\n\n\n\n\n\n\n\nBUMMER!');
      returnCondition = false;
    }
  }*/
  int stepLength = stepNumber * dataPoints;
  Serial.print("Returning the stepper motor from the ");
  Serial.print(dir);
  Serial.print(" direction, by ");
  Serial.print(stepLength);
  Serial.println(" steps");
  int newDir = !dir;
  int g = moveMotor(newDir,dirPin,stepLength,motorChoice);
  return g;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SPECTROMETER TRIGGER
 
 The following function is the trigger pulse to the spectrometer.
 A pulse will be sent out after every movement.  
 Delays should be used in the void loop when calling it out.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
void specTrigger() {
  Serial.println("Triggering the spectrometer. . .");
  delay(100);
  if (systemOkay) {
    digitalWrite(trigger,HIGH);
    delay(100);                  
    digitalWrite(trigger,LOW);
    delay(100);
  }
}
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 DIRECTION SELECTION 
 
 This function asks for and sets the direction of the motor
 through the serial monitor.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void selectXDirection() {
  boolean directCondition = false;
  char dir;
  while(!directCondition) {
    Serial.println("\n\n\nWhat direction (x-axis)? (l/L or r/R for left or right) (right = into the beam)");
    while(!(Serial.available() > 0)) {
      //wait
    }
    char c = Serial.read();
    if (c=='l' || c=='L') {
      xDirectVar = 0;
      directCondition = true;
    }
    else if(c=='r' || c=='R') {
      xDirectVar = 1;
      directCondition = true;
    }
    dir = c;
  }
  if(directCondition) {
    Serial.print("You've selected the ");
    Serial.print(dir);
    Serial.println(" direction for x");  
  }
}

void selectZDirection() {
  boolean directCondition = false;
  char dir;
  while(!directCondition) {
    Serial.println("\n\n\nWhat direction (z-axis)? (l/L or r/R for left or right) (right = forward / away from motor)");
    while(!(Serial.available() > 0)) {
      //wait
    }
    char c = Serial.read();
    if (c=='l' || c=='L') {
      zDirectVar = 1;
      directCondition = true;
    }
    else if(c=='r' || c=='R') {
      zDirectVar = 0;
      directCondition = true;
    }
    dir = c;
  }
  if(directCondition) {
    Serial.print("You've selected the ");
    Serial.print(dir);
    Serial.println(" direction for z");  
  }
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SET NUMBER OF DATA POINTS
 
 This function prompts the user for the number of data points to take, then
 sets this number for the return, and the move/trigger execution.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void dataPoints() {
  boolean dataCondition = false;
  while(!dataCondition) {
    Serial.println("\n\n\nHow many spectra will you collect? (x-axis)");
    while(!(Serial.available() > 0)) {
      //wait
    }
    xDataPoints = Serial.parseInt();
    if (xDataPoints > 0) {
      dataCondition = true;
      Serial.print("You will take ");
      Serial.print(xDataPoints);
      Serial.println(" data points at this Z-position.");
    }
  }
}

void setZScans() {
  boolean dataCondition = false;
  while(!dataCondition) {
    Serial.println("\n\n\nHow many different z scans do you want to take?");
    while(!(Serial.available() > 0)) {
      //wait
    }
    zScans = Serial.parseInt();
    if (zScans > 0) {
      dataCondition = true;
      Serial.print("You will take ");
      Serial.print(zScans);
      Serial.println(" sets of spectra.");
    }
  }
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 INCREMENT SIZE (BETWEEN MEASUREMENTS)
 
 Function to set the number of steps that the motor will move.  
 This will be based on the stepsize measurement on the driver.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void  setXSteps() {
  boolean stepVerify = false;
  char sizeCheck = 'n';
  while(!stepVerify) {
    while(sizeCheck != 'y' && sizeCheck != 'Y') {
      Serial.println("\n\n\nHave you checked the x step size.....?");
      while(!(Serial.available() > 0)) {
        //wait
      }
      sizeCheck = Serial.read();
      Serial.println(sizeCheck);
    }
    Serial.println("\n\n\nHow many x steps per increment?");
    while(!(Serial.available() > 0)) {
      //wait
    }
    int c = Serial.parseInt();
    if (c > 0 && c < 1000000) {
      xSteps = c;
      stepVerify = true;
      Serial.print("You've set ");
      Serial.print(xSteps);
      Serial.print(" steps in the ");  
      Serial.print(xDirectVar);
      Serial.println(" x direction.");
    }
  }
}

void  setZSteps() {
  boolean stepVerify = false;
  char sizeCheck = 'n';
  while(!stepVerify) {
    while(sizeCheck != 'y' && sizeCheck != 'Y') {
      Serial.println("\n\n\nHave you checked the z step size.....?");
      while(!(Serial.available() > 0)) {
        //wait
      }
      sizeCheck = Serial.read();
      Serial.println(sizeCheck);
    }
    Serial.println("\n\n\nHow many steps between each z position?");
    while(!(Serial.available() > 0)) {
      //wait
    }
    int c = Serial.parseInt();
    if (c > 0 && c < 1000000) {
      zSteps = c;
      stepVerify = true;
      Serial.print("You've set ");
      Serial.print(zSteps);
      Serial.print(" steps in the ");  
      Serial.print(zDirectVar);
      Serial.println(" z direction.");
    }
  }
}
