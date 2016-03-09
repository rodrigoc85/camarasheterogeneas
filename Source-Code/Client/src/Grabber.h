#pragma once

#include "ofMain.h"
#include "Thread2D.h"
#include "Thread3D.h"
#include "ThreadONI.h"
#include "Transmitter.h"
#include "ThreadData.h"
#include "Constants.h"
#include "ofxOpenCv.h"
#include "GlobalData.h"
#include "ofMatrix4x4.h"

#include "ofxOpenNI.h"
#include <pthread.h>

struct DebugTexture {
    ofTexture * videoTexture;
    int         tipo;
    int         id;
};

class Grabber : public IGrabber {
    public:

        bool connected;
		void setup();
		void update();
		void draw();
        void exit();
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void updateThreadData();
		bool isConnected();

		void setupGui();
		std::string getCamTag(int id, int tipo);
		void drawTag(std::string msj, int x, int y);
		ofVec3f * transformPointDepth(ofVec3f point, ofVec3f vtrans);
        ofVec3f * transformPoint(ofVec3f point, ofMatrix4x4 transform);
        Thread2D * t2D;
		Thread3D * t3D;
		ThreadONI * tONI;
		GlobalData * gdata;
        Transmitter  transmitter;
        xn::DepthGenerator * Xn_depth;
        XnDepthPixel*  rawPix;
        ofVec3f v1;
        int downsampling;
        XnPoint3D Point2D, Point3D;
        bool goLive;
        float * tmpX;
        float * tmpY;
        float * tmpZ;
        ofMatrix4x4 matrix;
        int y;
        int x;
        float d;
        ofVec3f * vt;
        bool b_exit_pressed;
        bool b_exit;
        bool b_exit_fired;
        void setVideoPreview(int , int , ofImage );
        void setConnected(bool);
        ofxOpenNI * openNIRecorder;
        ofxOpenNI * openNIPlayer;

        ofTrueTypeFont myfont;

        std::list<DebugTexture *> list;
        std::list<DebugTexture *>::iterator it;

        pthread_mutex_t uiImagesMutex;

};
