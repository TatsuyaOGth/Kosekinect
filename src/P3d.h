#pragma once

#include "ofMain.h"

class P3d {
public:
	
    ofVec3f pos; //位置
    ofVec3f vel; //速度
    ofVec3f frc; //力(加速度)
    ofVec3f core; //引力源
    float damping; //摩擦力
    float gravity; //重力
    int imageId;
    int width;
    int height;
    bool coreMode;
    
    P3d();
    virtual ~P3d(){};
    void resetForce();
    void addForce(float x, float y, float z);
    void addDampingForce();
    void setInitialCondition(float px, float py, float pz, float vx, float vy, float vz);
    void setGravity(float g);
    void setDamping(float d);
    void setCorePoint(float x, float y, float z);
    void setCorePoint(ofVec3f pos);
    void update();
//    void draw();
    
protected:
private:
    
    ofColor col;
    bool isCore; //コアが有効かどうか
};