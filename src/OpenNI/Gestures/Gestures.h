#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"

class SenderoKinectUser;

/* gesture interface */
class SenderoUserGesture {
private:
	SenderoKinectUser* user;
	bool gestureRecognized;
public:
	SenderoUserGesture(SenderoKinectUser* user){
		this->user = user;
		gestureRecognized = false;
	}
	virtual bool wasRecognized(){
		return gestureRecognized;
	}
	virtual void setGestureRecognized(bool isRecognized){
		gestureRecognized = isRecognized;
	}
	virtual SenderoKinectUser* getUser(){
		cout << "get user!" << endl;
		return user;
	}
	virtual void restart(){
		gestureRecognized = false;
	}
	virtual void update(uint64_t t) = 0;
};


#define RAISE_HAND_CONFIDENCE (2)
#define RAISE_HAND_GESTURE_TIME (600)

class AbstractJointRaiseGesture: public SenderoUserGesture {
private:
	ofVec3f lastHand;
	uint64_t lastTime;
	bool isRaising;
	int confidence;
	int gestureTime;
public:
	AbstractJointRaiseGesture(SenderoKinectUser* user): SenderoUserGesture(user) {
		restart();
	}
	ofVec3f targetJointScenePosition();
	bool isTargetJointAvailable();
	virtual void update(uint64_t t);
	virtual void restart();

	virtual Joint targetJoint() = 0;
};

class RightHandRaiseGesture: public AbstractJointRaiseGesture {
public:
	RightHandRaiseGesture(SenderoKinectUser* user): AbstractJointRaiseGesture(user) {}
	Joint targetJoint(){
		return JOINT_RIGHT_HAND;
	}
};

class LeftHandRaiseGesture: public AbstractJointRaiseGesture {
public:
	LeftHandRaiseGesture(SenderoKinectUser* user): AbstractJointRaiseGesture(user) {}
	Joint targetJoint(){
		return JOINT_LEFT_HAND;
	}
};

#include "../SenderoKinectUser.h"