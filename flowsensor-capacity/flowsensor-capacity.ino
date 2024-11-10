// To measure Liters of passing water volume

double flow; //Liters of passing water volume
unsigned long pulse_freq;
void pulse () // Interrupt function
{
  pulse_freq++;
}
void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  Serial.begin(9600);
  attachInterrupt(0, pulse, RISING); // Setup Interrupt

}

void loop() {
  // put your main code here, to run repeatedly:
  flow = .00225 * pulse_freq;
  Serial.print(flow, DEC);
  Serial.println("L");
  delay(500);

}
