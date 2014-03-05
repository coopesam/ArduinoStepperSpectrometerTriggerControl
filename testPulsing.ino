int directPin = 3;      //This pin sets the directPinion for the motor to move.  We may need two of these outputs for the z-directPinion.
int xMotor = 2;         //This pin is the pulse output for the motor.  Again, we may need two of these pins.
int steps = 0;          //This is the number of steps we'll move the motor.  Default to zero steps for safety.
int limitSwitch = 12;   //We will be using this pin for the limit switch motor stop function.  
int trigger = 13;       //This pin will be used to trigger the spectrometer. 
int directVar = 0;      //This variable is either 0 or 1 or true or false, which will be read as the direction of the motor.
boolean directCondition = false; //The variable used to verify the direction of the motor movement.
boolean stepVerify = false; //The variable used to verify the stepsize before moving and setting the number of steps.


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Setting up the Arduino board and program for use.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void setup() {

  pinMode(directPin,OUTPUT);
  pinMode(xMotor,OUTPUT);
  pinMode(limitSwitch,INPUT);
  delay(1000);
  Serial.begin(115200);
  while(!Serial);

}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 The body of our Arduino action.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void loop() {
  selectDirection2();
  Serial.print(directVar);
  delay(500);
  setSteps();
  Serial.print(steps);
  delay(500);
  moveMotor(directVar,steps,xMotor);
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 This function is what pulses the motor.  The arguments are
 the direction to move, the number of steps, and which motor we
 are moving.  For now the only motor we will control is the
 x-direction (xMotor).  When this function is called, it will
 first write the direction, then begin then move the motor
 by the set number of steps.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void moveMotor(int dir, int stepNumber, int motorChoice) {
  digitalWrite(directPin,dir);
  delay(1);
  for (stepNumber; stepNumber > 0; stepNumber--) {
    digitalWrite(motorChoice,HIGH);
    delay(1);
    digitalWrite(motorChoice,LOW);
    delay(1);
  }
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 The following function is the trigger pulse to the spectrometer.
 A pulse will be sent out after every movement.  
 Delays should be used in the void loop when calling it out.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 This function asks for and sets the direction of the motor
 through the serial monitor.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void selectDirection2() {
  while(!directCondition) {
    Serial.println("What direction?");
    while(!(Serial.available() > 0)) {
      //wait
    }
    char c = Serial.read();
    if (c=='l' || c=='L') {
      directVar = 1;
      directCondition = true;
    }
    else if(c=='r' || c=='R') {
      directVar = 0;
      directCondition = true;
    }
  }
  if(directCondition) {
    Serial.print("You've selected the ");
    Serial.print(directVar);
    Serial.println(" direction");  
  }
}


/* My first attempt at creating a function that verfies inputs, and sets gobal values.

void selectDirection() { 
  if (!directCondition) {
    Serial.println("What direction?");
    while (!directCondition) {
      if(Serial.available() > 0 ){
        char c;
        c = Serial.read();
        switch (c) {
        case 'l':
        case 'L':
          directVar = 1;
          directCondition = true;
          break;
        case 'r':
        case 'R':
          directVar = 0;
          directCondition = true;
          break;
        default:
          Serial.println("Pick a value L/l or R/r for left or right");
          break;
        }
        if(directCondition){
          Serial.print("You've selected the ");
          Serial.print(directVar);
          Serial.println(" direction");
        }
      }
    }
  } 
  else {  
  }
}*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Function to set the number of steps that the motor will move.  
 This will be based on the stepsize measurement on the driver.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void  setSteps() {
  char sizeCheck = 'n';
  while(!stepVerify) {
    while(sizeCheck != 'y' && sizeCheck != 'Y') {
      Serial.println("Have you checked the step size.....?");
      while(!(Serial.available() > 0)) {
        //wait
      }
      sizeCheck = Serial.read();
      Serial.println(sizeCheck);
    }
    Serial.println("How many steps?");
    while(!(Serial.available() > 0)) {
      //wait
    }
    int c = Serial.read();
    if (c > 0 && c < 1000000) {
      steps = c;
      stepVerify = true;
      Serial.print("You've set ");
      Serial.print(steps);
      Serial.print(" steps in the ");  
      Serial.print(directVar);
      Serial.println(" direction.");
    }
  }
}








