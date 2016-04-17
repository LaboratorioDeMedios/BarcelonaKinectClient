#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include <string.h>

class KinectSessionManager;

/* gesture interface */
class SenderoUserGesture {
private:
	bool gestureRecognized;
protected:
	string name;
public:
	SenderoUserGesture(){
		gestureRecognized = false;
		name = "UNKNOWN NAME";
	}

	string getName(){
		return ">> [GESTURE] Sendero Gesture: " + name;
	}
	virtual bool wasRecognized(){
		return gestureRecognized;
	}
	virtual void setGestureRecognized(bool isRecognized){
		if (isRecognized)
			cout << getName() << endl;
		gestureRecognized = isRecognized;
	}
	virtual void restart(){
		gestureRecognized = false;
	}
	virtual void update(uint64_t t, ofxOpenNIUser& user) = 0;
	virtual ~SenderoUserGesture(){}
};


#define RAISE_HAND_CONFIDENCE (2)
#define RAISE_HAND_GESTURE_TIME (500)

class AbstractJointRaiseGesture: public SenderoUserGesture {
private:
	ofVec3f lastHand;
	uint64_t lastTime;
	bool isRaising;
	int confidence;
	int gestureTime;
public:
	AbstractJointRaiseGesture(): SenderoUserGesture() {
		restart();
	}
	ofVec3f targetJointScenePosition(ofxOpenNIUser& user);
	bool isTargetJointAvailable(ofxOpenNIUser& user);
	virtual void update(uint64_t t, ofxOpenNIUser& user);
	virtual void restart();

	virtual Joint targetJoint() = 0;
	virtual ~AbstractJointRaiseGesture(){}
};

class RightHandRaiseGesture: public AbstractJointRaiseGesture {
public:
	RightHandRaiseGesture(): AbstractJointRaiseGesture() {
		name = "RightHandRaiseGesture";
	}
	Joint targetJoint(){
		return JOINT_RIGHT_HAND;
	}
};

class LeftHandRaiseGesture: public AbstractJointRaiseGesture {
public:
	LeftHandRaiseGesture(): AbstractJointRaiseGesture() {
		name = "LeftHandRaiseGesture";
	}
	Joint targetJoint(){
		return JOINT_LEFT_HAND;
	}
};

#include "KinectSessionManager.h"