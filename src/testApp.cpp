#include "testApp.h"

#define MAX_CORE_NUM 768000

//--------------------------------------------------------------
void testApp::setup() {
    ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0, 0, 0);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
	ofSetLogLevel(OF_LOG_VERBOSE);
	kinect.setRegistration(true);
	kinect.init();
	kinect.open();
    
	grayImage.allocate(kinect.width, kinect.height);
	depthImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
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
    backImg.loadImage("test.jpg");
    mov[0].loadMovie("test.mp4");
    mov[1].loadMovie("movie2.mov");
    mov[0].play();
    mov[0].setVolume(0);
    
    // 操作パラメータ初期値
    nearThreshold = 255;
	farThreshold = 240;
    
    handShineMode = false;
    handBalloonMode = false;
    handCircleMode = false;
    movieMode = false;
    debugMode = true;
    maxDistance = 1;
    
    erodeNum = 0;
    dirateNum = 1;
    
    ROI_width =   0;
    ROI_bottom =  0;
    ROI_top =     0;
    ROI1.set(ROI_width, ROI_top);
    ROI2.set(ofGetWidth() - ROI_width, ofGetHeight() - ROI_bottom);
}

//--------------------------------------------------------------
void testApp::update() {
	
    // ===== Kinect Update =====
    
	kinect.update();
    if(kinect.isFrameNew()) {
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        // 膨張縮小
        for(int i=0; i<erodeNum; i++){
            grayImage.erode_3x3();
        }
        for(int i=0; i<dirateNum; i++){
            grayImage.dilate_3x3();
        }
        
        // 手先検出
        unsigned char *pix = depthImage.getPixels();
        /*int numPixels = depthImage.getWidth() * depthImage.getHeight();
        for(int i = 0; i < numPixels; i++) {
            if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                pix[i] = 255;
            } else {
                pix[i] = 0;
            }
		}*/
        for (int y=0; y < depthImage.getHeight(); y++) {
            for (int x=0; x < depthImage.getWidth(); x++) {
                int i = x + y * depthImage.getWidth();
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
    }
    printf("x1 %f, x2 %f, y1 %f, y2 %f\n", ROI1.x, ROI2.x, ROI1.y, ROI2.y);

    
    
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
        
        if (handBalloonMode) {
            if (p < 10) {
                Particle myHandBaloon;
                float vx = ofRandom(-2, 2);
                myHandBaloon.setInitialCondition(pt.x, pt.y, vx, 0);
                myHandBaloon.setInitialGravity(0.02, ofRandom(0, ofGetWidth()), -100);
                handBalloons.push_back(myHandBaloon);
            }
        }
        
        if (handCircleMode) {
            if (p < 1) {
                ofColor c;
                int rand = ofRandom(10);
                if (rand > 5) c.set(255, 20, 20);
                else          c.set(255, 255, 255);
                RippleCircle myCircle;
                myCircle.setInitialCondition(pt.x, pt.y, 0, 0);
                myCircle.setColor(c);
                myCircle.setInitSize(80);
                handCircles.push_back(myCircle);
            }
        }
    }
    
    // Shines Update.
    for (int i = 0; i < shines.size(); i++) {
        shines[i].addGroundGravity();
        shines[i].update();
        shines[i].reduceLife(ofRandom(2,5));
    }
    // Balloons Update.
    for (int i = 0; i < handBalloons.size(); i++) {
        handBalloons[i].addGravity();
        handBalloons[i].update();
    }
    
    for (int i = 0; i < handCircles.size(); i++) {
        handCircles[i].rippleExpand(1);
        handCircles[i].reduceLife(2);
    }
    
    for (int i = 0; i < ponBalloons.size(); i++) {
        ponBalloons[i].addGravity();
        ponBalloons[i].update();
    }

    // ===== Movie Update =====
    if (movieMode) {
        mov[0].idleMovie();
        if (mov[0].isFrameNew()) {
            movImg.setFromPixels(mov[0].getPixels(), kinect.width, kinect.height);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw() {
    
    // ***** main image draw *****
    ofSetColor(255, 255, 255);
    int w = mainImg.width;
    int h = mainImg.height;
    unsigned char *tmpPixels = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * mainImg.width * mainImg.height*4));
    unsigned char *rgbPx = kinect.getPixels();
    unsigned char *px = grayImage.getPixels();
//    unsigned char *movPx = movImg.getPixels();
    unsigned char *movPx = backImg.getPixels();

    int numDots = mainImg.width * mainImg.height;
    for(int i=0; i < numDots; i++) {
        if (px[i] >= maxDistance) {
            tmpPixels[i*3+0] = rgbPx[i*3+0];
            tmpPixels[i*3+1] = rgbPx[i*3+1];
            tmpPixels[i*3+2] = rgbPx[i*3+2];
        } else {
            if (movieMode) {
                tmpPixels[i*3+0] = movPx[i*3+0];
                tmpPixels[i*3+1] = movPx[i*3+1];
                tmpPixels[i*3+2] = movPx[i*3+2];
            } else {
                tmpPixels[i*3+0] = rgbPx[i*3+0];
                tmpPixels[i*3+1] = rgbPx[i*3+1];
                tmpPixels[i*3+2] = rgbPx[i*3+2];
            }
        }
    }
    mainImg.setFromPixels(tmpPixels, mainImg.width, mainImg.height);
    mainImg.draw(0, 0, ofGetWidth(), ofGetHeight());
    free(tmpPixels);
    
    // ***** draw particles *****
    ofPushStyle();
    ofEnableAlphaBlending();
    for (int i = 0; i < handBalloons.size(); i++) {
        float handBalloonW = 120;
        float handBalloonH = 180;
        float handBalloonX = handBalloons[i].pos.x - (handBalloonW / 2);
        float handBalloonY = handBalloons[i].pos.y - (handBalloonH / 4);
        if (handBalloons[i].pos.y < -handBalloonH) {
            handBalloons.erase(handBalloons.begin()+i);
        } else {
            ofSetColor(255, 255, 255);
            balloonImg.draw(handBalloonX, handBalloonY, handBalloonW, handBalloonH);
        }
    }
    
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
    
    for (int i = 0; i < handCircles.size(); i++){
        if (handCircles[i].isDeath()) {
            handCircles.erase(handCircles.begin()+i);
        } else {
            handCircles[i].draw();
        }
    }

    for (int i = 0; i < ponBalloons.size(); i++) {
        float w = 120;
        float h = 180;
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
        contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight());
        
        ofSetColor(127, 127, 127, 200);
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
            handBalloonMode = !handBalloonMode;
            break;
            
        case '2':
            handShineMode = !handShineMode;
            break;
            
        case '3':
            handCircleMode = !handCircleMode;
            break;
            
        case '4':
            movieMode = !movieMode;
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
    ROI_width = ofGetWidth() - x;
    ROI_bottom = ofGetHeight() - y;
    ROI1.set(ROI_width, ROI_top);
    ROI2.set(ofGetWidth() - ROI_width, ofGetHeight() - ROI_bottom);
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

