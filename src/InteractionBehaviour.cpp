
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
    radius = 20;
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
        
        if (t1 > 0){
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

    // sphere.setRadius(70);
    // sphere.setPosition(0,0,0);
    sphereRadius = 70;

    interactionListener.polarPosition = ofVec3f(sphereRadius,0,0);
    interactionListener.color = ofColor(255,0,255,0);
    interactionListener.targetAlpha = 30;

    for(int i = 0; i < MAX_USERS; i++)
        movingSphere[i].setRadius(15);
    
    movingPointOrigin = ofVec3f(0,0,0);
    movingPoint = ofVec3f(0,0,0);

    moveX = moveY = 0;
    drawMovingPoint = true;
    isSelected = false;

    kinectSessionManager.start(MAX_USERS);

    interactionState = IDLE;
}

void InteractionBehaviour::update(ofCamera* cam) {

    uint64_t t = ofGetElapsedTimeMillis();

    kinectSessionManager.update();

    int diff = abs(interactionListener.targetAlpha - interactionListener.color.a);
    if (interactionListener.targetAlpha > interactionListener.color.a)
        interactionListener.color.a += min(10,diff);
    if (interactionListener.targetAlpha < interactionListener.color.a)
        interactionListener.color.a -= min(10,diff);

    // cout << "alpha " << (int)interactionListener.color.a << endl;
    switch (interactionState){
    case IDLE:
        if (kinectSessionManager.getNumberOfUsers() > 0){
            interactionState = USER_PRESENT;
            interactionListener.targetAlpha = 255;
            cout << "USER_PRESENT" << endl;
        }
        break;
    case USER_PRESENT:
        if (kinectSessionManager.getNumberOfUsers() == 0){
            interactionState = IDLE;
            interactionListener.targetAlpha = 30;
            cout << "IDLE" << endl;
        }
        break;
    case HAND_INTERACTING:
        if (kinectSessionManager.getNumberOfUsers() == 0){
            interactionState = IDLE;
            interactionListener.targetAlpha = 30;
            cout << "IDLE" << endl;
        }
        break;
    }

    interactionListener.update(t);

    // cout << "interactionListener.position " << interactionListener.position << endl;
    // cout << "toCartesian(interactionListener.position) " << toCartesian(interactionListener.position) << endl;


    ofColor colors[2] = {ofColor(255,0,255),ofColor(255,255,0)};

    static uint64_t last = ofGetElapsedTimeMillis();
    uint64_t now = ofGetElapsedTimeMillis();

    for(int i = 0; i < pixelsFast->size(); i++){
        Pixel* px = (*pixelsFast)[i];
        px->fadeToBlack(0.99);
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

        // draw distance to listener point
        float dist = toCartesian(interactionListener.polarPosition).distance(pxPosition);
        if (dist < interactionListener.lightingRadius){
            float normalizedDist = 1 - dist/interactionListener.lightingRadius;
            ofColor color = interactionListener.color;
            px->blendRGBA(color.r,color.g,color.b,1,ofLerp(0.1,1,normalizedDist)*(color.a/255.f));
        }

        // draw hand interactions

        if (interactionState == HAND_INTERACTING){
            for(int h = 0; h < touchPositions.size(); h++){
                movingSphere[h].setPosition(touchPositions[h]);
                float dist = touchPositions[h].distance(pxPosition);
                if (dist < this->radius){
                    float normalizedDist = 1 - dist/this->radius;
                    ofColor color = colors[h];
                    px->blendRGBA(color.r,color.g,color.b,1,ofLerp(0.1,1,normalizedDist));
                }
            }
        }
    }
}

void InteractionBehaviour::draw() {

    ofSetColor(255, 255, 255);

    for(int i = 0; i < kinectSessionManager.getNumberOfUsers(); i++){
        SenderoKinectUser& user = kinectSessionManager.getUser(i);
        ofVec3f rightHand = user.getRightHandScenePosition();
        ofVec3f ray = HAND_RAY_PROJECTION_ORIGIN - rightHand;

        ofDrawArrow(rightHand, HAND_RAY_PROJECTION_ORIGIN);

        ofVec3f* inter = intersect(rightHand,ray);
        if (inter){
            ofSpherePrimitive sphere;
            sphere.setRadius(10);
            sphere.setPosition(*inter);
            sphere.draw();
            delete inter;
        }
    }

    ofSetColor(255, 255, 255);
    glPushMatrix();
    // draw debug (ie., image, depth, skeleton)
    // ofTranslate(0,0,-200);
    kinectSessionManager.drawDebug();
    glPopMatrix();

    // for(int i = 0; i < MAX_USERS; i++){
    //     movingSphere[i].draw();
    // }

    // ofSetColor(interactionListener.color);
    // interactionListener.sphere.setPosition(toCartesian(interactionListener.position));
    // interactionListener.sphere.draw();
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
    kinectSessionManager.stop();
}


// http://mathworld.wolfram.com/SphericalCoordinates.html

ofVec3f InteractionBehaviour::toPolar(ofVec3f v){
    ofVec3f result;
    radio(result) = sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));
    theta(result) = (abs(v.x) > 0.00000000001) ? atan(v.y / v.x) : 0;
    phi(result) = acos(v.z / radio(result));
    return result;
}

ofVec3f InteractionBehaviour::toCartesian(ofVec3f v){
    ofVec3f result;
    result.x = radio(v) * sin(phi(v)) * cos(theta(v)); 
    result.y = radio(v) * sin(phi(v)) * sin(theta(v)); 
    result.z = radio(v) * cos(phi(v));
    return result;
}

ofVec3f InteractionBehaviour::randomizeSpherePoint(ofVec3f p){

    ofVec3f polar = toPolar(p);
    // theta(polar) += gaussian();
    // phi(polar) += gaussian();

    return toCartesian(polar);
}

vector<ofVec3f> InteractionBehaviour::getCurrentSpherePoint(ofCamera* cam){

    vector<ofVec3f> result;

    for(int i = 0; i < kinectSessionManager.getNumberOfUsers(); i++){
        SenderoKinectUser& user = kinectSessionManager.getUser(i);
        cout << "10" << endl;
        cout << user.isFound() << endl;
        cout << "10.5" << endl;
        user.updateState(ofGetElapsedTimeMillis());
        cout << "11" << endl;
        if (user.state == RAISED_HAND){

            interactionListener.targetAlpha = 10;
            interactionState = HAND_INTERACTING;

            ofVec3f rightHand = user.getRightHandScenePosition();
            cout << "12" << endl;
            ofVec3f ray = HAND_RAY_PROJECTION_ORIGIN - rightHand;
            ofVec3f* inter = intersect(rightHand,ray);
            cout << "13" << endl;
            if (inter){
                result.push_back(*inter);
                delete inter;
            }
        }
    }
    return result;
}