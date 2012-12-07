#include "RotateParticle.h"

RotateParticle::RotateParticle() {
    zukeiId = (int)ofRandom(0,4);
    rotv = 0;
    rot = 0;
    lifeTime = 255;
    fadev = 0;
    fade = 0;
    fadeSpeed = 0;
}

void RotateParticle::setRotate(float v) {
    rotv = v;
}

void RotateParticle::setColor(int r, int g, int b, int a) {
    col.set(r,g,b,a);
}

ofColor RotateParticle::getColor() {
    return col;
}

float RotateParticle::getRotate() {
    rot += rotv;
    return rot;
}

void RotateParticle::reduceLife(float t) {
    lifeTime -= (int)t;
}

bool RotateParticle::isDeath() {
    if (lifeTime <= 0) {
        return true;
    } else {
        return false;
    }
}



void RotateParticle::setLifeTime(int lt) {
    lifeTime = lt;
}

void RotateParticle::setFadeSpeed(float sp) {
    fadeSpeed = sp;
}

void RotateParticle::fadeUpdate() {
    
}
