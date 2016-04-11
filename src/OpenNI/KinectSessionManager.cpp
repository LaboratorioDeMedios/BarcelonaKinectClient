#include "KinectSessionManager.h"

KinectSessionManager::KinectSessionManager(){

}

void KinectSessionManager::start(int maxUsers){
	ofSetLogLevel(OF_LOG_VERBOSE);

	ofAddListener(openNIDevice.userEvent, this, &KinectSessionManager::userEvent);

    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.addUserGenerator();
    openNIDevice.setMaxNumUsers(maxUsers);
    openNIDevice.start();

    
    // set properties for all user masks and point clouds
    //openNIDevice.setUseMaskPixelsAllUsers(true); // if you just want pixels, use this set to true
    openNIDevice.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
    openNIDevice.setUsePointCloudsAllUsers(true);
    openNIDevice.setPointCloudDrawSizeAllUsers(2); // size of each 'point' in the point cloud
    openNIDevice.setPointCloudResolutionAllUsers(2); // resolution of the mesh created for the point cloud eg., this will use every second depth pixel      
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
    

    // you can alternatively create a 'base' user class
	SenderoKinectUser user;
	user.setUseMaskTexture(true);
	user.setUsePointCloud(true);
	user.setPointCloudDrawSize(2);
	user.setPointCloudResolution(2);
	openNIDevice.setBaseUserClass(user);
}

void KinectSessionManager::update(){
	openNIDevice.update();
}

void KinectSessionManager::drawDebug(){
	ofSetColor(255, 255, 255);
    ofPushMatrix();
    openNIDevice.drawDebug();
    ofPopMatrix();
    // draw users
    ofPushMatrix();
    // use a blend mode so we can see 'through' the mask(s)
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    // get number of current users
    int numUsers = openNIDevice.getNumTrackedUsers();
    // iterate through users
    for (int i = 0; i < numUsers; i++){
        // get a reference to this user
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
        // draw the mask texture for this user
        user.drawMask();

        ofMesh& mesh = user.getPointCloud();
        vector<ofVec3f> vertices = mesh.getVertices();
        ofSpherePrimitive s;
        s.setRadius(2);
        for(int v = 0; v < vertices.size(); v++){
        	cout << vertices[v] << endl;
        	ofVec3f scenePoint = SceneCalibration::kinectToSceneCoordinates(vertices[v]);
        	s.setPosition(scenePoint);
        	s.draw();
        }
    }
    ofDisableBlendMode();
    ofPopMatrix();
}

int KinectSessionManager::getNumberOfUsers(){
	return openNIDevice.getNumTrackedUsers();
}

SenderoKinectUser& KinectSessionManager::getUser(int index){
	cout << "before casting!!" << endl;
	return (SenderoKinectUser&) openNIDevice.getTrackedUser(index);
}

void KinectSessionManager::userEvent(ofxOpenNIUserEvent & event){
    // show user event messages in the console
    ofLogNotice() << getUserStatusAsString(event.userStatus) << " for user " << event.id << "from device" << event.deviceID;
}

void KinectSessionManager::stop(){
	openNIDevice.stop();
}