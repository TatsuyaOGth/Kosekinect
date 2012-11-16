//
//  FadeEvent.h
//  ofxKinectExample
//
//  Created by Tatsuya Ogusu on 2012/11/11.
//
//

#ifndef __ofxKinectExample__FadeEvent__
#define __ofxKinectExample__FadeEvent__

class FadeEvent {
public:
    
    FadeEvent();
    virtual ~FadeEvent(){};
    
    void setFadeVel(float v);
    void setOn(bool s);
    int getFade();
    float getFadef();
    bool getOn();
    void update();
    
private:
    
    int fade;
    float fadeV;
    bool on;
};

#endif /* defined(__ofxKinectExample__FadeEvent__) */
