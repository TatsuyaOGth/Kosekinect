#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

#include "Particle.h"
#include "Shines.h"
#include "RotateParticle.h"
#include "RippleCircle.h"
#include "FadeEvent.h"
#include "P3d.h"

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
	
    ofxCvGrayscaleImage grayImage; //深度画像
	ofxCvGrayscaleImage depthImage; //手検出結果（２値画像）
    ofxCvGrayscaleImage wallImage; //壁検出結果（２値画像）
	ofxCvGrayscaleImage grayThreshNear; //手検出用１
	ofxCvGrayscaleImage grayThreshFar; //手検出用２
    ofxCvContourFinder contourFinder; //手用輪郭画像
    ofxCvContourFinder wallContourFinder; //壁用輪郭画像
    ofxCvGrayscaleImage depthBinaryImage; //深度画像の２値画像
    ofxCvContourFinder depthContouFinder; //深度画像の輪郭画像
    ofxCvGrayscaleImage wallThreshNear; //壁検出用１
	ofxCvGrayscaleImage wallThreshFar; //壁手検出用２
    ofxCvColorImage movImg; //映像
	ofxCvColorImage mainImg; //メイン画像
    ofxCvColorImage tempImg;
    
    FadeEvent fadeEvent[2];
    
    bool bDrawPointCloud;
	int nearThreshold;
	int farThreshold;
    int wallThreshold;
	int angle;
    
    ofRectangle wRect;
    
    ofPoint ROI1;
    ofPoint ROI2;
    float ROI_width;
    float ROI_bottom;
    float ROI_top;
    
    vector <Shines> shines;
    vector <RotateParticle> zukeis;
    vector <Shines> wallShines;
    vector <P3d> balloons;
    vector <RotateParticle> flowers;
    
    ofImage balloonImg[2];
    ofImage flowerImg[2];
    ofImage shineImg[6];
    ofImage zukeiImg[5];
    ofVideoPlayer mov[2];
    
    ofColor shineCol[4];
    
    bool handShineMode;
    bool handZukeiMode;
    bool movieMode;
    bool blackFadeMode;
    bool debugMode;
    bool ROIDebug;

    int maxDistance;
    
    int erodeNum;
    int dirateNum;
};
