#include "Grain.h"
#include <math.h>

//コンストラクタ(初期化)
Grain::Grain(){
	setInitialCondition(0,0,0,0,0,0);
	damping = 0.01f;
    gravity = 0.9;
    coreMode = false;
    core = ofVec3f(ofGetWidth()/2, ofGetHeight()/2, -70);
    col.set(ofRandom(255), ofRandom(255), ofRandom(255));
}

//力(加速度)をリセット
void Grain::resetForce(){
    frc.set(0,0,0);
}

//力を加える
void Grain::addForce(float x, float y, float z){
    frc.x = frc.x + x;
    frc.y = frc.y + y;
    frc.z = frc.z + z;
}

//抵抗力の計算
void Grain::addDampingForce(){
    frc.x = frc.x - vel.x * damping;
    frc.y = frc.y - vel.y * damping;
    frc.z = frc.z - vel.z * damping;
}

//初期状態を設定
void Grain::setInitialCondition(float px, float py, float pz, float vx, float vy, float vz){
    pos.set(px, py, pz);
	vel.set(vx, vy, vz);
}

//引力源の位置を設定
void Grain::setCorePoint(float x, float y, float z) {
    core.x = x;
    core.y = y;
    core.z = z;
    coreMode = true;
}
void Grain::setCorePoint(ofVec3f pos) {
    core = pos;
    coreMode = true;
}



//更新
void Grain::update(){
	
    //add gravity.
    if (coreMode) {
        if (pos.x > core.x) vel.x -= gravity;
        else                vel.x += gravity;
        if (pos.y > core.y) vel.y -= gravity;
        else                vel.y += gravity;
        if (pos.z > core.z) vel.z -= gravity;
        else                vel.z += gravity;
    }
    
    //衝突判定テスト
//    if (pow((core.x-pos.x),2)+pow((core.y-pos.y),2)+pow((core.z-pos.z),2) <= pow((8.0+100.0),2)) {
//        vel.x *= -1;
//        vel.y *= -1;
//        vel.z *= -1;
//    }
    
        
	vel = vel + frc;
	pos = pos + vel;
    pos = core;
}

//描画
void Grain::draw(){
    
    glEnable(GL_DEPTH_TEST);
    ofPushMatrix();
    ofTranslate(pos.x, pos.y, pos.z);
    ofSetColor(col);
    ofCircle(0, 0, 0, 2);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(core.x, core.y, core.z);
    ofSetHexColor(0x222222);
    ofSetColor(0,0,0);
    //ofSphere(0, 0, 0, 100);
    ofPopMatrix();
    glDisable(GL_DEPTH_TEST);
    
    //ofLine(pos, core);
}



