#pragma once

#include "ofxOpenNI.h"
#include "GlobalData.h"
#include "ofMain.h"
#include <pthread.h>

class ThreadONI : public ofThread {

	public:
        /*ThreadONI();
        ~ThreadONI();*/

		void	threadedFunction();
		void    updateData();
		void process(ofEventArgs &e);
		void process();
		bool	isDeviceInitted();
        bool	isDataAllocated();
        void exit();

        bool    deviceInited;
		//ofxOpenNI player;
		//ofxOpenNI * openNIRecorder;
        ofxOpenNI * openNIRecorder;
        bool    dataAllocated;
        ofImage         img;
        ofFloatPixels  fXpix;
        ofFloatPixels  fYpix;
        ofFloatPixels  fZpix;
        ofShortPixels  spix;
        t_camera  * context;
        t_data * sys_data;
        int _id;

        bool started;
        bool idle;
        bool b_exit;
        pthread_mutex_t uiMutex;

        ThreadONI() {
            //openNIRecorder  = NULL;
            deviceInited    = false;
            idle = true;
            started = false;
            b_exit  = false;
        }

        ~ThreadONI() {
            b_exit = true;
            if(!started) return;

           // ofRemoveListener(ofEvents().update, this, &ThreadONI::process);

            //openNIRecorder->stop();
            // done

            if((context->use2D == 1) && (sys_data->goLive == 1)) {
                img.clear();
            }

            /*if(openNIRecorder != NULL) {
                delete openNIRecorder;
            }*/
        }
};
