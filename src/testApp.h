#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

#include "Particle.h"
#include "Shines.h"
#include "RippleCircle.h"

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
		
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	ofxKinect kinect;
	
    ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage depthImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvColorImage movImg;
	ofxCvColorImage mainImg;
	ofxCvContourFinder contourFinder;
    
    bool bDrawPointCloud;
	int nearThreshold;
	int farThreshold;
	int angle;
    
    ofPoint ROI1;
    ofPoint ROI2;
    float ROI_width;
    float ROI_bottom;
    float ROI_top;
    
    vector <Particle> handBalloons;
    vector <Shines> shines;
    vector <RippleCircle> handCircles;
    vector <Particle> ponBalloons;
    
    ofImage balloonImg;
    ofImage shineImg[6];
    ofImage backImg;
    ofVideoPlayer mov[2];
    
    bool handShineMode;
    bool handBalloonMode;
    bool handCircleMode;
    bool movieMode;
    bool debugMode;

    int maxDistance;
    
    int erodeNum;
    int dirateNum;
};
