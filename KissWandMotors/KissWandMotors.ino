#define DC_MOTOR_PIN 3
#define LED_PIN 13
float reqVolt = 3; // Required Voltage, modify as desired
float peakVolt = sq(5);
float volt = 255 * sq(reqVolt) / peakVolt; // Duty Cycle = RequiredVoltage² / PeakVoltage² ; Multiply by 255 to get the analog write value
int makeyPin = 10; //DDT
int makeyTouch = 0; //DDT

void setup() {
  /* Initialize DC motor control pin as digital output */
  Serial.begin(57600);
  pinMode( DC_MOTOR_PIN, OUTPUT );
  pinMode( LED_PIN, OUTPUT);
}

void loop() {

makeyTouch = digitalRead(makeyPin);   // read the input pin
digitalWrite( DC_MOTOR_PIN, LOW );

if(makeyTouch == HIGH) {
vibrateWands();
  }
  else {
  digitalWrite( DC_MOTOR_PIN, LOW );
  digitalWrite( LED_PIN, LOW );
    
  }



}
 void vibrateWands(){ /* Run motor */
  int pwmVal = random(53) + 90; // random of difference betw max & min, + min; 153 = 3V; random(53)+115
  analogWrite( DC_MOTOR_PIN, pwmVal ); //3V
  Serial.println(pwmVal);
  digitalWrite( LED_PIN, HIGH );
  /* Let it run for a while */
  delay( 500 );

}
