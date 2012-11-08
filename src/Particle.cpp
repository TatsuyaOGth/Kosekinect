#include "Particle.h"

//コンストラクタ(初期化)
Particle::Particle(){
	setInitialCondition(0,0,0,0);
	damping = 0.0;
    gravity = 0.0;
    pos.set(0, 0);
    frc.set(0, 0);
    vel.set(0, 0);
    target.set(0, 0);
}

//力(加速度)をリセット
void Particle::resetForce(){
    frc.set(0,0);
}

//力を加える
void Particle::addForce(float x, float y){
    frc.x = frc.x + x;
    frc.y = frc.y + y;
}

//抵抗力の計算
void Particle::addDampingForce(){
    frc.x = frc.x - vel.x * damping;
    frc.y = frc.y - vel.y * damping;
}

void Particle::addGravity() {
    if (pos.x > target.x)   vel.x -= gravity;
    else                    vel.x += gravity;
    if (pos.y > target.y)   vel.y -= gravity;
    else                    vel.y += gravity;
}

void Particle::addGroundGravity() {
    vel.y += gravity;
}

//初期状態を設定
void Particle::setInitialCondition(float px, float py, float vx, float vy){
    pos.set(px,py);
	vel.set(vx,vy);
}

void Particle::setInitialGravity(float g, ofPoint t) {
    gravity = g;
    target = t;
}
void Particle::setInitialGravity(float g, int x, int y) {
    gravity = g;
    target.set(x, y);
}

void Particle::setInitialDamping(float d) {
    damping = d;
}

//更新
void Particle::update(){	
	vel = vel + frc;
	pos = pos + vel; 
}
