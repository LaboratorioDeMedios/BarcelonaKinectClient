//
//  InteractionBehaviour.h
//  SenderoInteractionClient
//
//  Created by Christian Bouvier on 9/16/15.
//
//

#ifndef __SenderoInteractionClient__InteractionBehaviour__
#define __SenderoInteractionClient__InteractionBehaviour__

#include "OpenNI.h"
#include <iostream>
#include "SpecificBehaviour.h"
#include <vector>

#define MAX_HANDS (2)

using namespace std;

class InteractionBehaviour : public SpecificBehaviour {
public:
    InteractionBehaviour();
    ~InteractionBehaviour();
    virtual void mouseMoved(int x, int y );
    
private:
    void customSetup (map<int,Pixel*>* iPixels, vector<Pixel*>* iPixelsFast);
    void update(ofCamera* cam);
    void draw();
    void keyPressed(int key);
    void exit();
    ofVec3f* intersect(ofVec3f src, ofVec3f direction);
    vector<ofVec3f> getCurrentSpherePoint(ofCamera* cam);
    float radius;

    int mouseX, mouseY;

    ofVec3f movingPointOrigin;
    ofVec3f movingPoint;
    
    int moveX, moveY;
    bool drawMovingPoint;
    bool isSelected;

    ofSpherePrimitive sphere;
    ofSpherePrimitive movingSphere[MAX_HANDS];

    ofVec3f toPolar(ofVec3f xyz);
    ofVec3f toCartesian(ofVec3f rthetaphi);
    ofVec3f randomizeSpherePoint(ofVec3f p);
    const double sphereRadius = 70;
};

#endif /* defined(__SenderoInteractionClient__InteractionBehaviour__) */
