#ifndef PIN_ACTION_H_
#define PIN_ACTION_H_

/*Function to control the different pins */
void digital_write(int pinID, int value);
void pwm_write(int pinID, int value);

int digital_read(int pinID);
int analog_read(int pinID);



#endif
