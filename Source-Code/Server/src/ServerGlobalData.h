#pragma once
#include "ofxXmlSettings.h"
#include "ofMain.h"
#include "ofMatrix4x4.h"
#include "ServerGlobalData.h"

struct t_data {
    string   serverIp;
    int      serverPort;
    int      fps;
    int      persistToPly;
    int      logLevel;
    int      maxPackageSize;
    int      syncFactorValue;
    int      maxThreadedServers;
    int      maxReceiveFrameBuffer;
    int      processMostRecent;
    int      totalFreeCores;
    bool     allowCompression;
};

class ServerGlobalData {
	public:
        static bool     debug;
        void            loadCalibData(char * xml);
        t_data          * sys_data;
};
