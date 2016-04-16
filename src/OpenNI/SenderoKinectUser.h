#pragma once

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "SceneCalibration.h"
#include "Gestures/Gestures.h"

#define USER_POSITION_JOINT	(JOINT_TORSO)

enum SenderoKinectUserState {
	STANDING,
	RAISED_HAND,
};

class SenderoKinectUser: public ofxOpenNIUser {
private:
	SenderoUserGesture* rightHandGesture;
public:
	SenderoKinectUserState state;

	SenderoKinectUser(): ofxOpenNIUser() {
		state = STANDING;
		rightHandGesture = new RightHandRaiseGesture(this);
	}

	SenderoKinectUser(const SenderoKinectUser& other): ofxOpenNIUser(other) {
		this->state = other.state;
		state = STANDING;
		rightHandGesture = new RightHandRaiseGesture(this);
	}

	~SenderoKinectUser(){
		delete rightHandGesture;
	}

	void updateState(uint64_t t){
		cout << state << endl;
		switch (state){
		case STANDING:
			cout << "updating" << endl;
			rightHandGesture->update(t);
			if (rightHandGesture->wasRecognized()){
				state = RAISED_HAND;
				rightHandGesture->restart();
			}
			break;
		case RAISED_HAND:
			break;
		}
	}

	// void updateState(uint64_t t){
	// 	static ofVec3f lastHand = getRightHandScenePosition();
	// 	static uint64_t lastTime = t;
	// 	static bool isRaising = false;
	// 	static int confidence = 0;
	// 	static int gestureTime = RAISE_HAND_GESTURE_TIME;

	// 	switch (state){
	// 	case STANDING: 
	// 		{
	// 			uint64_t elapsedTime = t - lastTime;

	// 			ofVec3f currentHand = getRightHandScenePosition();

	// 			bool wasRaising = isRaising;

	// 			// cout << currentHand.y - lastHand.y << endl;
	// 			bool _isRaising = currentHand.y - lastHand.y > 0;

	// 			if (isRaising){
	// 				if (!_isRaising)
	// 					confidence++;
					
	// 				if (confidence > RAISE_HAND_CONFIDENCE){
	// 					isRaising = false;
	// 					confidence = 0;
	// 				}
	// 			}else{
	// 				if (_isRaising)
	// 					confidence++;

	// 				if (confidence > RAISE_HAND_CONFIDENCE){
	// 					isRaising = true;
	// 					confidence = 0;
	// 				}
	// 			}

	// 			if (wasRaising && isRaising){
	// 				gestureTime -= elapsedTime;
	// 			} else {
	// 				gestureTime = RAISE_HAND_GESTURE_TIME;
	// 			}

	// 			if (gestureTime <= 0){
	// 				// recognized!
	// 				cout << "isRaising " << isRaising << endl;
	// 				gestureTime = RAISE_HAND_GESTURE_TIME;
	// 				isRaising = false;
	// 				state = RAISED_HAND;
	// 			}

	// 			lastHand = currentHand;
	// 			lastTime = t;
	// 		}
	// 		break;

	// 	case RAISED_HAND:
	// 		break;
	// 	}
	// }

	ofVec3f getScenePosition(){
		if (getNumJoints() <= USER_POSITION_JOINT){
			return ofVec3f(0,0,0);
		}else{
			ofxOpenNIJoint& joint = getJoint(USER_POSITION_JOINT);
			return SceneCalibration::kinectToSceneCoordinates(joint.getProjectivePosition());	
		}
	}

	ofVec3f getRightHandScenePosition(){
		if (getNumJoints() <= JOINT_RIGHT_HAND){
			return ofVec3f(0,0,0);
		}else{
			ofxOpenNIJoint& joint = getJoint(JOINT_RIGHT_HAND);
			return SceneCalibration::kinectToSceneCoordinates(joint.getProjectivePosition());	
		}
	}
};