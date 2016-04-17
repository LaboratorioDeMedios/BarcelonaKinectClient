#pragma once

#include "ofMain.h"
#include <map>
#include "ofxOpenNI.h"
#include "SenderoKinectUser.h"


using namespace std;

class KinectSessionManager {
public:
	KinectSessionManager();
	static KinectSessionManager* sharedInstance();
	void start(int maxUsers);
	void stop();
	void update(uint64_t t);
	void drawDebug();
	int getNumberOfUsers();
	ofxOpenNIUser& getUser(int index);
	ofxOpenNIUser& getUserByID(int id);
	SenderoKinectUserSession* getSessionForUser(int index);
private:
	SenderoKinectUsersManager usersManager;
	static KinectSessionManager* instance;
	ofxOpenNI openNIDevice;
    ofTrueTypeFont verdana;
    void userEvent(ofxOpenNIUserEvent & event);
    void gestureEvent(ofxOpenNIGestureEvent & event);
};

