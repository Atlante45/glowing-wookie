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
int leftTargetSpeed, rightTargetSpeed;
int leftSpeed, rightSpeed;

void setSpeed(int wheel, int wheel_speed)
{
  int wheel_speed_save = wheel_speed;
  int wheel_direction = HIGH;
  if(wheel_speed < 0)
  {
     wheel_direction = LOW;
     wheel_speed *=-1;
  }
  
  int relative_speed = map(wheel_speed, 0, 100, 0, 255);
  
  if(wheel == LEFT)
  {
    rightSpeed = wheel_speed_save;
    ld = wheel_direction;
    ls = relative_speed;
    digitalWrite(LEFT_DIRECTION, ld);
    analogWrite(LEFT_SPEED, ls);
  }
  else
  {
    rightSpeed = wheel_speed_save;
    rd = wheel_direction;
    rs = relative_speed;
    digitalWrite(RIGHT_DIRECTION, rd);
    analogWrite(RIGHT_SPEED, rs);
  }
}

void updateSpeed(int delta) {
  if (leftSpeed != leftTargetSpeed){
       if (leftSpeed < leftTargetSpeed - delta) {
           leftSpeed += delta;
       } else if (leftSpeed > leftTargetSpeed + delta) {
           leftSpeed -= delta;
       }else {
           leftSpeed = leftTargetSpeed;
       }
       setSpeed(LEFT, leftSpeed);
  }
 
  if (rightSpeed != rightTargetSpeed){
       if (rightSpeed < rightTargetSpeed - delta) {
         rightSpeed += delta;
       } else if (rightSpeed > rightTargetSpeed + delta) {
           rightSpeed -= delta;
       } else {
           rightSpeed = rightTargetSpeed;
       }
       setSpeed(RIGHT, rightSpeed);
  }
}

void init_wheels()
{
    pinMode(LEFT_SPEED, OUTPUT);
    pinMode(LEFT_DIRECTION, OUTPUT);
    pinMode(RIGHT_SPEED, OUTPUT);
    pinMode(RIGHT_DIRECTION, OUTPUT);  
}

void parse(char * buffer)
{
  int right_speed, left_speed, ret;
  ret = sscanf(buffer, "set %d %d", &right_speed, &left_speed);
  if(ret == 2)
  {
    leftTargetSpeed = left_speed;
    rightTargetSpeed = right_speed;
    Serial.println("Speed set");
  }
  else
  {
    if(strcmp(buffer, "get") == 0)
     
      Serial.print("Left = ");
      if (ld==1) Serial.print("+");
      else Serial.print("-");
      Serial.println(ls);
      
      Serial.println("Speed get");
      Serial.print("Right = ");
      if (rd==1) Serial.print("+");
      else Serial.print("-");
      Serial.println(rs);
      
      Serial.print("Target left = ");
      Serial.println(leftTargetSpeed);
      Serial.print("Target right = ");
      Serial.println(rightTargetSpeed);
  }
  
  
}

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(2400);
   Serial.setTimeout(20000);
  
   init_wheels();
   setSpeed(LEFT, 0);
   setSpeed(RIGHT, 0);

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
   /*
   Serial.println("left/right");
   Serial.println(leftTargetSpeed);
   Serial.println("/");
   Serial.println(rightTargetSpeed);
   
   Serial.println("left/right");
   Serial.println(leftSpeed);
   Serial.println("/");
   Serial.println(rightSpeed);
   //*/
   
   
   updateSpeed(5);
   delay(16);
}
