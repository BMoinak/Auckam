volatile int state = HIGH;
volatile int flag = HIGH;
int count = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(6, OUTPUT);
  digitalWrite(6, state);

  /* Enable internal pullup. 
   * Without the pin will float and the example will not work */
  pinMode(29, INPUT_PULLUP);
  attachInterrupt(29, blink, FALLING); // Interrupt is fired whenever button is pressed
}

void loop()
{
  digitalWrite(6, state); //LED starts ON
  if(flag) {
    count++;
    Serial.println(count);
    flag = LOW;
  }
}

void blink()
{
  state = !state;
  flag = HIGH;
}
