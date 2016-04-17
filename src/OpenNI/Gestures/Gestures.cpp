#include "Gestures.h"
#include "../SceneCalibration.h"

bool AbstractJointRaiseGesture::isTargetJointAvailable(ofxOpenNIUser& user){
	return user.getNumJoints() > targetJoint();
}

ofVec3f AbstractJointRaiseGesture::targetJointScenePosition(ofxOpenNIUser& user){
	if (!isTargetJointAvailable(user))
		return ofVec3f(0,0,0);
	ofxOpenNIJoint& joint = user.getJoint(targetJoint());
	return SceneCalibration::kinectToSceneCoordinates(joint.getProjectivePosition());
}

void AbstractJointRaiseGesture::update(uint64_t t, ofxOpenNIUser& user){

	static uint64_t lastTime = t;
	uint64_t elapsedTime = t - lastTime;

	ofVec3f currentHand = targetJointScenePosition(user);

	bool wasRaising = isRaising;
	bool _isRaising = currentHand.y - lastHand.y > 0;

	if (isRaising){
		if (!_isRaising)
			confidence++;
		
		if (confidence > RAISE_HAND_CONFIDENCE){
			isRaising = false;
			confidence = 0;
		}
	}else{
		if (_isRaising)
			confidence++;

		if (confidence > RAISE_HAND_CONFIDENCE){
			isRaising = true;
			confidence = 0;
		}
	}

	if (wasRaising && isRaising){
		gestureTime -= elapsedTime;
	} else {
		gestureTime = RAISE_HAND_GESTURE_TIME;
	}

	if (gestureTime <= 0){
		// recognized!
		setGestureRecognized(true);
		gestureTime = RAISE_HAND_GESTURE_TIME;
		isRaising = false;
	}

	lastHand = currentHand;
	lastTime = t;
}

void AbstractJointRaiseGesture::restart(){
	SenderoUserGesture::restart();

	setGestureRecognized(false);
	lastHand = ofVec3f(0,0,0);
	isRaising = false;
	confidence = 0;
	gestureTime = RAISE_HAND_GESTURE_TIME;
}