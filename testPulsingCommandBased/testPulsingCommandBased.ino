/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Things to do: 1) Make stepper acceleration ramp (most likely through a function of delay() length decreases).
 2) add in the limit switch abort to the moveMotor() command
 3) Add in the z-direction motor
 
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// This is our attempt at making the code command - based through serial port.

// Pins connecting to Arduino:
int directPin = 7;      //This pin sets the directionPin for the motor to move.  We may need two of these outputs for the z-directionPin.
int xMotor = 6;         //This pin is the pulse output for the X-direction motor.  Again, we may need two of these pins.
int limitSwitch = 12;   //We will be using this pin for the limit switch motor stop function.
int trigger = 13;       //This pin will be used to trigger the spectrometer.
int interrupt = A0;      //The interrupt pin used for the limit switches on the stage so that nothing can break.  Used to abort moveMotor() function.

int steps = 0;          //This is the number of steps we'll move the motor between each measurement.  Default to zero steps for safety.
int directVar = 0;      //This variable is for the direction of movement. 1 = right (into the beam / away from the motor);
boolean directCondition = false; //The variable used to verify the direction of the motor movement.
boolean stepVerify = false; //The variable used to verify the stepsize before moving and setting the number of steps.
boolean dataCondition = false;
int xDataPoints = 0; 
int variable = 1;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SETUP
 
 Setting up the Arduino board and program for use.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void setup() {
  pinMode(interrupt, INPUT);
  pinMode(directPin, OUTPUT);
  pinMode(xMotor, OUTPUT);
  pinMode(limitSwitch, INPUT);
  pinMode(trigger, OUTPUT);
  digitalWrite(trigger, LOW);
  
  delay(1000);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("\nEnter commands like this:");
  Serial.println("Command, stepSize, steps. [Yes, include commas and period at the end]\n");
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 BODY
 
 The body of our Arduino action.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void loop() {
  // from http://stackoverflow.com/questions/5697047/convert-serial-read-into-a-useable-string-using-arduino
  String command = "";
  String param1 = 0;
  String param2 = 0;

    if(Serial.available() > 0)
    {
        command = Serial.readStringUntil(',');
        stepSize = Serial.readStringUntil(',');
        steps = Serial.readStringUntil('.');
        runCommand(command, param1, param2);
    }
}

void printHelp() {
  Serial.println("\n\nWelcome to our help menu!\n");
  Serial.println("----- Available Commands: -----");
  Serial.println("GetSettings ()");
  Serial.println(" - print all settings variables to the screen.");
  Serial.println("MoveMotor ()");
  Serial.println(" - params:\n");
  Serial.println("MoveToZero ()");
  Serial.println(" - params:\n");
  Serial.println("SetDirection (char dir)");
  Serial.println(" - params: dir = (Left / Right) direction for motor to initially move.");
  Serial.println(" - note: right = into beam -or- away from motor.\n");
  Serial.println("SetMeasurementNumber (int pointsToTake)");
  Serial.println(" - params: pointsToTake = number of spectra to collect.\n");
  Serial.println("SetSteps (int steps)");
  Serial.println(" - params: steps = # steps for motor to move between each measurement.\n");
  Serial.println("\n\nType a command!");
  
  // Other possibly useful commands:
  // Serial.println("ReturnMotor ()");
  // Serial.println(" - params: not sure.");
  // Serial.println("MoveTo (location)");
  // Serial.println(" - params: location = where to move motor to. (possibly 2D)");
  // Serial.println("TakeSpectrum ()");
  // Serial.println(" - takes spectrum at current location.");
}

void runCommand(String cmd, String p1, String p2) {
    int err = 0;
    cmd.toLowerCase();
    if ( cmd.equals("help") ) {
      printHelp();
    } else if ( cmd.equals("getsettings") ) {
      // GetSettings();
    } else if ( cmd.equals("movemotor") ) {
      // MoveMotor();
    } else if ( cmd.equals("movetozero") ) {
      // MoveToZero();
    } else if ( cmd.equals("setdirection") ) {
      // SetDirection();
    } else if ( cmd.equals("setmeasurementnumber") ) {
      // SetMeasurementNumber();
    } else if ( cmd.equals("setsteps") ) {
      // SetSteps();
    } else {
      if (cmd == "") {
        Serial.println("Weird, your command did not register!");
        err++;
      }
      if (ss == 0) {
        Serial.println("Please enter a valid step size! (2nd param)");
        err++;
      }
      if (s == 0) {
        Serial.println("Please enter a valid number of steps! (3rd param)");
        err++;
      }
      // If there is an error, no command will be run.
      if (err == 0) {
        if (cmd.equalsIgnoreCase("MoveMotor") ) {
          
        } else if (cmd.equalsIgnoreCase("MoveToZero") ) {
          
        }
      }
      Serial.print("\nCommand ");
      Serial.print(cmd);
      Serial.print(": ");
      Serial.print(ss);
      Serial.print(", ");
      Serial.print(s);
      Serial.print("\n\n");
    }
}

  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   EXECUTE CODE
   
   This code combines all of the commands together and runs a program.
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

  void ExecuteMeasurement() {
    boolean xMeasureExecute = false;
    dataPoints();
    selectDirection();
    setSteps();
    while (!xMeasureExecute) {
      Serial.println("Run. . .? (Y/n)");
      while (!(Serial.available() > 0)) {
        //wait 
      }
      int c = Serial.read();
      int g = 1;
      int h = 1;
      if (c == 'y' || c == 'Y') {
        for (int i = xDataPoints; i > 0; i--) {
          g = moveMotor(directVar,steps,xMotor);
          delay(100);
          specTrigger();
          delay(100);
        }
        
        h = returnMotor(directVar,steps,xDataPoints,xMotor);
        if (g == 0 || h == 0) {
          Serial.println("\n\n\n ERROR: The movement was exitted due to a failure to communicate.");
        }
        xMeasureExecute = true;
      } 
      else if (c == 'n' || c == 'N') {
        Serial.println("Why not? You should close the serial port and start over now. \n\n");
      } 
      else {

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

int moveMotor(int dir, int stepNumber, int motorChoice) {
  digitalWrite(directPin,dir);
  Serial.print("Moving the stepper motor in the ");
  Serial.print(dir);
  Serial.println(" direction.");
  delay(80);
  for (int i = stepNumber; i > 0; i--) {
    // if (analogRead(interrupt) < 0) {    //This is the interrupt code for the limit switch.  If the stage is ever moving and there is a voltage across the 
    //   return 0;                         //the limit switch pin, the code for move will stop. We need to figure out how to write measure the limit voltage. 
    // }                                   //(using the circuit Dr. Durfee and I  talked about).
    // Alex's idea:
    // - When interrupt happens, assign a variable to false
    // - in this loop, we continue to check if this variable is true. Once it goes
    //   false, we stop looping and print out the error to the console log.

    digitalWrite(motorChoice,HIGH);
    delay(100);
    digitalWrite(motorChoice,LOW);
    delay(100);
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

int returnMotor(int dir, int stepNumber, int dataPoints, int motorChoice) {
  boolean returnCondition = false;
  while(!returnCondition) {
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
  }
  int stepLength = stepNumber * dataPoints;
  Serial.print("Returning the stepper motor from the ");
  Serial.print(dir);
  Serial.print(" direction, by ");
  Serial.print(stepLength);
  Serial.println(" steps");
  dir = !dir;
  int g = moveMotor(dir,stepLength,motorChoice);
  directCondition = false;
  stepVerify = false;
  dataCondition = false;
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
  digitalWrite(trigger,HIGH);
  delay(100);                  
  digitalWrite(trigger,LOW);
  delay(100);
}
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 DIRECTION SELECTION 
 
 This function asks for and sets the direction of the motor
 through the serial monitor.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void selectDirection() {
  char dir;
  while(!directCondition) {
    Serial.println("\n\n\nWhat direction? (l/L or r/R for left or right) (right = into the beam)");
    while(!(Serial.available() > 0)) {
      //wait
    }
    char c = Serial.read();
    if (c=='l' || c=='L') {
      directVar = 0;
      directCondition = true;
    }
    else if(c=='r' || c=='R') {
      directVar = 1;
      directCondition = true;
    }
    dir = c;
  }
  if(directCondition) {
    Serial.print("You've selected the ");
    Serial.print(dir);
    Serial.println(" direction");  
  }
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SET NUMBER OF DATA POINTS
 
 This function prompts the user for the number of data points to take, then
 sets this number for the return, and the move/trigger execution.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void dataPoints() {
  while(!dataCondition) {
    Serial.println("\n\n\nHow many spectra will you collect?");
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

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 INCREMENT SIZE (BETWEEN MEASUREMENTS)
 
 Function to set the number of steps that the motor will move.  
 This will be based on the stepsize measurement on the driver.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void  setSteps() {
  char sizeCheck = 'n';
  while(!stepVerify) {
    while(sizeCheck != 'y' && sizeCheck != 'Y') {
      Serial.println("\n\n\nHave you checked the step size.....?");
      while(!(Serial.available() > 0)) {
        //wait
      }
      sizeCheck = Serial.read();
      Serial.println(sizeCheck);
    }
    Serial.println("\n\n\nHow many steps per increment?");
    while(!(Serial.available() > 0)) {
      //wait
    }
    int c = Serial.parseInt();
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
 
void GetSettings() {
  Serial.println("\n\n====== Current System Parameters: ======");
  // directVar:
  Serial.print("Direction: ");
  if (directVar == 0) {
    Serial.println("0 / Left / Out of Beam / Toward Motor.");
  } else if (directVar == 1) {
    Serial.println("1 / Right / Into Beam / Away From Motor.");
  } else {
    Serial.print("Not a valid number - ");
    Serial.println(directVar);
  }
  // xDataPoints:
  Serial.print("Number of Measurements [x-axis]: ");
  Serial.println(xDataPoints);
  // steps:
  Serial.print("Steps per Measurement: ");
  Serial.println(steps);
}

