#ifndef THREAD2D_H
#define THREAD2D_H

#include "GlobalData.h"

#include "ofxQTVideoSaver.h"
#include "ofMain.h"
#include <pthread.h>

class Thread2D : public ofThread {

	public:
		void threadedFunction();
		void process(ofEventArgs &e);
		void process();
		bool isDeviceInitted();
		bool isDataAllocated();
		bool b_exit;
        void exit();

        int  device;
        ofImage img;
        int camWidth;
        int camHeight;
        bool isAllocated;
        ofVideoGrabber vidGrabber;
        t_camera  * context;
        t_data * sys_data;
        ofxQtVideoSaver video;
        bool started;
        bool idle;
        float mTimestamp;
        bool first;

        pthread_mutex_t uiMutex;

        Thread2D() {
            idle    = true;
            started = false;
            b_exit  = false;
        }

        ~Thread2D() {
            b_exit = true;
            if(!started) return;
            //ofRemoveListener(ofEvents().update, this, &Thread2D::process);
            if(sys_data->goLive == 1) {
                img.clear();
            }

            if(sys_data->persistence == 1) {
                video.finishMovie();
            }
        }
};

#endif // THREAD2D_H
