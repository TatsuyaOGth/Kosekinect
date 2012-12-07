#pragma once

#include "Particle.h"

class Shines : public Particle {
public:
    
    int imgId;
    ofColor col;
    
    Shines();
    virtual ~Shines(){};
    
    void reduceLife(int t);
    bool isDeath();
    void setColor(ofColor c);
    
private:
    
    int lifeTime;
};