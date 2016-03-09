#include "Field.h"

using namespace std;

Field::Field() {
    totFrames   = 0;
    enabled     = true;
}

Field::~Field() {
    for (map< long int, ThreadData * >::iterator it = frame_map.begin(); it != frame_map.end(); ++it) {
        //ThreadData * td = ((ThreadData *) it->second);
        //td->releaseResources();
        //delete td;
    }
    ofLogVerbose() << "[Field::~Field]";
}

bool Field::hasCam(long int camId) {
    try {
        return (frame_map.find( camId ) != frame_map.end());
    } catch (std::out_of_range e) {}
    return false;
}

void Field::addFrame(ThreadData * frame, long int camId) {
    frame_map.insert ( std::pair<long int , ThreadData * > (camId, frame) );
}

void Field::releaseResources() {
    for (map< long int, ThreadData * >::iterator it = frame_map.begin(); it != frame_map.end(); ++it) {
        ThreadData * td = ((ThreadData *) it->second);
        //td->releaseResources();
        //delete td;
    }
    //ofLogVerbose() << "[Field::~Field]";
}
