#include <string>
#include <sstream>
#include "Robot.h"
#include "serialib.h"

using namespace std;

Robot::Robot(int streaming)
{
    port.Open(SERIAL_PORT, SERIAL_BAUDRATE);
    char c;
   // while (port.Read(&c, 1) <= 0) {
   //     sleep(1);
   // }

    this->streaming = streaming;
    this->current_order = NO_ORDER;

    //initialize capture from the camera
    this->capture = NULL;
    this->capture = cvCaptureFromCAM(0);

    //if there is no device
    if(!this->capture)
    {
	    printf("Error: Could not initialize capturing... \n");
	    exit(1);
    }

    //Init resolution
    cvSetCaptureProperty( this->capture, CV_CAP_PROP_FRAME_WIDTH, RES_WIDTH);
    cvSetCaptureProperty( this->capture, CV_CAP_PROP_FRAME_HEIGHT, RES_HEIGHT);


}

Robot::~Robot()
{
	//release the capture
	cvReleaseCapture(&this->capture);

}

IplImage* Robot::GetThresholdedImage(IplImage* img, int color)
{
	IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img, imgHSV, CV_BGR2HSV);

	IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

	if(color == RED)
	{
		cvInRangeS(imgHSV, cvScalar(0, 150, 120), cvScalar(4, 255, 255), imgThreshed);
		if(this->streaming)
			cvShowImage("red", imgThreshed);
	}
	/*
	else if(color == GREEN)
	{

		cvInRangeS(imgHSV, cvScalar(0, 150, 120), cvScalar(4, 255, 255), imgThreshed);
		if(this->streaming)
			cvShowImage("green", imgThreshed);
	}
	*/
	cvReleaseImage(&imgHSV);
	return imgThreshed;

}

int Robot::getObjectPosition(IplImage* frame, int color) 
{
	int x;
	IplImage* img = this->GetThresholdedImage(frame, color);

	//Compute the moments to estimate the position
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
	cvMoments(img, moments, 1);

	cvReleaseImage(&img);

	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double area = cvGetCentralMoment(moments, 0, 0);
	x = moment10/area;

	delete moments;

	return x;
}

void Robot::analyze_frame(IplImage* frame)
{
	int x;

	if(this->streaming)
		cvShowImage("video", frame);

	//Looking for Red
	x = this->getObjectPosition(frame, RED);
	if(x>0)
	{
		printf("Position=%d, color=Red\n",x);
		if(x < RES_WIDTH/2 - RES_WIDTH/16)
		{
			if(this->current_order != FRONT_LEFT)
			{
				printf("Front Left\n");
				this->current_order = FRONT_LEFT;
				this->sendOrder(0.2,0.6);
			}
		}
		else if(x > RES_WIDTH/2 + RES_WIDTH/8)
		{
			if(this->current_order != FRONT_RIGHT)
			{
				printf("Front Right\n");
				this->current_order = FRONT_RIGHT;
				this->sendOrder(0.6,0.2);
			}
		}
		else
		{
			if(this->current_order != FRONT)
			{
				printf("Front\n");
				this->current_order = FRONT;
				this->sendOrder(0.4,0.4);
			}
		}
		return;
	}
	
	/*
	//Looking for Green
	x = this->getObjectPosition(frame, GREEN));
	if(x>0)
	{
		printf("Position=%d, color=GREEN\n",x);
		if(x < RES_WIDTH/2 - RES_WIDTH/16)
		{
			if(this->current_order != BACK_LEFT)
			{
				printf("Back Left\n");
				this->current_order = BACK_LEFT;
				this->sendOrder(-0.6,-0.2);
			}
		}
		else if(x > RES_WIDTH/2 + RES_WIDTH/8)
		{
			if(this->current_order != BACK_RIGHT)
			{
				printf("Back Right\n");
				this->current_order = BACK_RIGHT;
				this->sendOrder(-0.2,-0.6);
			}
		}
		else
		{
			if(this->current_order != BACK)
			{
				printf("Back\n");
				this->current_order = BACK;
				this->sendOrder(-0.4,-0.4);
			}
		}
		return;
	}
	*/

	if(this->current_order != STAY)
	{
		printf("Stay\n");
		this->current_order = STAY;
		this->sendOrder(0,0);
	}
}


void Robot::start()
{
	if(this->streaming)
	{
		//Open 3 windows to stream 
		cvNamedWindow("video");
		cvNamedWindow("green");
		cvNamedWindow("red");
	}
	
	while(true)
	{
		//try to grab a frame
		IplImage* frame = NULL;
		frame = cvQueryFrame(this->capture);
		//if we couldn't grab a frame
		if(!frame)
			break;

		this->analyze_frame(frame);

		//Wait for a keypress
		int res = cvWaitKey(50);
		//If a key has been pressed, break out of the loop
		if(res!=-1)
			break;
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

