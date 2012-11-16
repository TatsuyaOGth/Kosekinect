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
    
    balloonImg.loadImage("balloon.png");
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
    mov[0].loadMovie("tsm.mov");
    mov[1].loadMovie("tsm.mov");
    mov[0].play();
    mov[0].setVolume(0);
    
    fadeEvent.setFadeVel(5);
    
    // 操作パラメータ初期値
    nearThreshold = 255;
	farThreshold = 240;
    wallThreshold = 5;
    
    handShineMode = false;
    handZukeiMode = false;
    movieMode = false;
    blackFadeMode = false;
    debugMode = true;
    maxDistance = 1;
    
    erodeNum = 0;
    dirateNum = 1;
    
    ROI_width =   60;
    ROI_bottom =  60;
    ROI_top =     0;
    ROI1.set(ROI_width, ROI_top);
    ROI2.set(kinect.width - ROI_width, kinect.height - ROI_bottom);
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
                myShine.setInitialGravity(0.02);
                shines.push_back(myShine);
            }
        }
        if (handZukeiMode) {
            if (p < 30) {
                RotateParticle myPar;
                float vx = ofRandom(-10, 10);
                float vy = ofRandom(-10, 10);
                myPar.setInitialCondition(pt.x, pt.y, vx, vy);
                myPar.setInitialDamping(0.02);
                myPar.setColor(ofRandom(255),ofRandom(255),ofRandom(255), 200);
                myPar.setRotate(ofRandom(-3, 3));
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
        zukeis[i].update();
        zukeis[i].reduceLife(ofRandom(3));
    }
    for (int i = 0; i < wallShines.size(); i++) {
        wallShines[i].update();
        wallShines[i].reduceLife(ofRandom(2,5));
    }
    for (int i = 0; i < ponBalloons.size(); i++) {
        ponBalloons[i].addGravity();
        ponBalloons[i].update();
    }

    
    // ===== Back Images Update =====
    if (movieMode) {
        mov[0].idleMovie();
        if (mov[0].isFrameNew()) {
            movImg.setFromPixels(mov[0].getPixels(), kinect.width, kinect.height);
        }
    }
    fadeEvent.update();
    
    // ===== Main image Update =====
    int w = mainImg.width;
    int h = mainImg.height;
    
    ofSetColor(255, 255, 255);
    unsigned char *tmpPixels = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * mainImg.width * mainImg.height * 3));
    unsigned char *tmpBinaryPx = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * mainImg.width * mainImg.height));
    unsigned char *rgbPx = kinect.getPixels();
    unsigned char *dp = grayImage.getPixels();
    unsigned char *movPx = movImg.getPixels();
    
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
                if (blackFadeMode) {
                    tmpPixels[i*3+0] = (int)((float)movPx[i*3+0] * fadeEvent.getFadef()); //背景映像（黒フェード）
                    tmpPixels[i*3+1] = (int)((float)movPx[i*3+1] * fadeEvent.getFadef());
                    tmpPixels[i*3+2] = (int)((float)movPx[i*3+2] * fadeEvent.getFadef());
                } else {
                    tmpPixels[i*3+0] = movPx[i*3+0] + (int)(fadeEvent.getFadef() * (255-movPx[i*3+0])); //背景映像（白フェード）
                    tmpPixels[i*3+1] = movPx[i*3+1] + (int)(fadeEvent.getFadef() * (255-movPx[i*3+1]));
                    tmpPixels[i*3+2] = movPx[i*3+2] + (int)(fadeEvent.getFadef() * (255-movPx[i*3+2]));
                }
                tmpBinaryPx[i*3] = 255;
            } else {
                tmpPixels[i*3+0] = rgbPx[i*3+0];
                tmpPixels[i*3+1] = rgbPx[i*3+1];
                tmpPixels[i*3+2] = rgbPx[i*3+2];
                tmpBinaryPx[i*3] = 255;
            }
        }
    }
    mainImg.setFromPixels(tmpPixels, mainImg.width, mainImg.height);
    depthBinaryImage.setFromPixels(tmpBinaryPx, depthBinaryImage.width, depthBinaryImage.height);
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
            shineImg[(int)ofRandom(0,6)].draw(depthContouFinder.blobs[i].pts[j], 10, 10);
        }
    }
    
    // もろもろのパーティクルを描画
    ofPushStyle();
    ofEnableAlphaBlending();
    
    for (int i = 0; i < shines.size(); i++){
        if (shines[i].isDeath() || shines[i].pos.y > ofGetHeight() + 100) {
            shines.erase(shines.begin()+i);
        } else {
            float shineSize = ofRandom(60, 200);
            float shinePosX = shines[i].pos.x - (shineSize / 2);
            float shinePosY = shines[i].pos.y - (shineSize / 2);
            ofSetColor(255, 255, 255, 200);
            shineImg[shines[i].imgId].draw(shinePosX, shinePosY, shineSize, shineSize);
        }
    }
    for (int i = 0; i < zukeis.size(); i++){
        if (zukeis[i].isDeath()) {
            zukeis.erase(zukeis.begin()+i);
        } else {
            float size = ofRandom(60, 200);
            float posx = zukeis[i].pos.x - (size / 2);
            float posy = zukeis[i].pos.y - (size / 2);
            ofSetColor(zukeis[i].getColor());
            zukeiImg[zukeis[i].zukeiId].draw(posx, posy, size, size);
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

    for (int i = 0; i < ponBalloons.size(); i++) {
        float w = 120;
        float h = 200;
        float x = ponBalloons[i].pos.x - (w / 2);
        float y = ponBalloons[i].pos.y - (h / 4);
        if (ponBalloons[i].pos.y < -h) {
            ponBalloons.erase(ponBalloons.begin()+i);
        } else {
            ofSetColor(255, 255, 255);
            balloonImg.draw(x, y, w, h);
        }
    }

    ofDisableAlphaBlending();
    ofPopStyle();
    
    // ===== DEBUG VIEW =====
    if (debugMode) {
        ofPushStyle();
        ofEnableAlphaBlending();
        contourFinder.draw(wRect.x, wRect.y, wRect.width, wRect.height);
        wallContourFinder.draw(wRect.x, wRect.y, wRect.width, wRect.height);
        
        ofSetColor(127, 127, 127, 100);
        ofRect(0, 600, ofGetWidth(), 200);
        
        ofSetColor(0, 0, 255);
        ofNoFill();
        ofRect(ROI1.x, ROI1.y, ROI2.x-ROI1.x, ROI2.y-ROI1.y);
        
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
            
            break;
            
        case '5':
            if (ponBalloons.size() < 500) {
                for (int i=0; i < 50; i++) {
                    Particle myBaloon;
                    float vx = ofRandom(-5, 5);
                    float vy = ofRandom(0, 5);
                    myBaloon.setInitialCondition(ofRandom(0, ofGetWidth()), ofGetHeight()+100, vx, vy);
                    myBaloon.setInitialGravity(0.02, ofRandom(0, ofGetWidth()), -100);
                    ponBalloons.push_back(myBaloon);
                }
            }
            break;
            
        case '6':
            fadeEvent.setOn(!fadeEvent.getOn());
            break;
            
        case '7':
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

