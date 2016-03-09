#pragma once

#include "ofMain.h"
#include "IGrabber.h"
#include "ofxNetwork.h"
#include "ThreadData.h"
#include "GlobalData.h"
#include "FrameUtils.h"

typedef std::vector< unsigned char > (*f_compress) (const std::vector< unsigned char > & src);
typedef std::vector< unsigned char > (*f_uncompress) (const std::vector< unsigned char > & src);

class Transmitter : public ofThread {

	public:
		void threadedFunction();
		void process(ofEventArgs &e);
		void process();
		bool checkConnError();
		bool connError(std::string, bool);
		void exit();
        int camWidth;
        int camHeight;
        IGrabber * grabber;
        f_compress_img      compress_img;
        f_compress          frame_compress;
        f_uncompress        frame_uncompress;
        ofxTCPClient TCP;
        ofxTCPServer TCPSVR;

		int state; // 0 - todavía no se conectó al servidor, 1 - todavía no tiene cliente asignado, 2 - tiene cliente asignado.
		string cliId;
		int serverPort;
		int go;
        t_data * sys_data;
		void sendFrame(int totalCams, ThreadData * tData);
        bool connectionClosed;
        bool started;
        bool idle;
        bool b_exit;
        Transmitter() {
            idle                = true;
            started             = false;
            connectionClosed    = false;
            b_exit              = false;
        }

        ~Transmitter() {
            b_exit = true;
            if(!started) return;
            //ofRemoveListener(ofEvents().update, this, &Transmitter::process);
        }
};
