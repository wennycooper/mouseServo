#include <fcntl.h>
#include <linux/input.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;


#define MOUSEFILE "/dev/input/mouse0\0"
#define SERVOBLASTERFILE "/dev/servoblaster"

int fd;
int fd_sb;
 
char str1[256];
char str2[256];

//

void servoAngle(signed int angleX, signed int angleY)
{
    signed posX = 0;
    signed posY = 0;

    // make -180 < angleX < 180
    if (angleX < -180) angleX = -180;
    if (angleX > 180) angleX = 180;
    if (angleY < -180) angleY = -180;
    if (angleY > 180) angleY = 180;


    posX = 180 - angleX/3;
    posY = 200 + angleY/5;

    //printf("posX should be = %d\n", posX);
    //printf("posY should be = %d\n", posY);

    sprintf(str1, "P1-24=%d\n", posX);
    sprintf(str2, "P1-26=%d\n", posY);

    printf("set %s", str1);
    printf("set %s", str2);

    write(fd_sb, str1, strlen(str1));
    write(fd_sb, str2, strlen(str2));

}


void servoMiddle()
{
    printf("servoMiddle() was called\n");

    sprintf(str1, "P1-24=180\n");
    write(fd_sb, str1, strlen(str1)); 
    sprintf(str2, "P1-26=200\n");
    write(fd_sb, str2, strlen(str2));

}

void servoClose()
{
    servoMiddle();
    printf("servoClose() was called\n");
    //system("echo P1-24=0> /dev/servoblaster");
    sprintf(str1, "P1-24=0\n");
    write(fd_sb, str1, strlen(str1));
    sprintf(str2, "P1-26=0\n");
    write(fd_sb, str2, strlen(str2));
}


int main()
{
    VideoCapture stream1(0);
    Mat frame;

    struct input_event ie;
    char *ptr = (char*)&ie;
    //
    unsigned char button,bLeft,bMiddle,bRight;
    signed char x, y;
    int absolute_x = 0;
    int absolute_y = 0;

    if((fd_sb = open(SERVOBLASTERFILE, O_WRONLY)) == -1)
    {
        printf("Opening %s got ERROR\n", SERVOBLASTERFILE);
        exit(EXIT_FAILURE);

    }

    if((fd = open(MOUSEFILE, O_RDONLY | O_NONBLOCK )) == -1)
    {
        printf("NonBlocking %s open ERROR\n",MOUSEFILE);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("NonBlocking %s open OK\n",MOUSEFILE);
    }

    servoMiddle();


    while(stream1.read(frame))
    {       
        resize(frame, frame, Size(160, 120));
        imshow("mouseServo0", frame);

        if(read(fd, &ie, sizeof(struct input_event))!=-1)
        {
            //
            x=(signed char) ptr[1];
            y=(signed char) ptr[2];
            // computes absolute x,y coordinates
            absolute_x+=x;
            absolute_y-=y;
            // show it!
            printf("X%i Y%i\n",absolute_x,absolute_y);
            servoAngle(absolute_x/2, absolute_y/2);
            fflush(stdout);
        }

        int c = waitKey(10);
        if((char)c == 27) break;
    }


    servoClose();

    close(fd);
    close(fd_sb);
    return 0;
}
