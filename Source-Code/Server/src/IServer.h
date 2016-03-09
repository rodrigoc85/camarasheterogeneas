#ifndef ISERVER_H
#define ISERVER_H

#include "ofMain.h"
#include "ThreadData.h"
//#include "ThreadServer.h"

class IServer : public ofBaseApp {
    public:
        virtual void computeFrames() = 0;
        virtual void threadServerReady(void * ntsrv) = 0;
        virtual void threadServerClosed(void * ntsrv) = 0;
    protected:
    private:
};

#endif // ISERVER_H
