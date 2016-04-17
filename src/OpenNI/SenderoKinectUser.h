#pragma once

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "SceneCalibration.h"
#include "Gestures/Gestures.h"

enum SenderoKinectUserState {
	UNKNOWN,
	STANDING,
	RAISED_LEFT_HAND,
	RAISED_RIGHT_HAND,
	RAISED_BOTH_HANDS
};

#define USER_POSITION_JOINT	(JOINT_TORSO)

class SenderoKinectUserSession {
private:
	SenderoUserGesture* rightHandGesture;
	SenderoUserGesture* leftHandGesture;
	SenderoKinectUserState state;
	int id;
public:

	SenderoKinectUserState getState() {
		return state;
	}

	SenderoKinectUserSession(int id) {
		cout << "NEW" << endl;
		rightHandGesture = new RightHandRaiseGesture();
		leftHandGesture = new LeftHandRaiseGesture();
		state = STANDING;
		this->id = id;
	}

	virtual ~SenderoKinectUserSession(){
		cout << "DELETE" << endl;
		delete rightHandGesture;
		delete leftHandGesture;
	}

	void update(uint64_t t, ofxOpenNIUser& user){
		cout << state << endl;
		switch (state){
			case UNKNOWN:
				state = STANDING;
				/* fall through */
			case STANDING:
				{
					rightHandGesture->update(t, user);
					leftHandGesture->update(t, user);

					int gesturesQty = 0;
					SenderoKinectUserState newState = STANDING;
					if (leftHandGesture->wasRecognized()){
						newState = RAISED_LEFT_HAND;
						gesturesQty++;
					}
					if (rightHandGesture->wasRecognized()){
						newState = RAISED_RIGHT_HAND;
						gesturesQty++;
					}
					if (gesturesQty == 2)
						newState = RAISED_BOTH_HANDS;
					if (newState != STANDING){
						state = newState;
					}
				}
				break;
			case RAISED_LEFT_HAND:
				{
					rightHandGesture->update(t, user);
					if (rightHandGesture->wasRecognized()){
						state = RAISED_BOTH_HANDS;
					}
				}
				break;
			case RAISED_RIGHT_HAND:
				{
					leftHandGesture->update(t, user);
					if (leftHandGesture->wasRecognized()){
						state = RAISED_BOTH_HANDS;
					}
				}
				break;
			case RAISED_BOTH_HANDS:
				break;
			default:
				cout << "NI IDEA MAN" << endl;
				break;
		}
	}

	ofVec3f getScenePosition(ofxOpenNIUser& user){
		if (user.getNumJoints() <= USER_POSITION_JOINT){
			return ofVec3f(0,0,0);
		}else{
			ofxOpenNIJoint& joint = user.getJoint(USER_POSITION_JOINT);
			return SceneCalibration::kinectToSceneCoordinates(joint.getProjectivePosition());	
		}
	}

	ofVec3f getRightHandScenePosition(ofxOpenNIUser& user){
		if (user.getNumJoints() <= JOINT_RIGHT_HAND){
			return ofVec3f(0,0,0);
		}else{
			ofxOpenNIJoint& joint = user.getJoint(JOINT_RIGHT_HAND);
			return SceneCalibration::kinectToSceneCoordinates(joint.getProjectivePosition());
		}	
	}

	ofVec3f getLeftHandScenePosition(ofxOpenNIUser& user){
		if (user.getNumJoints() <= JOINT_LEFT_HAND){
			return ofVec3f(0,0,0);
		}else{
			ofxOpenNIJoint& joint = user.getJoint(JOINT_LEFT_HAND);
			return SceneCalibration::kinectToSceneCoordinates(joint.getProjectivePosition());
		}	
	}

	vector<ofVec3f> getActiveJointsScenePositions(ofxOpenNIUser& user){
		vector<ofVec3f> result;
		switch (getState()){
			case RAISED_RIGHT_HAND:
				result.push_back(getRightHandScenePosition(user));
				break;
			case RAISED_LEFT_HAND:
				result.push_back(getLeftHandScenePosition(user));
				break;
			case RAISED_BOTH_HANDS:
				result.push_back(getLeftHandScenePosition(user));
				result.push_back(getRightHandScenePosition(user));
				break;
			default:
				break;
		}
		return result;
	}

	bool raisedAtLeastOneHand(){
		switch(getState()){
			case RAISED_LEFT_HAND:
			case RAISED_RIGHT_HAND:
			case RAISED_BOTH_HANDS:
				return true;
			default:
				return false;
		}
	}
};

class SenderoKinectUsersManager {
private:
	map<int, SenderoKinectUserSession*> mapper;

	bool idIsInVector(int id, const vector<int>& v){
		for(int i = 0; i < v.size(); i++){
			if (id == v[i])
				return true;
		}
		return false;
	}
public:
	void updateCurrentTrackedStates(const vector<int>& currentIds){
		// insert new ids
		for(int i = 0; i < currentIds.size(); i++){
			int id = currentIds[i];
			if (mapper.find(id) == mapper.end()){
				mapper[id] = new SenderoKinectUserSession(id);
			}
		}

		// remove not current ids
		map<int, SenderoKinectUserSession*>::iterator it;
		for (it = mapper.begin(); it != mapper.end(); ++it){
			int id = it->first;
			if (!idIsInVector(id, currentIds)){
				delete it->second;
				mapper.erase(it);
			}
		}
	}
	SenderoKinectUserSession* getSessionForUser(int id){
		if (mapper.find(id) != mapper.end())
			return mapper[id];
		return NULL;
	}
};
