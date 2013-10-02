#ifndef _ROBOT_H
#define _ROBOT_H

#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>

#include "serialib.h"

#define SERIAL_PORT     "/dev/ttyUSB0"
#define SERIAL_BAUDRATE 2400

#define RES_WIDTH 160
#define RES_HEIGHT 120

#define RED 0
#define GREEN 1

enum order{
	NO_ORDER,
	STAY,
	FRONT,
	FRONT_LEFT,
	FRONT_RIGHT,
	BACK,
	BACK_LEFT,
	BACK_RIGHT
};

class Robot
{
    public:
        Robot(int streaming);
	~Robot();
        void sendOrder(float leftMotor, float rightMotor);
	void start();

    protected:
	IplImage* GetThresholdedImage(IplImage* img, int color);
	void getObjectPosition(IplImage* frame, int color, int* x, int* y);
	void analyze_frame(IplImage* frame);

	serialib port;
	CvCapture* capture;
	enum order current_order;
	int streaming;
};

#endif
