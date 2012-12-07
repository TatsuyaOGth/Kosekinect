#include "testApp.h"
#include <stdlib.h> 

//--------------------------------------------------------------
void testApp::setup() {
    ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0, 0, 0);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    wRect.setFromCenter(ofGetWidth()/2, ofGetHeight()/2, 960, 720);
    //wRect.set(0, 0, ofGetWidth(), ofGetHeight());
    
	ofSetLogLevel(OF_LOG_VERBOSE);
	kinect.setRegistration(true);
	kinect.init();
	kinect.open();
    
	grayImage.allocate(kinect.width, kinect.height);
    depthBinaryImage.allocate(kinect.width, kinect.height);
	depthImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
    wallImage.allocate(kinect.width, kinect.height);
	wallThreshNear.allocate(kinect.width, kinect.height);
	wallThreshFar.allocate(kinect.width, kinect.height);
    movImg.allocate(kinect.width, kinect.height);
    mainImg.allocate(kinect.width, kinect.height);
	
	angle = 0;
	kinect.setCameraTiltAngle(angle);
    
    balloonImg[0].loadImage("balloon1.png");
    balloonImg[1].loadImage("balloon2.png");
    flowerImg[0].loadImage("flower1.png");
    flowerImg[1].loadImage("flower2.png");
    shineImg[0].loadImage("Shine1.png");
    shineImg[1].loadImage("Shine2.png");
    shineImg[2].loadImage("Shine3.png");
    shineImg[3].loadImage("Shine4.png");
    shineImg[4].loadImage("Shine5.png");
    shineImg[5].loadImage("Shine6.png");
    zukeiImg[0].loadImage("hoshi.png");
    zukeiImg[1].loadImage("shikaku.png");
    zukeiImg[2].loadImage("maru.png");
    zukeiImg[3].loadImage("takaku.png");
    mov[0].loadMovie("backmovie2.mov");
    mov[1].loadMovie("backmovie2.mov");
    mov[0].play();
    mov[1].play();
    
    fadeEvent[0].setFadeVel(5);
    fadeEvent[1].setFadeVel(5);
    
    shineCol[0].set(200, 64, 200, 200);
    shineCol[1].set(120, 0, 120, 100);
    shineCol[2].set(160, 40, 160, 200);
    shineCol[3].set(120, 0, 120, 200);
    
    // 操作パラメータ初期値
    nearThreshold = 255;
	farThreshold = 197;
    wallThreshold = 5;
    
    handShineMode = false;
    handZukeiMode = false;
    movieMode = false;
    blackFadeMode = false;
    debugMode = true;
    ROIDebug = false;
    maxDistance = 1;
    
    erodeNum = 0;
    dirateNum = 3;
    
    ROI_width =   150;
    ROI_bottom =  60;
    ROI_top =     0;
    ROI1.set(ROI_width, ROI_top);
    ROI2.set(kinect.width - ROI_width, kinect.height - ROI_bottom);
    
    fadeEvent[0].setOn(true);
    fadeEvent[1].setOn(false);
}

//--------------------------------------------------------------
void testApp::update() {
	
    // ===== Kinect Update =====
	kinect.update();
    if(kinect.isFrameNew()) {
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        wallImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        // 膨張縮小
        for(int i=0; i<erodeNum; i++){
            grayImage.erode_3x3();
        }
        for(int i=0; i<dirateNum; i++){
            grayImage.dilate_3x3();
        }
        
        // 深度指定範囲検出
        unsigned char *pix = depthImage.getPixels();
        for (int y=0; y < depthImage.height; y++) {
            for (int x=0; x < depthImage.width; x++) {
                int i = x + y * depthImage.width;
                if (x > ROI1.x && x < ROI2.x && y > ROI1.y && y < ROI2.y) {
                    if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                        pix[i] = 255;
                    } else {
                        pix[i] = 0;
                    }
                } else {
                    pix[i] = 0;
                }
            }
        }
        depthImage.flagImageChanged();
		contourFinder.findContours(depthImage, 10, (kinect.width*kinect.height)/2, 20, false);
        
        
        unsigned char *px = wallImage.getPixels();
        for (int y=0; y < wallImage.height; y++) {
            for (int x=0; x < wallImage.width; x++) {
                int i = x + y * wallImage.width;
                if (x > ROI1.x && x < ROI2.x && y > ROI1.y && y < ROI2.y) {
                    if(px[i] < wallThreshold+maxDistance && px[i] > maxDistance) {
                        px[i] = 255;
                    } else {
                        px[i] = 0;
                    }
                } else {
                    px[i] = 0;
                }
            }
        }
        wallImage.flagImageChanged();
        wallContourFinder.findContours(wallImage, 10, (kinect.width*kinect.height)/2, 20, false);
        
    }

        
    // ===== Particles Create and Update =====
    
    // Create Hand Particles
    for (int i=0; i < contourFinder.nBlobs; i++) {
        ofRectangle rect = contourFinder.blobs[i].boundingRect;
        rect.x = rect.x / contourFinder.getWidth() * ofGetWidth();
        rect.y = rect.y / contourFinder.getHeight() * ofGetHeight();
        rect.width = rect.width / contourFinder.getWidth() * ofGetWidth();
        rect.height = rect.height / contourFinder.getHeight() * ofGetHeight();
        ofPoint pt;
        pt.set(ofRandom(rect.x, rect.x+rect.width), ofRandom(rect.y, rect.y+rect.height));
        
        int p = ofRandom(99);
        
        if (handShineMode) {
            if (p < 30 && shines.size() < 500) {
                Shines myShine;
                float vx = ofRandom(-2, 2);
                myShine.setInitialCondition(pt.x, pt.y, vx, 0);
                myShine.setInitialGravity(0.04);
                myShine.setColor(shineCol[(int)ofRandom(0,4)]);
                shines.push_back(myShine);
            }
        }
        if (handZukeiMode) {
            if (p < 30) {
                RotateParticle myPar;
                float vx = ofRandom(-10, 10);
                float vy = ofRandom(-10, 10);
                float vz = ofRandom(10, 15);
                myPar.setInitialCondition(pt.x, pt.y, 0, vx, vy, vz);
                myPar.setDamping(0.03);
                myPar.setColor(ofRandom(255),ofRandom(255),ofRandom(255), 255);
                myPar.setRotate(ofRandom(-6, 6));
                zukeis.push_back(myPar);
            }
        }
    }
    
    // wall particles
    for (int i=0; i < wallContourFinder.nBlobs; i++) {
        ofRectangle rect = wallContourFinder.blobs[i].boundingRect;
        rect.x = rect.x / wallContourFinder.getWidth() * ofGetWidth();
        rect.y = rect.y / wallContourFinder.getHeight() * ofGetHeight();
        rect.width = rect.width / wallContourFinder.getWidth() * ofGetWidth();
        rect.height = rect.height / wallContourFinder.getHeight() * ofGetHeight();
        ofPoint pt;
        pt.set(ofRandom(rect.x, rect.x+rect.width), ofRandom(rect.y, rect.y+rect.height));
        //int p = ofRandom(99);
        if (movieMode) {
            if (/*p < 90 &&*/ wallShines.size() < 1000) {
                Shines myShine;
                float vx = ofRandom(-2, 2);
                float vy = ofRandom(-2, 2);
                myShine.setInitialCondition(pt.x, pt.y, vx, vy);
                myShine.setInitialGravity(0.02);
                wallShines.push_back(myShine);
            }
        }
    }

    
    // Update Particles
    for (int i = 0; i < shines.size(); i++) {
        shines[i].addGroundGravity();
        shines[i].update();
        shines[i].reduceLife(ofRandom(2,5));
    }
    for (int i = 0; i < zukeis.size(); i++) {
        zukeis[i].resetForce();
        zukeis[i].addDampingForce();
        zukeis[i].update();
        zukeis[i].reduceLife(ofRandom(2,5));
    }
    for (int i = 0; i < wallShines.size(); i++) {
        wallShines[i].update();
        wallShines[i].reduceLife(ofRandom(2,5));
    }
    for (int i = 0; i < balloons.size(); i++) {
        balloons[i].update();
    }
    for (int i = 0; i < flowers.size(); i++) {
        flowers[i].update();
    }

    
    // ===== Back Images Update =====
    if (movieMode) {
        mov[0].idleMovie();
        if (mov[0].isFrameNew()) {
            movImg.setFromPixels(mov[0].getPixels(), kinect.width, kinect.height);
        }
    }
    fadeEvent[0].update();
    fadeEvent[1].update();

    
    // ===== Main image Update =====
    int w = mainImg.width;
    int h = mainImg.height;
    
    ofSetColor(255, 255, 255);
    unsigned char *tmpPixels = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * mainImg.width * mainImg.height * 3));
    unsigned char *tmpBinaryPx = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * mainImg.width * mainImg.height));
    unsigned char *rgbPx = kinect.getPixels();
    unsigned char *dp = grayImage.getPixels();
    unsigned char *movPx = movImg.getPixels();
    
    /*
    int numDots = mainImg.width * mainImg.height;
    for(int i=0; i < numDots; i++) {
        if (dp[i] >= maxDistance) {
            int d = abs(dp[i] - maxDistance);
            if (movieMode) {
                if (d < wallThreshold) {
                    tmpPixels[i*3+0] = 255; //境界線は白く
                    tmpPixels[i*3+1] = 255;
                    tmpPixels[i*3+2] = 255;
                } else {
                    tmpPixels[i*3+0] = rgbPx[i*3+0]; //前景はKinectのRGB画像
                    tmpPixels[i*3+1] = rgbPx[i*3+1];
                    tmpPixels[i*3+2] = rgbPx[i*3+2];
                }
                //tmpBinaryPx[i] = 255;
            } else {
                tmpPixels[i*3+0] = rgbPx[i*3+0]; //ムービーモードでない場合、前景はKinectのRGB画像
                tmpPixels[i*3+1] = rgbPx[i*3+1];
                tmpPixels[i*3+2] = rgbPx[i*3+2];
                //tmpBinaryPx[i] = 0;
            }
        } else {
            if (movieMode) {
                if (blackFadeMode) {
                    tmpPixels[i*3+0] = (int)((float)movPx[i*3+0] * fadeEvent.getFadef()); //背景映像（黒フェード）
                    tmpPixels[i*3+1] = (int)((float)movPx[i*3+1] * fadeEvent.getFadef());
                    tmpPixels[i*3+2] = (int)((float)movPx[i*3+2] * fadeEvent.getFadef());
                } else {
                    tmpPixels[i*3+0] = movPx[i*3+0] + (int)(fadeEvent.getFadef() * (255-movPx[i*3+0])); //背景映像（白フェード）
                    tmpPixels[i*3+1] = movPx[i*3+1] + (int)(fadeEvent.getFadef() * (255-movPx[i*3+1]));
                    tmpPixels[i*3+2] = movPx[i*3+2] + (int)(fadeEvent.getFadef() * (255-movPx[i*3+2]));
                }
            } else {
                tmpPixels[i*3+0] = rgbPx[i*3+0];
                tmpPixels[i*3+1] = rgbPx[i*3+1];
                tmpPixels[i*3+2] = rgbPx[i*3+2];
            }
            //tmpBinaryPx[i] = 0;
        }
    }
     */
    
    for (int y=0; y < depthImage.height; y++) {
        for (int x=0; x < depthImage.width; x++) {
            int i = x + y * depthImage.width;
            if (x > ROI1.x && x < ROI2.x && y > ROI1.y && y < ROI2.y) {
                if (dp[i] >= maxDistance) {
                    int d = abs(dp[i] - maxDistance);
                    if (movieMode) {
                        if (d < wallThreshold) {
                            tmpPixels[i*3+0] = 255; //境界線は白く
                            tmpPixels[i*3+1] = 255;
                            tmpPixels[i*3+2] = 255;
                        } else {
                            tmpPixels[i*3+0] = rgbPx[i*3+0]; //前景はKinectのRGB画像
                            tmpPixels[i*3+1] = rgbPx[i*3+1];
                            tmpPixels[i*3+2] = rgbPx[i*3+2];
                        }
                        //tmpBinaryPx[i] = 255;
                    } else {
                        tmpPixels[i*3+0] = rgbPx[i*3+0]; //ムービーモードでない場合、前景はKinectのRGB画像
                        tmpPixels[i*3+1] = rgbPx[i*3+1];
                        tmpPixels[i*3+2] = rgbPx[i*3+2];
                        //tmpBinaryPx[i] = 0;
                    }
                } else {
                    if (movieMode) {
                        if (blackFadeMode) {
                            tmpPixels[i*3+0] = (int)((float)movPx[i*3+0] * fadeEvent[0].getFadef()); //背景映像（黒フェード）
                            tmpPixels[i*3+1] = (int)((float)movPx[i*3+1] * fadeEvent[0].getFadef());
                            tmpPixels[i*3+2] = (int)((float)movPx[i*3+2] * fadeEvent[0].getFadef());
                        } else {
                            tmpPixels[i*3+0] = movPx[i*3+0] + (int)(fadeEvent[1].getFadef() * (255-movPx[i*3+0])); //背景映像（白フェード）
                            tmpPixels[i*3+1] = movPx[i*3+1] + (int)(fadeEvent[1].getFadef() * (255-movPx[i*3+1]));
                            tmpPixels[i*3+2] = movPx[i*3+2] + (int)(fadeEvent[1].getFadef() * (255-movPx[i*3+2]));
                        }
                    } else {
                        tmpPixels[i*3+0] = rgbPx[i*3+0];
                        tmpPixels[i*3+1] = rgbPx[i*3+1];
                        tmpPixels[i*3+2] = rgbPx[i*3+2];
                    }
                    //tmpBinaryPx[i] = 0;
                }
            } else {
                if (movieMode) {
                    if (blackFadeMode) {
                        tmpPixels[i*3+0] = (int)((float)movPx[i*3+0] * fadeEvent[0].getFadef()); //背景映像（黒フェード）
                        tmpPixels[i*3+1] = (int)((float)movPx[i*3+1] * fadeEvent[0].getFadef());
                        tmpPixels[i*3+2] = (int)((float)movPx[i*3+2] * fadeEvent[0].getFadef());
                    } else {
                        tmpPixels[i*3+0] = movPx[i*3+0] + (int)(fadeEvent[1].getFadef() * (255-movPx[i*3+0])); //背景映像（白フェード）
                        tmpPixels[i*3+1] = movPx[i*3+1] + (int)(fadeEvent[1].getFadef() * (255-movPx[i*3+1]));
                        tmpPixels[i*3+2] = movPx[i*3+2] + (int)(fadeEvent[1].getFadef() * (255-movPx[i*3+2]));
                    }
                } else {
                    tmpPixels[i*3+0] = rgbPx[i*3+0];
                    tmpPixels[i*3+1] = rgbPx[i*3+1];
                    tmpPixels[i*3+2] = rgbPx[i*3+2];
                }
            }
        }
    }
    mainImg.setFromPixels(tmpPixels, mainImg.width, mainImg.height);
    //depthBinaryImage.setFromPixels(tmpBinaryPx, depthBinaryImage.width, depthBinaryImage.height);
    free(tmpPixels);
    free(tmpBinaryPx);
    
    // デプスマップの輪郭線を抽出
    depthContouFinder.findContours(depthBinaryImage, 10, (kinect.width*kinect.height)/2, 20, false);
}

//--------------------------------------------------------------
void testApp::draw() {
    
    // メインイメージの描画
    mainImg.draw(wRect.x, wRect.y, wRect.width, wRect.height);
    
    
    // 前景の輪郭に光を描画
    for (int i=0; i < depthContouFinder.nBlobs; i++) {
        int rndn = (int)ofRandom(5,10);
        for(int j=0; j < depthContouFinder.blobs[i].nPts; j+=rndn) {
            int x = (depthContouFinder.blobs[i].pts[j].x / kinect.width) * wRect.width + wRect.x/2;
            int y = (depthContouFinder.blobs[i].pts[j].y / kinect.height) * wRect.height - wRect.y/2;
            shineImg[(int)ofRandom(0,6)].draw(x, y, 100, 100);
        }
    }
    
    // もろもろのパーティクルを描画
    ofPushStyle();
    ofEnableAlphaBlending();
    
    for (int i = 0; i < shines.size(); i++){
        if (shines[i].isDeath() || shines[i].pos.y > ofGetHeight() + 100) {
            shines.erase(shines.begin()+i);
        } else {
            float shineSize = ofRandom(60, 150);
            float shinePosX = shines[i].pos.x - (shineSize / 2);
            float shinePosY = shines[i].pos.y - (shineSize / 2);
            ofSetColor(shines[i].col);
            shineImg[shines[i].imgId].draw(shinePosX, shinePosY, shineSize, shineSize);
        }
    }
    for (int i = 0; i < zukeis.size(); i++){
        if (zukeis[i].isDeath()) {
            zukeis.erase(zukeis.begin()+i);
        } else {
            float size = 80;
            float posx = zukeis[i].pos.x - (size / 2);
            float posy = zukeis[i].pos.y - (size / 2);
            ofPushMatrix();
            ofTranslate(zukeis[i].pos.x, zukeis[i].pos.y, zukeis[i].pos.z);
            ofRotateZ(zukeis[i].getRotate());
            ofSetColor(zukeis[i].getColor());
            zukeiImg[zukeis[i].zukeiId].draw(-(size/2), -(size/2), size, size);
            ofPopMatrix();
        }
    }
    
    for (int i = 0; i < wallShines.size(); i++){
        if (wallShines[i].isDeath()) {
            wallShines.erase(wallShines.begin()+i);
        } else {
            float shineSize = ofRandom(20, 100);
            float shinePosX = wallShines[i].pos.x - (shineSize / 2);
            float shinePosY = wallShines[i].pos.y - (shineSize / 2);
            ofSetColor(255, 255, 255, 200);
            shineImg[wallShines[i].imgId].draw(shinePosX, shinePosY, shineSize, shineSize);
        }
    }
    
    glEnable(GL_DEPTH_TEST);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glAlphaFunc(GL_GEQUAL, 0.5);
	glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    for (int i = 0; i < balloons.size(); i++) {
        float x = balloons[i].pos.x - (balloons[i].width / 2);
        float y = balloons[i].pos.y - (balloons[i].height / 2);
        if (balloons[i].pos.y < -balloons[i].height) {
            balloons.erase(balloons.begin()+i);
        } else {
            ofPushMatrix();
            ofTranslate(balloons[i].pos.x, balloons[i].pos.y, balloons[i].pos.z);
            ofSetColor(255, 255, 255, 255);
            balloonImg[balloons[i].imageId].draw(-(balloons[i].width/2), -(balloons[i].height/2), balloons[i].width, balloons[i].height);
            ofPopMatrix();
        }
    }
    
    for (int i = 0; i < flowers.size(); i++) {
        float x = flowers[i].pos.x - (flowers[i].width / 2);
        float y = flowers[i].pos.y - (flowers[i].height / 2);
        if (flowers[i].pos.y > ofGetHeight()+400) {
            flowers.erase(flowers.begin()+i);
        } else {
            ofPushMatrix();
            ofTranslate(flowers[i].pos.x, flowers[i].pos.y, flowers[i].pos.z);
            ofRotateZ(flowers[i].getRotate());
            ofSetColor(255, 255, 255, 255);
            flowerImg[flowers[i].imageId].draw(-(flowers[i].width/2), -(flowers[i].height/2), flowers[i].width, flowers[i].height);
            ofPopMatrix();
        }
    }
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);

    ofDisableAlphaBlending();
    ofPopStyle();
    
    // ===== DEBUG VIEW =====
    if (debugMode) {
        ofPushStyle();
        ofEnableAlphaBlending();
        contourFinder.draw(wRect.x, wRect.y, wRect.width, wRect.height);
//        depthContouFinder.draw(wRect.x, wRect.y, wRect.width, wRect.height);
        wallContourFinder.draw(wRect.x, wRect.y, wRect.width, wRect.height);
        
        ofSetColor(127, 127, 127, 100);
        ofRect(0, 600, ofGetWidth(), 200);
        
        
        
        // パラメータ
        ofSetColor(255, 255, 255);
        stringstream reportStream;
        reportStream << "set far threshold " << farThreshold << " (press: LEFT RIGHT) num blobs found " << contourFinder.nBlobs
        << ", fps: " << ofGetFrameRate() << endl
        << "kinect max distance (press: + / -): " << maxDistance << endl
        << "[z][x] set erode:" << erodeNum << "  "
        << "[a][s] set dirate:" << dirateNum << endl
        << "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl
        << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl;
        ofDrawBitmapString(reportStream.str(),20,622);
        ofDisableAlphaBlending();
        
        if (ROIDebug) {
            ofSetColor(255, 255, 255);
            kinect.draw(0, 0, kinect.width, kinect.height);
            ofSetColor(0, 0, 255);
            ofNoFill();
            ofRect(ROI1.x, ROI1.y, ROI2.x-ROI1.x, ROI2.y-ROI1.y);
        }
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
        case '1':
            handZukeiMode = !handZukeiMode;
            break;
            
        case '2':
            handShineMode = !handShineMode;
            break;
            
        case '3':
            movieMode = !movieMode;
            break;
            
        case '4':
            blackFadeMode = false;
            fadeEvent[1].setOn(!fadeEvent[1].getOn());
            break;
            
        case '5':
            blackFadeMode = true;
            fadeEvent[0].setOn(!fadeEvent[0].getOn());
            break;
            
        case '6':
            if (balloons.size() < 100) {
                for (int i=0; i < 50; i++) {
                    P3d myBaloon;
                    float vx = ofRandom(-5, 5);
                    float vy = ofRandom(-10, 0);
                    myBaloon.setInitialCondition(ofRandom(0, ofGetWidth()), ofGetHeight()+500, ofRandom(-600, 600), vx, vy, 0);
                    myBaloon.setCorePoint(ofRandom(0, ofGetWidth()), -360, myBaloon.pos.z);
                    myBaloon.setGravity(0.02);
                    myBaloon.imageId = i % 2;
                    myBaloon.width = 640;
                    myBaloon.height = 360;
                    balloons.push_back(myBaloon);
                }
            }
            break;
            
        case '7':
            if (flowers.size() < 100) {
                for (int i=0; i < 15; i++) {
                    RotateParticle myParticle;
                    float vx = ofRandom(-1, 1);
                    float vy = ofRandom(8, 10);
                    myParticle.setInitialCondition(ofRandom(0, ofGetWidth()), -480 - ofRandom(1200), ofRandom(-400, 200), vx, vy, 0);
                    myParticle.setCorePoint(ofRandom(0, ofGetWidth()), -360, myParticle.pos.z);
                    myParticle.setRotate(ofRandom(-2,2));
                    myParticle.imageId = i % 2;
                    myParticle.width = 640;
                    myParticle.height = 480;
                    flowers.push_back(myParticle);
                }
            }

            break;
            
        case '8':
            break;
            
        case '9':
            break;
            
        case '0':
            break;
            

		case OF_KEY_RIGHT:
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case OF_KEY_LEFT:
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
        
        case '-':
            maxDistance--;
            if (maxDistance < 0) maxDistance = 0;
            break;
            
        case '+':
        case ';':
            maxDistance++;
            if (maxDistance > 255) maxDistance = 255;
            break;
            
        case 'z':
            if (erodeNum!=0) {
                erodeNum--;
            }
            break;
            
        case 'x':
            if (erodeNum!=255) {
                erodeNum++;
            }
            break;
            
        case 'a':
            if (dirateNum!=0) {
                dirateNum--;
            }
            break;
            
        case 's':
            if (dirateNum!=255) {
                dirateNum++;
            }
            break;

            
        case ' ':
            debugMode = !debugMode;
            break;
            
        case 'r':
            ROIDebug = !ROIDebug;
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
/*    int X = ofMap((float)x, 0, ofGetWidth(), 0, kinect.width);
    int Y = ofMap((float)y, 0, ofGetWidth(), 0, kinect.height);
    ROI_width = kinect.width - X;
    ROI_bottom = kinect.height - Y;
    ROI1.set(ROI_width, ROI_top);
    ROI2.set(kinect.width - ROI_width, kinect.height - ROI_bottom);
 */
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
}

