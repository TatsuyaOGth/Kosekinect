#include "Shines.h"

Shines::Shines() {
    imgId = (int)ofRandom(0,6);
    lifeTime = 255;
}

void Shines::reduceLife(int t) {
    lifeTime -= t;
}

bool Shines::isDeath() {
    if (lifeTime <= 0) {
        return true;
    } else {
        return false;
    }
}

void Shines::setColor(ofColor c) {
    col = c;
}