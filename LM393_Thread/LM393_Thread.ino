
int sensor = 2;
unsigned long start_time = 0;
unsigned long end_time = 0;
int steps =0;
float steps_old=0;
float temp=0;
float rps=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(sensor, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  start_time=millis();
  end_time= start_time+1000;

  while(millis()<end_time){
    if(digitalRead(sensor)){
      steps=steps+1;
      while(digitalRead(sensor));
    }
  }
  temp=steps-steps_old;
  steps_old=steps;
  rps=(temp/20);
  Serial.println(rps);
}
