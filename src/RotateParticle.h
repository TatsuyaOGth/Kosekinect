#pragma once

#include "P3d.h"

class RotateParticle : public P3d{
public:
    
    int zukeiId;
    void reduceLife(float t);
    bool isDeath();
    int fadev;
    
    RotateParticle();
    virtual ~RotateParticle(){};
    
    void setRotate(float v);
    void setColor(int r, int g, int b, int a);
    float getRotate();
    ofColor getColor();
    
    void setLifeTime(int lofetime);
    void setFadeSpeed(float sp);
    void fadeUpdate();
    
private:
    
    ofColor col;
    float rotv;
    float rot;
    int lifeTime;
    
    int fade;
    float fadeSpeed;

};