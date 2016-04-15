//
//  InteractionBehaviour.h
//  SenderoInteractionClient
//
//  Created by Christian Bouvier on 9/16/15.
//
//

#ifndef __SenderoInteractionClient__InteractionBehaviour__
#define __SenderoInteractionClient__InteractionBehaviour__

#include "KinectSessionManager.h"
#include "SenderoKinectUser.h"
#include <iostream>
#include "SpecificBehaviour.h"
#include <vector>
#include "CosmicBehaviour.h"
#include "of3dGraphics.h"

#define MAX_USERS (1)

enum InteractionState {
    IDLE,
    USER_PRESENT,
    HAND_INTERACTING
};



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

    CosmicBehaviour interactionListener;


    ofSpherePrimitive movingSphere[MAX_USERS];

    ofVec3f toPolar(ofVec3f xyz);
    ofVec3f toCartesian(ofVec3f rthetaphi);
    ofVec3f randomizeSpherePoint(ofVec3f p);
    float sphereRadius = 70;

    InteractionState interactionState;
    KinectSessionManager kinectSessionManager;
};


#endif /* defined(__SenderoInteractionClient__InteractionBehaviour__) */
