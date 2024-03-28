#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main(){
    VideoCapture cap(1);
    if(!cap.isOpened()){
        cout << "Cannout start webcam" << endl;
        return -1;
    }
    namedWindow("Control");
    //red 170-179
    // I have set the limits for red color object you can change the values or use tracker to change for desired color
    int lowH = 147;
    int highH = 255;

    int lowS = 101;
    int highS = 160;

    int lowV = 190;
    int highV = 255;

    createTrackbar("LowH", "Control", &lowH, 255);
    createTrackbar("highH", "Control", &highH, 255);

    createTrackbar("LowS", "Control", &lowS, 255);
    createTrackbar("highS", "Control", &highS, 255);

    createTrackbar("LowV", "Control", &lowV, 255);
    createTrackbar("highV", "Control", &highV, 255);

    int lastX = -1;
    int lastY = -1;

    Mat temp;
    cap.read(temp);

    Mat imgLines = Mat::zeros(temp.size(), CV_8UC3);

    while(true){
        Mat orginal;
        bool success = cap.read(orginal);
        if(!success){
            cout << "Cannot read a frame from a video" << endl;
            break;
        }
        Mat imgHSV;
        cvtColor(orginal, imgHSV, COLOR_BGR2HSV);
        
        Mat imgThreshold;

        inRange(imgHSV, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), imgThreshold);

        //morphological opening of image: errosion followed by dilation
        erode(imgThreshold, imgThreshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(imgThreshold, imgThreshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        //morphological clossing of image: dilation followed by errosion
        dilate(imgThreshold, imgThreshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        erode(imgThreshold, imgThreshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        Moments momnt = moments(imgThreshold);

        double dM01 = momnt.m01;
        double dM10 = momnt.m10;
        double dArea = momnt.m00;

        cout << dArea << "\n";
        if(dArea > 9000){
            int posX = dM10 / dArea;
            int posY = dM01 / dArea;
            // if(lastX >= 0 && lastY >= 0 && posX >=0 && posY >= 0){
            //     line(imgLines, Point(posX, posY), Point(lastX, lastY), Scalar(0,0,255), 2);
            // }
            lastX = posX;
            lastY = posY;

            int w = 50;
            int h = 50;
            int thickness = 2;

            Point p1(posX, posY);
            Point p2(posX + w, posY + h);

            rectangle(orginal, p1, p2, Scalar(255, 0, 0), thickness, LINE_8);

			// cout << "X: " << posX << "   ";
			// cout << "Y: " << posY << "\n";
        }


        imshow("Thresholded Image", imgThreshold);
        imshow("Original", orginal);

        if(waitKey(30) == 27){
            break;
        }    
    }
    return 0;
}