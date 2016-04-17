#include "KinectSessionManager.h"

KinectSessionManager* KinectSessionManager::instance = NULL;

KinectSessionManager* KinectSessionManager::sharedInstance(){
    if (instance == NULL)
        instance = new KinectSessionManager();
    return instance;
}

KinectSessionManager::KinectSessionManager(){
    instance = this;
}

void KinectSessionManager::start(int maxUsers){
	ofSetLogLevel(OF_LOG_VERBOSE);

	ofAddListener(openNIDevice.userEvent, this, &KinectSessionManager::userEvent);
    ofAddListener(openNIDevice.gestureEvent, this, &KinectSessionManager::gestureEvent);

    openNIDevice.setup(false);
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.addUserGenerator();
    openNIDevice.addImageGenerator();
    // openNIDevice.addGestureGenerator();
    // openNIDevice.addAllGestures();
    openNIDevice.setMaxNumUsers(maxUsers);
    openNIDevice.setUserSmoothing(0.1);
    
    // set properties for all user masks and point clouds
    //openNIDevice.setUseMaskPixelsAllUsers(true); // if you just want pixels, use this set to true
    openNIDevice.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
    openNIDevice.setUsePointCloudsAllUsers(true);
    openNIDevice.setPointCloudDrawSizeAllUsers(2); // size of each 'point' in the point cloud
    // openNIDevice.setPointCloudResolutionAllUsers(50); // resolution of the mesh created for the point cloud eg., this will use every second depth pixel      
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
    

    // you can alternatively create a 'base' user class
	ofxOpenNIUser user;
	// user.setUseMaskTexture(true);
	user.setUsePointCloud(true);
	user.setPointCloudDrawSize(2);
	user.setPointCloudResolution(20);
    // user.setUseAutoCalibration(false);
    // user.setForceResetTimeout(300);
    // user.setConfidenceThreshold(0.1);
	openNIDevice.setBaseUserClass(user);

    openNIDevice.start();
}

void KinectSessionManager::update(uint64_t t){
	openNIDevice.update();
    vector<int> currentIds;
    for(int i = 0; i < getNumberOfUsers(); i++){
        ofxOpenNIUser& user = getUser(i);
        currentIds.push_back(user.getXnID());
    }
    usersManager.updateCurrentTrackedStates(currentIds);

    for(int i = 0; i < getNumberOfUsers(); i++){
        ofxOpenNIUser& user = getUser(i);
        SenderoKinectUserSession* session = usersManager.getSessionForUser(user.getXnID());
        session->update(t, user);
    }

}

void KinectSessionManager::drawDebug(){
	ofSetColor(255, 255, 255);
    // draw users
    ofPushMatrix();
    // use a blend mode so we can see 'through' the mask(s)
    // ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    // get number of current users
    int numUsers = openNIDevice.getNumTrackedUsers();
    // iterate through users
    for (int i = 0; i < numUsers; i++){
        // get a reference to this user
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
        // draw the mask texture for this user
        // user.drawMask();

        ofMesh& mesh = user.getPointCloud();
        vector<ofVec3f> vertices = mesh.getVertices();
        ofSpherePrimitive s;
        s.setRadius(2);
        ofSetColor(255, 0, 0);
        for(int v = 0; v < vertices.size(); v++){
        	ofVec3f scenePoint = SceneCalibration::kinectToSceneCoordinates(vertices[v]);
        	s.setPosition(scenePoint);
        	s.draw();
        }
    }
    // ofDisableBlendMode();
    ofPopMatrix();
}

int KinectSessionManager::getNumberOfUsers(){
	return openNIDevice.getNumTrackedUsers();
}

SenderoKinectUserSession* KinectSessionManager::getSessionForUser(int index){
    ofxOpenNIUser& user = openNIDevice.getTrackedUser(index);
    return usersManager.getSessionForUser(user.getXnID());
}

ofxOpenNIUser& KinectSessionManager::getUser(int index){
	return openNIDevice.getTrackedUser(index);
}

ofxOpenNIUser& KinectSessionManager::getUserByID(int id){
    bool found = false;
    for(int i = 0; i < getNumberOfUsers(); i++){
        ofxOpenNIUser& user = getUser(i);
        if (user.getXnID() == id){
            found = true;
            return user;
        }
    }
    assert(found);
}

void KinectSessionManager::userEvent(ofxOpenNIUserEvent & event){
    // show user event messages in the console
    ofLogNotice() << getUserStatusAsString(event.userStatus) << " for user " << event.id << "from device" << event.deviceID;
}

void KinectSessionManager::gestureEvent(ofxOpenNIGestureEvent & event){
    // show user event messages in the console
    ofLogNotice() << getGestureStatusAsString(event.gestureStatus) << " for " << event.gestureName << endl;
}

void KinectSessionManager::stop(){
	openNIDevice.stop();
}