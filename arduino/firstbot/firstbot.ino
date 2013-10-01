#define RIGHT_SPEED 9
#define RIGHT_DIRECTION 7
#define LEFT_DIRECTION 8
#define LEFT_SPEED 10

void setSpeed(int wheel, int wheel_speed)
{
  int wheel_direction = HIGH;
  if(wheel_speed < 0)
  {
     wheel_direction = LOW;
     wheel_speed *=-1;
  }
  
  int relative_speed = map(wheel_speed, 0, 100, 0, 255);
  
  if(wheel == LEFT)
  {
    digitalWrite(LEFT_DIRECTION, wheel_direction);
    analogWrite(LEFT_SPEED,relative_speed);
  }
  else
  {
    digitalWrite(RIGHT_DIRECTION, wheel_direction);
    analogWrite(RIGHT_SPEED,relative_speed);
  }
}

void init_wheel()
{
   // Vitesse moteur gauche
    pinMode(LEFT_SPEED, OUTPUT);
    // Direction moteur gauche
    pinMode(LEFT_DIRECTION, OUTPUT);
    
    // Vitesse moteur gauche
    pinMode(RIGHT_SPEED, OUTPUT);
    // Direction moteur gauche
    pinMode(RIGHT_RIRECTION, OUTPUT);  
}

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
    
   init_wheel();
}


  Serial.println("Goodnight moon!");
}

void loop() // run over and over
{
  char c;
  if(Serial.available())
    {
      c = Serial.read();
      Serial.print(c);
    }
}
