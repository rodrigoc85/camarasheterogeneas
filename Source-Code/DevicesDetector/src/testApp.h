#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxXmlSettings.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        testApp();
        ~testApp();
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        ofxOpenNI * openNIRecorder;
        int numDevices;
        int currDevices;
        ofVideoGrabber vidGrabber;

        bool deviceInited;
        bool readyToSave;
        ofShortPixels  spix;
        ofImage         img;
        int w, h;
        int step;
        bool keyP;
        int numRGBDevices;
        int currRGBDevice;
        ofTrueTypeFont myfont;

};
