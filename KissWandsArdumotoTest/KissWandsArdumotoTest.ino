    /* Ardumoto Example Sketch
  by: Jim Lindblom
  date: November 8, 2013
  license: Public domain. Please use, reuse, and modify this 
  sketch!

  Adapted to v20 hardware by: Marshall Taylor
  date: March 31, 2017

  Three useful functions are defined:
    setupArdumoto() -- Setup the Ardumoto Shield pins
    driveArdumoto([motor], [direction], [speed]) -- Drive [motor] 
      (0 for A, 1 for B) in [direction] (0 or 1) at a [speed]
      between 0 and 255. It will spin until told to stop.
    stopArdumoto([motor]) -- Stop driving [motor] (0 or 1).

  setupArdumoto() is called in the setup().
  The loop() demonstrates use of the motor driving functions.
*/

// Clockwise and counter-clockwise definitions.
// Depending on how you wired your motors, you may need to swap.
#define FORWARD  0
#define REVERSE 1

// Motor definitions to make life easier:
#define MOTOR_A 0
#define MOTOR_B 1

// Pin Assignments //
//Default pins:
#define DIRA 2 // Direction control for motor A
#define PWMA 3  // PWM control (speed) for motor A
#define DIRB 4 // Direction control for motor B
#define PWMB 11 // PWM control (speed) for motor B

// Makey trigger definitions
int makeyPin = 10; //DDT
int makeyTouch = 0; //DDT
int counter = 0;


////Alternate pins:
//#define DIRA 8 // Direction control for motor A
//#define PWMA 9 // PWM control (speed) for motor A
//#define DIRB 7 // Direction control for motor B
//#define PWMB 10 // PWM control (speed) for motor B


void setup()
{
  Serial.begin(57600);
  setupArdumoto(); // Set all pins as outputs
}

void loop()
{

makeyTouch = digitalRead(makeyPin);   // read the input pin

if ((makeyTouch == HIGH) && (counter <=1)) {

  // Drive motor A and motor B at random speeds betw/ 148 to 201 speed values (2.2V - 3.0V)
//// Motor chart
// 140: 2.17V
// 160: 2.45V
// 190: 2.86V
// 201: 3.00V

  int pwmVal = random(53) + 148; // random of difference betw max & min, + min; 201 = 3V
  //drive motors Reverse; Forward does not deliver voltage
  driveArdumoto(MOTOR_A, REVERSE, pwmVal); // Set motor A to REVERSE at max
  driveArdumoto(MOTOR_B, REVERSE, pwmVal);  // Set motor A to REVERSE at half
  Serial.println(pwmVal);
  delay(2000);  // Run for 1/2 second
  stopArdumoto(MOTOR_A);  // STOP motors for 1/10th of a second
  stopArdumoto(MOTOR_B);
  delay( 1000 );

  }
}

// driveArdumoto drives 'motor' in 'dir' direction at 'spd' speed
void driveArdumoto(byte motor, byte dir, byte spd)
{
  if (motor == MOTOR_A)
  {
    digitalWrite(DIRA, dir);
    analogWrite(PWMA, spd);
  }
  else if (motor == MOTOR_B)
  {
    digitalWrite(DIRB, dir);
    analogWrite(PWMB, spd);
  }  
}

// stopArdumoto makes a motor stop
void stopArdumoto(byte motor)
{
  driveArdumoto(motor, 0, 0);
}

// setupArdumoto initialize all pins
void setupArdumoto()
{
  // All pins should be setup as outputs:
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  // Initialize all pins as low:
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}
