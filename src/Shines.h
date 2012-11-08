#pragma once

#include "Particle.h"

class Shines : public Particle {
public:
    
    int imgId;
    
    Shines();
    virtual ~Shines(){};
    
    void reduceLife(int t);
    bool isDeath();

    
private:
    
    int lifeTime;
};