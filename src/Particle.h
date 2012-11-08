#pragma once

#include "ofMain.h"

class Particle {
public:
	
    ofVec2f pos; //位置
    ofVec2f vel; //速度
    ofVec2f frc; //力(加速度)
    float damping; //摩擦力
    float gravity; //重力
    ofPoint target; //引力源
    
    Particle();
    virtual ~Particle(){};
    void resetForce();
    void addForce(float x, float y);
    void addDampingForce();
    void addGravity();
    void addGroundGravity();
    void setInitialCondition(float px, float py, float vx, float vy);
    void setInitialGravity(float g, ofPoint target);
    void setInitialGravity(float g, int x=0, int y=0);
    void setInitialDamping(float d);
    void update();
    
protected:
private:
    
};