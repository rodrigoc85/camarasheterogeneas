#include "Thread2D.h"

void Thread2D::threadedFunction() {
    isAllocated = false;

    first = true;

    pthread_mutex_init(&uiMutex, NULL);
    vidGrabber.setVerbose(true);
    vidGrabber.setDeviceID(context->deviceInstance);
    vidGrabber.setDesiredFrameRate(sys_data->fps);
    vidGrabber.initGrabber(context->resolutionX, context->resolutionY);
    string path = "cameras/2D/" + ofToString(context->id);
    img.allocate(context->resolutionX, context->resolutionX, OF_IMAGE_COLOR);

    if(sys_data->persistence == 1) {
        ofDirectory::createDirectory(path, true, true);
        //nuevo QT
        video.setCodecQualityLevel( OF_QT_SAVER_CODEC_QUALITY_NORMAL );
        video.setup( context->resolutionX, context->resolutionY, "cameras/2D/" + ofToString(context->id) + "/rec.mov" );
        //fin:nuevo QT
    }

    started = true;
    unsigned long long minMillis = 1000/sys_data->fps;
    unsigned long long currMill, baseMill;

    while(!b_exit/*isThreadRunning()*/) {
        baseMill = ofGetElapsedTimeMillis();

        process();
        currMill = ofGetElapsedTimeMillis();
        if((currMill - baseMill) < minMillis) {
            ofSleepMillis(minMillis - (currMill - baseMill));
        }
    }

}
void Thread2D::process(ofEventArgs &e) {
}

void Thread2D::process() {

    ofLogVerbose() << "[Thread2D::process] " << endl;
    if(!started) return;
    if(!idle) {
        ofLogVerbose() << "[Thread2D::process] :: NO IDLE / FPS ";
        return;
    }
    idle = false;

    ofLogVerbose() << "[Thread2D::process] :: IDLE / FPS ";

    vidGrabber.grabFrame();

    if (vidGrabber.isFrameNew()) {
        isAllocated = false;

        pthread_mutex_lock(&uiMutex);
        img.setFromPixels(vidGrabber.getPixels(), context->resolutionX, context->resolutionY, OF_IMAGE_COLOR, true);
        pthread_mutex_unlock(&uiMutex);

        if(sys_data->persistence == 1) {
            //nuevo QT
            float time  = ofGetElapsedTimef() - mTimestamp;
            video.addFrame(vidGrabber.getPixels(), first ? 0 : time);
            mTimestamp  = ofGetElapsedTimef();
            first       = false;
            //fin:nuevo QT
        }

        isAllocated = true;
        //unlock();

    }
    idle = true;
    //return;
}

bool Thread2D::isDeviceInitted() {
    return vidGrabber.isInitialized();
}

bool Thread2D::isDataAllocated() {
    return isDeviceInitted() && isAllocated;
}

void Thread2D::exit() {
    b_exit = true;
    ofLogVerbose() << "[Thread2D::exit]";
    pthread_mutex_destroy(&uiMutex);
    stopThread();
}
