/*
This application uses Open Source components. 
You can find the source code of the open source project along with license information below. 
I acknowledge and I'm grateful to the developer for his contribution to open source.

Project: oscTouchObject https://github.com/paul-ferragut/oscTouchObject

*/


#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"


#define MARKER_LOW_RANGE 0
#define MARKER_HIGH_RANGE 180

#define USE_OSC

#ifdef USE_OSC
#include "ofxOsc.h"
#endif



 struct trianglePt {

	ofPoint pos;
	vector<int>groupBelonging;
	bool paired;
};

struct triangleTracker {

	ofPoint pos[3];
	
	int apexPtIndex;

	float orientation;

	float rangeLow;
	float rangeHigh;

	ofPoint center;
	ofPoint apex;

	int index;
	float apexAngle;
	float heightTri;//usually called altitude on isocele triangles
	float widthTri;
	bool visible;

};


class touchObject {
	/* IMAGES*/

	ofImage radialc;
	

public:
touchObject(){};
~touchObject(){};
  void setup();
  void update(vector<ofPoint>touchPt, float distanceTriangle);
  void draw();
 
  vector<ofPoint> getCenter();
  vector<float> getWidth();
  vector<float> getHeight();
  vector<float> getAngle();
  vector<int> getIndex();

  int findTop(ofPoint pt[3]);
  float findOrientation(ofPoint pt[3], int top);
  float findAltitude(ofPoint pt[3], int top);
  float findAngleApex(ofPoint pt[3], int top);
  float findWidth(ofPoint pt[3], int top);

  vector<triangleTracker>triangleT;
  vector<int>degrees;
  ofxXmlSettings settings;
  
#ifdef USE_OSC
  ofxOscSender sender;
  void sendOsc();
#endif

};
