#include "Thread3D.h"

void Thread3D::threadedFunction() {
    dataAllocated = false;

	char buff[30];
	openNIRecorder = new ofxOpenNI(context->deviceInstance);
	openNIRecorder->setup();

	cout << "openNIRecorder->getNumDevices() " << openNIRecorder->getNumDevices() << endl;
    pthread_mutex_init(&uiMutex, NULL);
	if(context->use2D == 1) {
	    openNIRecorder->addImageGenerator();
	}

	if(context->use3D == 1) {
	    openNIRecorder->addDepthGenerator();
	}
	ofLogVerbose() << "[Thread3D::threadedFunction] - context->use3D: " << context->use3D;

    ofLogVerbose() << "[Thread3D::threadedFunction] - row1.x: " << context->row1.x << ", row1.y: " << context->row1.y << ", row1.z: " << context->row1.z << ", row1.w: " << context->row1.w << ", row1.id"  << context->id;
    ofLogVerbose() << "[Thread3D::threadedFunction] - row2.x: " << context->row2.x << ", row2.y: " << context->row2.y << ", row2.z: " << context->row2.z << ", row2.w: " << context->row2.w << ", row2.id"  << context->id;
    ofLogVerbose() << "[Thread3D::threadedFunction] - row3.x: " << context->row3.x << ", row3.y: " << context->row3.y << ", row3.z: " << context->row3.z << ", row3.w: " << context->row3.w << ", row3.id"  << context->id;
    ofLogVerbose() << "[Thread3D::threadedFunction] - row4.x: " << context->row4.x << ", row4.y: " << context->row4.y << ", row4.z: " << context->row4.z << ", row4.w: " << context->row4.w << ", row4.id"  << context->id;

    openNIRecorder->setRegister(true);
    openNIRecorder->setMirror(true);
    openNIRecorder->setUseDepthRawPixels(true);


    if((context->use2D == 1)) {
        img.allocate(context->resolutionX, context->resolutionX, OF_IMAGE_COLOR);
    }

	string path = "cameras/3D/" + ofToString(context->id);

    if(sys_data->persistence == 1) {
        ofDirectory::createDirectory(path, true, true);
        sprintf( buff, "%u", ofGetSystemTimeMicros() );
        openNIRecorder->startRecording( ofToDataPath( path  + "/oni_" + ofToString(ofGetUnixTime()) + "_" + buff + ".oni") );
    } else {
        openNIRecorder->start();
    }

    started = true;
    //ofAddListener(ofEvents().update, this, &Thread3D::process);

    unsigned long long minMillis = 1000/sys_data->fps;
    unsigned long long currMill, baseMill;

    while(isThreadRunning()) {
        baseMill = ofGetElapsedTimeMillis();

        process();
        currMill = ofGetElapsedTimeMillis();
        if((currMill - baseMill) < minMillis) {
            ofSleepMillis(minMillis - (currMill - baseMill));
        }
    }

}

void Thread3D::process(ofEventArgs &e) {}

void Thread3D::process() {
    if(!started) return;
    if(!idle) {
        ofLogVerbose() << "[Thread3D::process] :: NO IDLE / FPS ";
        return;
    }
    idle = false;

    ofLogVerbose() << "[Thread3D::process] :: IDLE / FPS ";

    openNIRecorder->update();
	updateData();

    idle = true;
}

void Thread3D::updateData() {
    dataAllocated = false;
    if(openNIRecorder->isNewFrame()) {
        deviceInited    = true;
    }

    if(deviceInited) {
        //lock();
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
        //unlock();
    }
}

bool Thread3D::isDeviceInitted() {
    return deviceInited;
}

bool Thread3D::isDataAllocated() {
    return dataAllocated;
}

void Thread3D::exit() {
    b_exit = true;
    ofLogVerbose() << "[Thread3D::exit]";
    pthread_mutex_destroy(&uiMutex);
    stopThread();
}
