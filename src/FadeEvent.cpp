//
//  FadeEvent.cpp
//  ofxKinectExample
//
//  Created by Tatsuya Ogusu on 2012/11/11.
//
//

#include "FadeEvent.h"

FadeEvent::FadeEvent() {
    fade = 0;
    fadeV = 0;
    on = false;
}

void FadeEvent::setFadeVel(float v) {
    fadeV = v;
}

void FadeEvent::setOn(bool s) {
    on = s;
}

int FadeEvent::getFade() {
    return fade;
}

float FadeEvent::getFadef() {
    return (float)(fade / 255.0);
}

bool FadeEvent::getOn() {
    return on;
}

void FadeEvent::update() {
    if (on && fade < 255) {
        fade += (int)fadeV;
    } else if(!on && fade > 0) {
        fade -= (int)fadeV;
    }
}