#pragma once

#include "Particle.h"

class RippleCircle : public Particle {
public:
    
    RippleCircle();
    virtual ~RippleCircle(){};
    
    void rippleExpand(float v);
    void setColor(ofColor col);
    void setInitSize(float r);
    void draw();
    void reduceLife(int t);
    bool isDeath();
    
private:
    
    float radius;
    ofColor color;
    int lifeTime;
    
};