#include "ThreadONI.h"

void ThreadONI::threadedFunction() {

    openNIRecorder = new ofxOpenNI(context->deviceInstance);
    openNIRecorder->setup();
	openNIRecorder->start();

    openNIRecorder->startPlayer(context->file);
    openNIRecorder->setPaused(false);
    openNIRecorder->setLooped(true);
    openNIRecorder->setMirror(true);
    pthread_mutex_init(&uiMutex, NULL);
    dataAllocated = false;

	if((context->use2D == 1) /*&& (sys_data->goLive == 1)*/) {
        img.allocate(context->resolutionX, context->resolutionX, OF_IMAGE_COLOR);
	}

    deviceInited    = true;

    started = true;

    unsigned long long minMillis = 1000/(sys_data->fps);
    unsigned long long currMill, baseMill;

    while(!b_exit/*isThreadRunning()*/) {
        baseMill = ofGetElapsedTimeMillis();

        process();
        currMill = ofGetElapsedTimeMillis();
        if((currMill - baseMill) < minMillis) {
            ofSleepMillis(minMillis - (currMill - baseMill));
        }

    }
    //ofAddListener(ofEvents().update, this, &ThreadONI::process);

}
void ThreadONI::process(ofEventArgs &e) {
}

void ThreadONI::process() {
    if(!started) return;
    if(!idle) {
        ofLogVerbose() << "[ThreadONI::process] :: NO IDLE / FPS ";
        return;
    }
    idle = false;

    ofLogVerbose() << "[ThreadONI::process] :: IDLE / FPS ";
    //lock();
        //if(!openNIRecorder->isContextReady()) return;
        //openNIRecorder->nextFrame();
        openNIRecorder->update();
        updateData();
    //unlock();
    idle = true;
}

void ThreadONI::updateData() {

    dataAllocated = false;

    if(deviceInited) {
        pthread_mutex_lock(&uiMutex);
        if(context->use2D) {
            if((context->use2D == 1) && (openNIRecorder->isNewFrame())) {
                ofPixels&    ipixels    = openNIRecorder->getImagePixels();

                img.setFromPixels(ipixels.getPixels(), context->resolutionX, context->resolutionY, OF_IMAGE_COLOR, true);
                img.mirror(false , true);
                if(context->resolutionDownSample < 1) {
                    img.resize(img.width * context->resolutionDownSample, img.height * context->resolutionDownSample);
                }
            }
        }

        if(context->use3D && (openNIRecorder->isNewFrame())) {
            spix    = openNIRecorder->getDepthRawPixels();
        }
        dataAllocated = true;
        pthread_mutex_unlock(&uiMutex);
    }
}

bool ThreadONI::isDeviceInitted() {
    return deviceInited;
}

bool ThreadONI::isDataAllocated() {
    return dataAllocated;
}

void ThreadONI::exit() {
    b_exit = true;
    ofLogVerbose() << "[ThreadONI::exit]";
    pthread_mutex_destroy(&uiMutex);
    stopThread();
}
