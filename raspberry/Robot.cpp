#include <string>
#include <sstream>
#include "Robot.h"
#include "serialib.h"

using namespace std;

Robot::Robot()
{
    port.Open("/dev/ttyUSB0", 57600);
    char c;
    while (port.Read(&c, 1) <= 0) {
        sleep(1);
    }
}

void Robot::sendOrder(float leftMotor, float rightMotor)
{
    int left = (int)(leftMotor*100);
    int right = (int)(rightMotor*100);

    ostringstream oss;
    oss << "set " << left << " " << right << "\r";

    port.WriteString(oss.str().c_str());
}

