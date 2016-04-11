#pragma once

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "SceneCalibration.h"

#define USER_POSITION_JOINT	(JOINT_TORSO)

class SenderoKinectUser: public ofxOpenNIUser {
public:
	ofVec3f getScenePosition(){
		if (getNumJoints() <= USER_POSITION_JOINT){
			return ofVec3f(0,0,0);
		}else{
			ofxOpenNIJoint& joint = getJoint(USER_POSITION_JOINT);
			return SceneCalibration::kinectToSceneCoordinates(joint.getWorldPosition());	
		}
	}
};