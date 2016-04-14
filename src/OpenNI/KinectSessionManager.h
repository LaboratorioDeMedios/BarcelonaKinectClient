#pragma once

#include "ofMain.h"
#include <map>
#include "ofxOpenNI.h"
#include "SenderoKinectUser.h"
using namespace std;


class KinectSessionManager {
public:
	KinectSessionManager();
	void start(int maxUsers);
	void stop();
	void update();
	void drawDebug();
	int getNumberOfUsers();
	SenderoKinectUser& getUser(int index);
private:
	ofxOpenNI openNIDevice;
    ofTrueTypeFont verdana;
    void userEvent(ofxOpenNIUserEvent & event);
    void gestureEvent(ofxOpenNIGestureEvent & event);
};