
//
//  InteractionBehaviour.cpp
//  SenderoInteractionClient
//
//  Created by Christian Bouvier on 9/16/15.
//
//

#include "InteractionBehaviour.h"

#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>     /* atof */
#include <vector>

#include <cstdlib>
#include <cmath>
#include <limits>

#define radio(vect) (vect.x)
#define theta(vect) (vect.y)
#define phi(vect)   (vect.z)


double generateGaussianNoise(double mu, double sigma)
{
    const double epsilon = std::numeric_limits<double>::min();
    const double two_pi = 2.0*3.14159265358979323846;

    static double z0, z1;
    static bool generate;
    generate = !generate;

    if (!generate)
       return z1 * sigma + mu;

    double u1, u2;
    do
     {
       u1 = rand() * (1.0 / RAND_MAX);
       u2 = rand() * (1.0 / RAND_MAX);
     }
    while ( u1 <= epsilon );

    z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
    z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
    return z0 * sigma + mu;
}

double gaussian(){
    return generateGaussianNoise(0, 6);
}


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

InteractionBehaviour::InteractionBehaviour(){
    radius = 100;
}

ofVec3f* InteractionBehaviour::intersect(ofVec3f src, ofVec3f direction){   
/*Las posiciones relativas entre un rayo y una esfera son:
 *1. No se intersectan
 *2. El rayo es tangente a la esfera (Interseccion en 1 punto)
 *3. El rayo atraviesa la esfera (Interseccion en 2 puntos)
 *
 * Para averiguar esto, sustituyo en la ecuacion de la esfera la del rayo
 * y mediante la obtencion de las raices determino los puntos de interseccion
 */
    double radio = 70;

    ofVec3f centro(0,0,0);
    direction = direction.getNormalized();
    //Obtengo las partes del rayo para mayor simplicidad de las cuentas
    double x_src = src.x;
    double y_src = src.y;
    double z_src = src.z;
     
    double x_dir = direction.x;
    double y_dir = direction.y;
    double z_dir = direction.z;
   
    //Terminos de la ecuacion cuadratica
    
    // Por vector normal, dX² + dY² + dZ² = 1
    double a = 1; 
    double b = 2*(x_dir*(x_src - centro.x)) + 2*((y_dir*(y_src - centro.y))) + 2*(z_dir*(z_src - centro.z));
    double c = pow(x_src - centro.x,2) + pow(y_src - centro.y,2) + pow(z_src - centro.z,2) - pow(radio,2);    
    
    double discriminante = pow(b,2) - 4*(a*c);
    
    //std::cout << "Norma: " << norma << std::endl;
    //std::cout << "x_src: " << x_src << " y_src: " << y_src << " z_src: " << z_src << std::endl;
    //std::cout << "x_dir: " << x_dir << " y_dir: " << y_dir << " z_dir: " << z_dir << std::endl;
    //std::cout << "a: " << a << " b: " << b << " c: " << c << std::endl;
    //std::cout << "Discriminante: " << discriminante << std::endl;
    double t;
    
    if (discriminante < 0){
    //No hubo interseccion
        return NULL;
    } else {
    //Hubo interseccion, determino si fue una o dos
        double t1 = (-b - sqrt(discriminante))/2;
        //std::cout << "t1: " << t1 << std::endl; 
        double t2 = ((-b + sqrt(discriminante))/2);
        //Caso de interseccion doble, devuelvo la mas cercana al rayo
        
        if (t1 > 0){ //&& (t1 <= Constantes::UMBRAL_DOUBLE) && (objRef == this)){
            t = t1;
        } else if (t2 > 0){
            t = t2;
        }else{
            return NULL;
        } 
    }
    

    ofVec3f* punto = new ofVec3f(0,0,0);

    *punto = src + direction*t;
    return punto;
}


InteractionBehaviour::~InteractionBehaviour(){
}

void InteractionBehaviour::customSetup (map<int,Pixel*>* pixels, vector<Pixel*>* pixelsFast) {

    sphere.setRadius(70);
    sphere.setPosition(0,0,0);

    for(int i = 0; i < MAX_HANDS; i++)
        movingSphere[i].setRadius(30);
    
    movingPointOrigin = ofVec3f(0,0,0);
    movingPoint = ofVec3f(0,0,0);

    moveX = moveY = 0;
    drawMovingPoint = true;
    isSelected = false;


    // openNI
    ofSetLogLevel(OF_LOG_VERBOSE);
    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    // setup the hand generator
    openNIDevice.addHandsGenerator();
    // add all focus gestures (ie., wave, click, raise arm)
    openNIDevice.addAllHandFocusGestures();
    // or you can add them one at a time
    //vector<string> gestureNames = openNIDevice.getAvailableGestures(); // you can use this to get a list of gestures
                                                                         // prints to console and/or you can use the returned vector
    //openNIDevice.addHandFocusGesture("Wave");
    openNIDevice.setMaxNumHands(MAX_HANDS);

    ofAddListener(openNIDevice.handEvent, this, &InteractionBehaviour::handEvent);

    openNIDevice.start();
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
}

void InteractionBehaviour::update(ofCamera* cam) {

    ofColor colors[2] = {ofColor(198,0,147),ofColor(255,255,0)};
    openNIDevice.update();

    static uint64_t last = ofGetElapsedTimeMillis();
    uint64_t now = ofGetElapsedTimeMillis();

    for(int i = 0; i < pixelsFast->size(); i++){
        Pixel* px = (*pixelsFast)[i];
        px->fadeToBlack(0.9);
    }

    vector<ofVec3f> touchPositions = getCurrentSpherePoint(cam);
    
    // if ((touchPosition) /*&& (touchPosition->distance(movingPointOrigin) > 0.0001)*/){
        
    //     cout << "new touchPosition" << endl;
    //     movingPointOrigin = *touchPosition;
    //     movingPoint = movingPointOrigin;
    // }


    // movingPoint = randomizeSpherePoint(movingPoint);
    // if (movingPoint.distance(movingPointOrigin) > 60){
    //     movingPoint = movingPointOrigin;
    // }

    // movingSphere.setPosition(movingPointOrigin);

    for(int i = 0; i < pixelsFast->size(); i++){
        Pixel* px = (*pixelsFast)[i];
        ofVec3f pxPosition = px->getPosition();

        for(int h = 0; h < touchPositions.size(); h++){

            movingSphere[h].setPosition(touchPositions[h]);

            float dist = touchPositions[h].distance(pxPosition);

            if (dist < this->radius){
                float normalizedDist = 1 - dist/this->radius;
                ofColor color = colors[h];
                px->blendRGBA(color.r,color.g,color.b,255,ofLerp(0.1,1,normalizedDist));
                // if (isSelected)
                //     px->blendRGBA(198,0,147,255,ofLerp(0.1,1,normalizedDist));
                // else
                //     px->blendRGBA(115,132,230,255,ofLerp(0.1,1,normalizedDist));
            }
        }
    }
}

void InteractionBehaviour::draw() {

    ofSetColor(255, 255, 255);
    glPushMatrix();
    // draw debug (ie., image, depth, skeleton)
    ofTranslate(0,0,-200);
    openNIDevice.drawDebug();
    glPopMatrix();

    ofSetColor(0,255, 0, 128);

    if (drawMovingPoint){
        ofSetColor(0,0, 255, 200);
        for(int i = 0; i < MAX_HANDS; i++)
            movingSphere[i].draw();
    }
}

void InteractionBehaviour::handEvent(ofxOpenNIHandEvent & event){
    // show hand event messages in the console
    ofLogNotice() << getHandStatusAsString(event.handStatus) << "for hand" << event.id << "from device" << event.deviceID;
}

void InteractionBehaviour::keyPressed(int key){
    //sample -> paint every pixel with red at key pressed and blend with black at update
    vector<Pixel*>::iterator it = this->pixelsFast->begin();
    
    while (it!=this->pixelsFast->end()){
        Pixel* px = *it;
        if (isSelected)
            px->blendRGBA(115,132,230, 255, 1.0f);
        else
            px->blendRGBA(198,0,147, 255, 1.0f);
        it++;
    }
    //end of sample

    drawMovingPoint = false;
    isSelected = !isSelected;
}

void InteractionBehaviour::mouseMoved(int x, int y ){
    mouseX = x;
    mouseY = y;
    moveX = x;
    moveY = y;
}

void InteractionBehaviour::exit() {
    SpecificBehaviour::exit();
    openNIDevice.stop();
}


// http://mathworld.wolfram.com/SphericalCoordinates.html

ofVec3f InteractionBehaviour::toPolar(ofVec3f v){
    ofVec3f result;
    radio(result) = sphereRadius;
    theta(result) = atan(v.y / v.x);
    phi(result) = acos(v.z / sphereRadius);
    return result;
}

ofVec3f InteractionBehaviour::toCartesian(ofVec3f v){
    ofVec3f result;
    result.x = sphereRadius * cos(theta(v)) * sin(phi(v)); 
    result.y = sphereRadius * sin(theta(v)) * sin(phi(v)); 
    result.z = sphereRadius * cos(phi(v));
    return result;
}

ofVec3f InteractionBehaviour::randomizeSpherePoint(ofVec3f p){

    ofVec3f polar = toPolar(p);
    theta(polar) += gaussian();
    phi(polar) += gaussian();

    return toCartesian(polar);
}

// ofVec3f* InteractionBehaviour::getCurrentSpherePoint(ofCamera* cam){
//     ofVec3f screenToWorld = cam->screenToWorld(ofVec3f(moveX,moveY,0.0));
//     ofVec3f src = cam->getPosition();
//     ofVec3f direction = screenToWorld - cam->getPosition();
//     return intersect(src,direction);
// }

vector<ofVec3f> InteractionBehaviour::getCurrentSpherePoint(ofCamera* cam){
    int numHands = openNIDevice.getNumTrackedHands();
    vector<ofVec3f> positions;
    
    // iterate through users
    for (int i = 0; i < numHands; i++){
        
        // get a reference to this user
        ofxOpenNIHand & hand = openNIDevice.getTrackedHand(i);
        
        // get hand position
        ofPoint & handPosition = hand.getPosition();
        // cout << "handPosition " << handPosition << endl;
        // do something with the positions like:
        ofPoint p = handPosition;
        p.z = 80;
        p.y = -p.y;

        // translate
        p.x -= 240;
        p.y += 320;

        positions.push_back(p);
    }
    return positions;
}