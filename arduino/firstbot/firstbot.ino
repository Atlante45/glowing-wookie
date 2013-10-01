#define RIGHT_SPEED 9
#define RIGHT_DIRECTION 7
#define LEFT_DIRECTION 8
#define LEFT_SPEED 10

#define LEFT 0
#define RIGHT 1

#define SIZE_BUFFER 16
char buffer[SIZE_BUFFER];
int buffer_lenght;

int rs, rd, ls, ld;

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
    ld = wheel_direction;
    ls = relative_speed;
    digitalWrite(LEFT_DIRECTION, wheel_direction);
    analogWrite(LEFT_SPEED,relative_speed);
  }
  else
  {
    rd = wheel_direction;
    rs = relative_speed;
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
    pinMode(RIGHT_DIRECTION, OUTPUT);  
}

void parse(char * buffer)
{
  int right_speed, left_speed, ret;
  ret = sscanf(buffer, "set %d %d", &right_speed, &left_speed);
  if(ret == 2)
  {
    setSpeed(LEFT, left_speed);
    setSpeed(RIGHT, right_speed);
    Serial.println("Speed set");
  }
  else
  {
    if(strcmp(buffer, "get") == 0)
      Serial.println("Speed get");
      Serial.print("Right: direction=");
      Serial.print(rd);
      Serial.print(" speed=");
      Serial.println(rs);
      
      Serial.print("Left: direction=");
      Serial.print(ld);
      Serial.print(" speed=");
      Serial.println(ls);
  }
  
  
}

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
   Serial.setTimeout(20000);
  
   init_wheel();

  Serial.println("Goodnight moon!");
}

void loop() // run over and over
{
   char c; 
 
   if(Serial.available())
   {
      c = Serial.read();
      Serial.print(c);
      if( buffer_lenght < SIZE_BUFFER -1)
      {
        buffer[buffer_lenght++] = c;
        if(c == '\r')
        {
          Serial.println();
          Serial.println("Execute command");
          buffer[buffer_lenght] = '\0';
           parse(buffer);
           buffer_lenght = 0;
        }
      }
      else
      {
          Serial.println();
          Serial.println("Buffer overflow");
         buffer_lenght = 0; 
      }
      
      
   }
   
    
}
