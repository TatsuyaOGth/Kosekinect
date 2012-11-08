#include "RippleCircle.h"

RippleCircle::RippleCircle() {
    color.set(255, 255, 255);
    lifeTime = 255;
}

void RippleCircle::setColor(ofColor c) {
    color.set(c);
}

void RippleCircle::setInitSize(float r) {
    radius = r;
}


void RippleCircle::reduceLife(int t) {
    lifeTime -= t;
}

void RippleCircle::rippleExpand(float v) {
    radius += v;
}

void RippleCircle::draw() {
    ofEnableSmoothing();
//    ofEnableAlphaBlending();
    ofSetColor(color.r, color.g, color.b, lifeTime);
    ofCircle(pos.x, pos.y, radius);
//    ofDisableAlphaBlending();
    ofDisableSmoothing();
}

bool RippleCircle::isDeath() {
    if (lifeTime <= 0) {
        return true;
    } else {
        return false;
    }
}