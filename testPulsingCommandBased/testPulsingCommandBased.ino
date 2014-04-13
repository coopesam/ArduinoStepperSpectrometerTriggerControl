/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Things to do: 1) Make stepper acceleration ramp (most likely through a function of delay() length decreases).
 2) add in the limit switch abort to the moveMotor() command
 3) Add in the z-direction motor
 
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// This is our attempt at making the code command - based through serial port.

// Pins connecting to Arduino:
int directPin = 7;      //This pin sets the directionPin for the motor to move.  We may need two of these outputs for the z-directionPin.
int xMotor = 6;         //This pin is the pulse output for the X-direction motor.  Again, we may need two of these pins.
int limitSwitch = 12;   //We will be using this pin for the limit switch motor stop function.
int trigger = 13;       //This pin will be used to trigger the spectrometer.
int interrupt = A0;      //The interrupt pin used for the limit switches on the stage so that nothing can break.  Used to abort moveMotor() function.

int xSteps = 0;          //This is the number of steps we'll move the x motor between each measurement.  Default to zero steps for safety.
int zSteps = 0;          //This is the number of steps we'll move the z motor between each measurement.  Default to zero steps for safety.
int directVar = 0;      //This variable is for the direction of movement. 1 = right (into the beam / away from the motor);
boolean directCondition = false; //The variable used to verify the direction of the motor movement.
boolean stepVerify = false; //The variable used to verify the stepsize before moving and setting the number of steps.
boolean dataCondition = false;
int xDataPoints = 0;
int zDataPoints = 0;
int variable = 1;

char inData[20]; // Allocate some space for the string
char inChar=-1; // Where to store the character read
byte index = 0; // Index into array; where to store the character

String command = "";
String param1 = "";
String param2 = "";

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SETUP
 
 Setting up the Arduino board and program for use.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

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
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 BODY
 
 The body of our Arduino action.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
// from http://stackoverflow.com/questions/5697047/convert-serial-read-into-a-useable-string-using-arduino

//char[20] Comp(char* This) {
String Comp() {
    char data[20];
//void loop() {
    while (Serial.available() > 0) // Don't read unless
    {                              // there you know there is data
        if(index < 19) // One less than the size of the array
        {
            inChar = Serial.read(); // Read a character
            data[index] = inChar; // Store it
            index++; // Increment where to write next
            data[index] = '\0'; // Null terminate the string
        }
    }
    
    index = 0;
    
    return data;

    /*if (index != 0) {
        Serial.print("Run Commands now: ");
        Serial.println(inData);
        for (int i=0;i<19;i++) {
            inData[i]=0;
        }
        index=0;
    }*/
}

void loop()
{
    delay(500);
    command = Comp();
    if (inData != "") {
      Serial.println(command);
    }
    /*if (Comp("m1 on")==0) {
        Serial.write("Motor 1 -> Online\n");
    }
    if (Comp("m1 off")==0) {
        Serial.write("Motor 1 -> Offline\n");
    }
    if (Comp("help")==0) {
      PrintHelp();
    }*/
}

void PrintHelp() {
  Serial.println("\n\nWelcome to our help menu!\n");
  Serial.println("----- Available Commands: -----\n");
  Serial.println("GetSettings ()");
  Serial.println(" - print all settings variables to the screen.\n");
  Serial.println("GetSpectrum ()");
  Serial.println(" - gets spectrum at current location.\n");
  Serial.println("Help ()");
  Serial.println(" - prints functions available to be run.\n");
  Serial.println("MoveMotor ()");
  Serial.println(" - moves motor in x direction only AND not back & forth.");
  Serial.println(" - params:\n");
  Serial.println("RunMotor ()");
  Serial.println(" - MAIN RUNNING FUNCTION");
  Serial.println(" - runs motor forwards and backwards, according to steps, dir, pointsToTake, etc.\n");
  Serial.println("SetDirection (char dir)");
  Serial.println(" - params: dir = (Left / Right) direction for motor to initially move.");
  Serial.println(" - note: right = into beam -or- away from motor.\n");
  Serial.println("SetXMeasurements (int xPointsToTake)");
  Serial.println(" - params: xPointsToTake = number of spectra to collect in x axis.\n");
  Serial.println("SetZMeasurements (int zPointsToTake)");
  Serial.println(" - params: zPointsToTake = number of spectra to collect in z axis.\n");
  Serial.println("SetXSteps (int steps)");
  Serial.println(" - params: steps = # steps for motor to move between each measurement [x-axis].\n");
  Serial.println("SetZSteps (int steps)");
  Serial.println(" - params: steps = # steps for motor to move between each measurement [z-axis].\n");
  Serial.println("\n\nNow type a command!");
  
  // Other possibly useful commands:
  // Serial.println("ReturnMotor ()");
  // Serial.println(" - params: not sure.");
  // Serial.println("MoveTo (X, Y)");
  // Serial.println(" - params: location = where to move motor to. (possibly 2D)");
  // Serial.println("MoveToZero ()");
  // Serial.println(" - params:\n");
}

void runCommand(String cmd, String p1, String p2) {
    int err = 0;
    cmd.toLowerCase();
    if ( cmd.equals("help") ) {
      PrintHelp();
    } else if ( cmd.equals("getsettings") ) {
      GetSettings();
    } else if ( cmd.equals("getspectrum") ) {
      SpecTrigger();
    } else if ( cmd.equals("movemotor") ) {
      MoveMotor(1);
    } else if ( cmd.equals("runmotor") ) {
      MoveMotor(2);
    } else if ( cmd.equals("setdirection") ) {
      SetDirection(p1);
    } else if ( cmd.equals("setxmeasurements") ) {
      SetXMeasurements(p1);
    } else if ( cmd.equals("setzmeasurements") ) {
      SetZMeasurements(p1);
    } else if ( cmd.equals("setxsteps") ) {
      SetXSteps(p1);
    } else if ( cmd.equals("setzsteps") ) {
      SetZSteps(p1);
    } else {
      Serial.print("\nUnregistered Command: ");
      Serial.print(cmd);
      Serial.print(": ");
      Serial.print(p1);
      Serial.print(", ");
      Serial.print(p2);
      Serial.print("\n\n");
    }
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 EXECUTE CODE
 
 This code combines all of the commands together and runs a program.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// void ExecuteMeasurement() {
  // boolean xMeasureExecute = false;
  // dataPoints();
  // selectDirection();
  // setSteps();
  // while (!xMeasureExecute) {
  //   Serial.println("Run. . .? (Y/n)");
  //   while (!(Serial.available() > 0)) {
  //     //wait 
  //   }
  //   int c = Serial.read();
  //   int g = 1;
  //   int h = 1;
  //   if (c == 'y' || c == 'Y') {
  //     for (int i = xDataPoints; i > 0; i--) {
  //       g = moveMotor(directVar,steps,xMotor);
  //       delay(100);
  //       specTrigger();
  //       delay(100);
  //     }
      
  //     h = returnMotor(directVar,steps,xDataPoints,xMotor);
  //     if (g == 0 || h == 0) {
  //       Serial.println("\n\n\n ERROR: The movement was exitted due to a failure to communicate.");
  //     }
  //     xMeasureExecute = true;
  //   } 
  //   else if (c == 'n' || c == 'N') {
  //     Serial.println("Why not? You should close the serial port and start over now. \n\n");
  //   } 
  //   else {

  //   }
  // } 
// }

void RunMotor() {
  for (int i = xDataPoints; i > 0; i--) {
    MoveMotor(1);
    delay(100);
    SpecTrigger();
    delay(100);
  }
  
  // Return Motor:
  MoveMotor(1);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 MOVE MOTOR
 
 This function is what pulses the motor.  The arguments are
 the direction to move, the number of steps, and which motor we
 are moving.  For now the only motor we will control is the
 x-direction (xMotor).  When this function is called, it will
 first write the direction, then begin then move the motor
 by the set number of steps.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// int moveMotor(int dir, int stepNumber, int motorChoice) {
//   digitalWrite(directPin,dir);
//   Serial.print("Moving the stepper motor in the ");
//   Serial.print(dir);
//   Serial.println(" direction.");
//   delay(80);
//   for (int i = stepNumber; i > 0; i--) {
//     // if (analogRead(interrupt) < 0) {    //This is the interrupt code for the limit switch.  If the stage is ever moving and there is a voltage across the 
//     //   return 0;                         //the limit switch pin, the code for move will stop. We need to figure out how to write measure the limit voltage. 
//     // }                                   //(using the circuit Dr. Durfee and I  talked about).
//     // Alex's idea:
//     // - When interrupt happens, assign a variable to false
//     // - in this loop, we continue to check if this variable is true. Once it goes
//     //   false, we stop looping and print out the error to the console log.

//     digitalWrite(motorChoice,HIGH);
//     delay(100);
//     digitalWrite(motorChoice,LOW);
//     delay(100);
//   }
//   return 1;
// }

// New version - command based:

void MoveMotor(int directions) {
  // Tell motor which direction to move:
  digitalWrite(directPin, directVar);
  
  Serial.print("Moving the stepper motor in the ");
  if (directVar == 0) {
    Serial.println("0 / Left / Out of Beam / Toward Motor");
  } else if (directVar == 1) {
    Serial.println("1 / Right / Into Beam / Away From Motor");
  }
  Serial.println(" direction.");
  
  for (int i = 0; i < xSteps; i++) {
    // if (!atLimit) { // limit switches activate this variable
      digitalWrite(xMotor, HIGH);
      delay(100);
      digitalWrite(xMotor, LOW);
      delay(100);
    // }
  }

  if (directions == 2) {
    directVar = !directVar;
    MoveMotor(0);
  } else if (directions == 0) {
    directVar = !directVar;
  } else {
    Serial.println("Done.");
  }
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 MOTOR RETURN
 
 The next function returns the motor the the previous position
 before measurements were taken.
 When we are able to get a distance measurement feedback,
 we should create a return home position.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
// int returnMotor(int dir, int stepNumber, int dataPoints, int motorChoice) {
//   int stepLength = stepNumber * dataPoints;
//   Serial.print("Returning the stepper motor from the ");
//   Serial.print(dir);
//   Serial.print(" direction, by ");
//   Serial.print(stepLength);
//   Serial.println(" steps");
//   dir = !dir;
//   int g = moveMotor(dir,stepLength,motorChoice);
//   directCondition = false;
//   stepVerify = false;
//   dataCondition = false;
//   return g;
// }

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SPECTROMETER TRIGGER
 
 The following function is the trigger pulse to the spectrometer.
 A pulse will be sent out after every movement.  
 Delays should be used in the void loop when calling it out.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
void SpecTrigger() {
  // Serial.println("Triggering the spectrometer. . .");
  delay(100);
  digitalWrite(trigger,HIGH);
  delay(100);                  
  digitalWrite(trigger,LOW);
  delay(100);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 DIRECTION SELECTION 
 
 This function asks for and sets the direction of the motor
 through the serial monitor.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
// void selectDirection() {
//   char dir;
//   while(!directCondition) {
//     Serial.println("\n\n\nWhat direction? (l/L or r/R for left or right) (right = into the beam)");
//     while(!(Serial.available() > 0)) {
//       //wait
//     }
//     char c = Serial.read();
//     if (c=='l' || c=='L') {
//       directVar = 0;
//       directCondition = true;
//     }
//     else if(c=='r' || c=='R') {
//       directVar = 1;
//       directCondition = true;
//     }
//     dir = c;
//   }
//   if(directCondition) {
//     Serial.print("You've selected the ");
//     Serial.print(dir);
//     Serial.println(" direction");  
//   }
// }

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SET NUMBER OF DATA POINTS
 
 This function prompts the user for the number of data points to take, then
 sets this number for the return, and the move/trigger execution.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// void dataPoints() {
//   while(!dataCondition) {
//     Serial.println("\n\n\nHow many spectra will you collect?");
//     while(!(Serial.available() > 0)) {
//       //wait
//     }
//     xDataPoints = Serial.parseInt();
//     if (xDataPoints > 0) {
//       dataCondition = true;
//       Serial.print("You will take ");
//       Serial.print(xDataPoints);
//       Serial.println(" data points at this Z-position.");
//     }
//   }
// }

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 INCREMENT SIZE (BETWEEN MEASUREMENTS)
 
 Function to set the number of steps that the motor will move.  
 This will be based on the stepsize measurement on the driver.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// void  setSteps() {
//   char sizeCheck = 'n';
//   while(!stepVerify) {
//     while(sizeCheck != 'y' && sizeCheck != 'Y') {
//       Serial.println("\n\n\nHave you checked the step size.....?");
//       while(!(Serial.available() > 0)) {
//         //wait
//       }
//       sizeCheck = Serial.read();
//       Serial.println(sizeCheck);
//     }
//     Serial.println("\n\n\nHow many steps per increment?");
//     while(!(Serial.available() > 0)) {
//       //wait
//     }
//     int c = Serial.parseInt();
//     if (c > 0 && c < 1000000) {
//       steps = c;
//       stepVerify = true;
//       Serial.print("You've set ");
//       Serial.print(steps);
//       Serial.print(" steps in the ");  
//       Serial.print(directVar);
//       Serial.println(" direction.");
//     }
//   }
// }

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  GetSettings: PRINT SETTINGS TO SERIAL MONITOR
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
 
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
  // zDataPoints:
  Serial.print("Number of Measurements [z-axis]: ");
  Serial.println(zDataPoints);
  // x steps:
  Serial.print("Steps per Measurement [x-axis]: ");
  Serial.println(xSteps);
  // z steps:
  Serial.print("Steps per Measurement [z-axis]: ");
  Serial.println(zSteps);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  SetDirection: SET DIRECTION VARIABLE (duh)
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void SetDirection(String dir) {
  directVar = dir.toInt();
  Serial.print("\nDirection set to: ");
  Serial.print(dir);
  Serial.println("\n");
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  SetXMeasurements / SetZMeasurements: SET # OF MEASUREMENTS TO TAKE IN X OR Z DIRECTION
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void SetXMeasurements(String points) {
  xDataPoints = points.toInt();
  Serial.print("\nX data points set to: ");
  Serial.print(points);
  Serial.println("\n");
}

void SetZMeasurements(String points) {
  zDataPoints = points.toInt();
  Serial.print("\nZ data points set to: ");
  Serial.print(points);
  Serial.println("\n");
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  SetXSteps / SetZSteps: SET # OF STEPS BETWEEN MEASUREMENTS IN X OR Z DIRECTION
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void SetXSteps(String steps) {
  xSteps = steps.toInt();
  Serial.print("\nZ steps set to: ");
  Serial.print(steps);
  Serial.println("\n");
}

void SetZSteps(String steps) {
  zSteps = steps.toInt();
  Serial.print("\nZ steps set to: ");
  Serial.print(steps);
  Serial.println("\n");
}
