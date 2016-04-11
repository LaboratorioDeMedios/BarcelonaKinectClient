#pragma once

#include "ofMain.h"
#include <cstdlib>
#include <cmath>
#include <limits>


#define radio(vect) vect.x
#define theta(vect) vect.y // 0 <= theta <= PI x-y plane
#define phi(vect)   vect.z // 0 <= phi <= 2PI angle in x-z plane

#define mo phi

#define VELOCITY (0.05)
#define TIME_IN_CENTER (3000)

class CosmicBehaviour {
public:
    ofVec3f polarPosition;
    ofColor color;
    float targetAlpha;
    float lightingRadius;

    CosmicBehaviour(){
    	lightingRadius = 60;
    }

    void update(uint64_t t){
    	static uint64_t last = t;
    	static uint64_t centerTime = TIME_IN_CENTER;

    	uint64_t elapsedT = t - last;

    	if ((mo(polarPosition) <= 0.0000001) && (centerTime > 0)){ 
    		double remain = centerTime - elapsedT;
    		centerTime = remain > 0 ? remain : 0;
    		return;
    	} else {
    		centerTime = TIME_IN_CENTER;
    	}
    	
    	float d = 0;//abs(mo(polarPosition)-PI)/PI;

		mo(polarPosition) += (VELOCITY * elapsedT / 30) * (1.2 - d);
		if (mo(polarPosition) >= 2*PI){
		    mo(polarPosition) -= 2*PI; /* this is fucking relevant */
		}
		cout << mo(polarPosition) << endl;

		// theta(polarPosition) = gaussian();

		lightingRadius += 5.0/30;
		if (lightingRadius >= 100){
			lightingRadius = 30;
		}

		last = t;
    }

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
	    return generateGaussianNoise(0, PI/6)/4;
	}

};