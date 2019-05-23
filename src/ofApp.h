/*
Tangible Recognition Implementation by www.convivial.studio
Calendar, Visualization Coding - by Laura Wagner
*/

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "touchObject.h"



class ofApp : public ofBaseApp {


public:

  void setup();
  void update();
  void draw();
  void exit();

  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);


  /* Touches */
  void touchDown(ofTouchEventArgs & touch);
  void touchMoved(ofTouchEventArgs & touch);
  void touchUp(ofTouchEventArgs & touch);
  void touchDoubleTap(ofTouchEventArgs & touch);
  void touchCancelled(ofTouchEventArgs & touch);
  map<int, ofTouchEventArgs> touchMap;

  ofxPanel gui;
  ofxFloatSlider distanceTriangle;
  ofxFloatSlider errorToleranceTriangle;
  ofxToggle drawDebugB;
  ofxToggle fullScreenB;

  touchObject touchObj;

  /* IMAGES*/

  ofImage radialc;



};
