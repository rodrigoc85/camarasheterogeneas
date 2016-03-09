#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() {
    tope = 0;
    base = 0;

    for(int i = 0; i<MAX_BUFF_SIZE; i++) {
        buffer[i] = NULL;
    }
}

FrameBuffer::~FrameBuffer() {
    tope = 0;
    base = 0;
    ofLogVerbose() << "[FrameBuffer::~FrameBuffer]";
    for(int i = 0; i<MAX_BUFF_SIZE; i++) {
        try {
            if(buffer[i] != NULL) {
                ThreadData * prevFrame  = buffer[i];
                while(prevFrame != NULL) {
                    ThreadData * td = prevFrame;
                    prevFrame = prevFrame->sig;
                    //if(td->deletable) {
                    //    cout << "DELETABLE EN TRUE" << endl;
                        delete td;
                    /*} else {
                        cout << "DELETABLE EN FALSE" << endl;
                    }*/
                }
            }
            buffer[i]   = NULL;
        } catch (int e) {
            ofLogWarning() << "[FrameBuffer::~FrameBuffer]: An exception occurred. Exception Nr. " << e;
        }
    }
}

void FrameBuffer::addFrame(ThreadData * frame, int totalCams) {
    if(totalCameras == 0 ) return;
    //ofLogVerbose() << "[FFrameBuffer::addFrame] - " << "base " << base << ", tope " << tope << ", totalCams " << totalCams;

    if((tope == base) && (buffer[base] != NULL)) {
        try {
            ThreadData * prevFrame  = buffer[base];
            while(prevFrame != NULL) {
                ThreadData * td = prevFrame;
                prevFrame = prevFrame->sig;
                if(td->state > 0) {
                    //PROBLEMA:
                    //Si hago delete no debería pasar nada... sin embargo explota. Por eso uso la función
                    //releaseResources en lugar de delete, para vaciar al menos algo de memoria.
                    //delete td;
                    td->releaseResources();
                }
            }
            if(buffer[base] /*&& buffer[base]->deletable*/) {
                cout << "DELETABLE EN TRUE 2" << endl;
                ofLogWarning() << "[FrameBuffer::addFrame]: DELETABLE EN TRUE 2";
                delete buffer[base];
            }
        } catch (int e) {
            ofLogWarning() << "[FrameBuffer::addFrame]: An exception occurred. Exception Nr. " << e;
        }
        buffer[base] = NULL;
        base         = ((base + 1) % MAX_BUFF_SIZE);
    }
    buffer[tope]        = frame;
    totalCameras[tope]  = totalCams;
    tope         = ((tope + 1) % MAX_BUFF_SIZE);
}

std::pair <int, ThreadData *> FrameBuffer::getHeadFrame() {
    std::pair <int, ThreadData *> retVal;
    if(buffer[base] == NULL) {
        retVal.first    = 0;
        retVal.second   = NULL;
        //ofLogWarning() << "[FrameBuffer::getHeadFrame]: Se llamó a getHeadFrame y estaba vacío. "  << "base " << base << ", tope " << tope;
        return retVal;
        //return NULL;
    }
    retVal.first        = totalCameras[base];
    retVal.second       = buffer[base];
    buffer[base] = NULL;

    base         = ((base + 1) % MAX_BUFF_SIZE);
    return retVal;
}

int FrameBuffer::length() {
    ofLogVerbose() << "[FrameBuffer::length] " << tope << ", " << base;
    if(tope == base){
        if(buffer[base] == NULL) return 0;
        //int max = MAX_BUFFER_SIZE;
        if(buffer[base] != NULL) return MAX_BUFF_SIZE;
    }

    if(tope > base) return tope - base;
    return tope + (MAX_BUFF_SIZE - base);
}
