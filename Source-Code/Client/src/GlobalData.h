#pragma once

#include "ofxXmlSettings.h"
#include "ofMain.h"
//#include "Constants.h"
#include "ofMatrix4x4.h"

struct t_camera {
    int id;
    int deviceInstance;
    int resolutionX;
    int resolutionY;
    int pcDownSample;
    float resolutionDownSample;
    int fps;
    bool colorRGB;
    bool use2D;
    bool use3D;
    bool useONI;
    string   file;
    bool useCompression;
    int  rgbCompressionQuality;
    float near3D;
    float far3D;
    float points3DDownSample;
    ofMatrix4x4 matrix;
    ofVec3f xyz;
    ofVec3f abc;

    ofVec4f row1;
    ofVec4f row2;
    ofVec4f row3;
    ofVec4f row4;

    bool hasCoef2D;
    ofVec4f coef2Da;
    ofVec4f coef2Db;

    ofVec4f imgrow1;
    ofVec4f imgrow2;
    ofVec4f imgrow3;
    ofVec4f imgrow4;

    t_camera * sig;
};


struct t_data {
    bool     goLive;
    bool     persistence;
    int      logLevel;
    int      cliId;
    int      cliPort;
    string   serverIp;
    int      serverPort;
    int      fps;
    int      maxPackageSize;
    bool     allowCompression;
};

class GlobalData {

	public:

		int     getFPS();
		bool    getGoLive();
		int     getTotal3D();
        int     getTotal2D();
		int     getTotalDevices();

        void    loadCalibData(char * xml);

        int      total3D;
        int      total2D;
        int      totalONI;

        t_camera    * camera;
        t_data      * sys_data;

};
