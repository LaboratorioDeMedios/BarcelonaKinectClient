#pragma once



// Headers for OpenNI
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnHash.h>
#include <XnLog.h>

// Header for NITE
#include "XnVNite.h"
// local header
// #include "PointDrawer.h"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include <XnTypes.h>

#include "ofConstants.h"
#include "ofPoint.h"
#include "ofMesh.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofGraphics.h"
#include "ofMatrix3x3.h"
#include "ofQuaternion.h"
#include "of3dUtils.h"
#include "ofLog.h"
#include "ofThread.h"
#include "ofEvents.h"
#include "ofNode.h"
#include "ofCamera.h"

class OpenNI {
	
	static void start();
};
