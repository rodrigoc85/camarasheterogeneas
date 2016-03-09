#include "Transmitter.h"
#include "Base64.h"
#include "ToHex.h"
#include "Grabber.h"

#include <cerrno>

#ifdef TARGET_WIN32
#define ENOTCONN        WSAENOTCONN
#define EWOULDBLOCK     WSAEWOULDBLOCK
#define ENOBUFS         WSAENOBUFS
#define ECONNRESET      WSAECONNRESET
#define ESHUTDOWN       WSAESHUTDOWN
#define EAFNOSUPPORT    WSAEAFNOSUPPORT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define EINPROGRESS     WSAEINPROGRESS
#define EISCONN         WSAEISCONN
#define ENOTSOCK        WSAENOTSOCK
#define EOPNOTSUPP      WSAEOPNOTSUPP
#define ETIMEDOUT       WSAETIMEDOUT
#define EADDRNOTAVAIL   WSAEADDRNOTAVAIL
#define ECONNREFUSED    WSAECONNREFUSED
#define ENETUNREACH     WSAENETUNREACH
#define EADDRINUSE      WSAEADDRINUSE
#define EADDRINUSE      WSAEADDRINUSE
#define EALREADY        WSAEALREADY
#define ENOPROTOOPT     WSAENOPROTOOPT
#define EMSGSIZE        WSAEMSGSIZE
#define ECONNABORTED    WSAECONNABORTED
#endif

bool connected = false;
int retry;
//ofImage im;
void Transmitter::threadedFunction() {
    //im.loadImage("tibu.jpg");
    HINSTANCE hGetProcIDDLL;
    hGetProcIDDLL   =  LoadLibraryA("imageCompression.dll");
    if (!hGetProcIDDLL) {
        std::cout << "No se pudo cargar la libreria: " << std::endl;
    }
    compress_img    = (f_compress_img)   GetProcAddress(hGetProcIDDLL, "compress_img");

    HINSTANCE hGetProcIDDLLPC;
    hGetProcIDDLLPC   =  LoadLibraryA("FrameCompression.dll");
    if (!hGetProcIDDLLPC) {
        std::cout << "No se pudo cargar la libreria: " << std::endl;
    }

	frame_compress     = (f_compress)       GetProcAddress(hGetProcIDDLLPC, "frame_compress");
    frame_uncompress   = (f_uncompress)     GetProcAddress(hGetProcIDDLLPC, "frame_uncompress");

    retry           = -1;
    state           = -1;

    std::string str =  ofToString(sys_data->cliId) + "|" + ofToString(sys_data->cliPort);
    cliId           = str;

    started = true;

    cout << ">> DESCONECTADO " << endl;
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

bool Transmitter::checkConnError() {
    #ifdef TARGET_WIN32
    int	err	= WSAGetLastError();
    #else
    int err = errno;
    #endif

    if(connectionClosed) return true;
    switch(err) {
        case EBADF:
        case ECONNRESET:
        case EINTR: //EINTR: receive interrupted by a signal, before any data available");
		case ENOTCONN: //ENOTCONN: trying to receive before establishing a connection");
        case ENOTSOCK: //ENOTSOCK: socket argument is not a socket");
        case EOPNOTSUPP: //EOPNOTSUPP: specified flags not valid for this socket");
        case ETIMEDOUT: //ETIMEDOUT: timeout");
        case EIO: //EIO: io error");
        case ENOBUFS: //ENOBUFS: insufficient buffers to complete the operation");
        case ENOMEM: //ENOMEM: insufficient memory to complete the request");
        case EADDRNOTAVAIL: //EADDRNOTAVAIL: the specified address is not available on the remote machine");
        case EAFNOSUPPORT: //EAFNOSUPPORT: the namespace of the addr is not supported by this socket");
        case EISCONN: //EISCONN: the socket is already connected");
        case ECONNREFUSED: //ECONNREFUSED: the server has actively refused to establish the connection");
        case ENETUNREACH: //ENETUNREACH: the network of the given addr isn't reachable from this host");
        case EADDRINUSE: //EADDRINUSE: the socket address of the given addr is already in use");
        case EINPROGRESS: //EINPROGRESS: the socket is non-blocking and the connection could not be established immediately" );
        case EALREADY: //EALREADY: the socket is non-blocking and already has a pending connection in progress");
        case ENOPROTOOPT: //ENOPROTOOPT: The optname doesn't make sense for the given level.");
        case EPROTONOSUPPORT: //EPROTONOSUPPORT: The protocol or style is not supported by the namespace specified.");
        case EMFILE: //EMFILE: The process already has too many file descriptors open.");
        case ENFILE: //ENFILE: The system already has too many file descriptors open.");
        case EACCES: //EACCES: The process does not have the privilege to create a socket of the specified 	 style or protocol.");
        case EMSGSIZE: //EMSGSIZE: The socket type requires that the message be sent atomically, but the message is too large for this to be possible.");
        case EPIPE:         connectionClosed = true;
                            break;
    }
    return connectionClosed;
}

int prevState = 0;

void Transmitter::process(ofEventArgs &e) {}
void Transmitter::process() {

    if(connectionClosed) return;
    if(!started) return;

    if(!idle) {
        return;
    }

    connected = grabber->isConnected();

    if(connected && (state == -1)) {
        state = 0;
        return;
    }

    if(!connected && (state == -1)) {
        if(retry > 0) {
            retry--;
        }
        if(retry == 0) {
            cout << ">> INICIANDO RECONEXIÓN CON EL SERVIDOR " << endl;
            ofLogVerbose()  << "[Transmitter::process]>> INICIANDO RECONEXIÓN CON EL SERVIDOR";
            state = 0;
        }
    }

    idle = false;

    if(state == 0) { // 0 - Todavía no se conectó al servidor
        try {
            ofLogVerbose() << "[Transmitter::process] state=0, conectando a " << sys_data->serverIp << "-" << sys_data->serverPort;
            TCP.setup( sys_data->serverIp, sys_data->serverPort );
            ofLogVerbose()  << "[Transmitter::process] TCP.isClientConnected(0) " << TCP.isConnected();
            if(TCP.isConnected()) {
                TCPSVR.setup(sys_data->cliPort, true);
                TCP.send( cliId );
                TCP.close();
                state = 2;
                grabber->setConnected(true);
                connected = grabber->isConnected();
                ofLogVerbose() << " IS CONNECTED CON EL SERVIDOR " << TCPSVR.isConnected();
            }
            cout << ">> CONECTADO A IP: " << sys_data->serverIp << ", PUERTO: " << sys_data->serverPort << endl;
            ofLogVerbose()  << "[Transmitter::process]>> CONECTADO A IP: " << sys_data->serverIp << ", PUERTO: " << sys_data->serverPort;
        } catch (int e) {
            ofLogVerbose() << "[Transmitter::process] An exception occurred. Exception Nr. " << e;
            state       = 3;
        }
    } else if(state == 2) { // 2 - Tiene cliente asignado.
        try {
            lock();
            ofLogVerbose()  << "[Transmitter::process] TCPSVR.getNumClients() " << TCPSVR.getNumClients() << ", TCPSVR.isClientConnected(0) " << TCPSVR.isClientConnected(0) << ",  TCPSVR.isConnected() " <<  TCPSVR.isConnected();
            if(TCPSVR.getNumClients() > 0) {
                if(TCPSVR.isClientConnected(0)) {
                    ofLogVerbose()  << endl << "[Transmitter::process] Actualizando ultima informacion:";
                    grabber->updateThreadData();
                    ofLogVerbose()  << "[Transmitter::process] Saliendo de actualizar";
                    ofLogVerbose() << "[Transmitter::process] suma de camaras: " << (grabber->total2D + grabber->total3D + grabber->totalONI);

                    if(grabber->tData && (grabber->total2D + grabber->total3D + grabber->totalONI)>0) {
                        ofLogVerbose() << "[Transmitter::process] " << grabber->tData;
                        sendFrame((grabber->total2D + grabber->total3D + grabber->totalONI), grabber->tData);
                    }

                } else {
                    ofLogVerbose() << "[Transmitter::process] El servidor no está conectado.";
                    state = -1;
                    grabber->setConnected(false);
                    connected = grabber->isConnected();
                    TCPSVR.close();
                    retry   = 50;
                }
            }
            unlock();
        } catch (int e) {
            ofLogVerbose() << "[Transmitter::process] An exception occurred. Exception Nr. " << e;
            state       = 3;
        }
    } else {
        //ofLogVerbose() << "[Transmitter::process] state=3";
    }

    idle = true;
}

void Transmitter::sendFrame(int totalCams, ThreadData * tData) {
    ofLogVerbose() << "[Transmitter::sendFrame]";
    //ofLogVerbose() << " al entrar a serdFrame " << tData[0].nubeH << endl;
    /*
    La idea es simular ahora el envío de un frame completo incluyendo sus imágenes y nubes de punto.
     1) - Formar gran bytearray // Pronto.
     2) - Dado el tamaño de los bloques en los que voy a tener que partir el gran bytearray,
          separarlo agregando marcas para poder verificar y reordenarlo del otro lado.
          Para control pensaba agregar por c/bloque: id(No de Frame), total(Total de bloques en los que se partió el bytearray), index (Número de ese bloque en la secuencia)
     3) Calcular algún tipo de checksum de verificación del frame completo para poder verificar que los datos están más o menos bien.
    */

    int imageBytesToSend    = 0;
    int totalBytesSent      = 0;
    int messageSize         = 0;

    FrameUtils::compressImages(tData, totalCams, compress_img);

    int frameSize       = FrameUtils::getFrameSize(tData, totalCams);
    int origFrameSize   = frameSize;
    char * bytearray    = FrameUtils::getFrameByteArray(tData, totalCams, frameSize);
    //ofLogVerbose()  << "[Transmitter::sendFrame] a";
    //COMPRESION
    vector< unsigned char > result;
    if(sys_data->allowCompression) {
        std::vector<unsigned char> src(bytearray, bytearray + frameSize);
        result      = frame_compress(src);
        frameSize   = result.size();
        free(bytearray);
        bytearray   = new char[result.size()];
        memcpy(bytearray, (char *) &result[0], result.size());
    }
    //FIN COMPRESION

    // new
    int val0  = floor(frameSize / sys_data->maxPackageSize);   //totMaxRecSize
    int val1  = frameSize - val0 * sys_data->maxPackageSize; //resto
    if(!((val0 >= -10) && (val0 <= 1000) && (val1 >= 0) && (val1 <= sys_data->maxPackageSize))) {
        free(bytearray);
        int i = 0;
        for(i=0; i<totalCams; i++) {
            if(tData[i].state > 0) {
                if((tData[i].state == 1) || (tData[i].state == 3)) {
                    free(tData[i].compImg);
                }
            }
        }
        return;
    }
    if((TCPSVR.getNumClients() > 0) && (TCPSVR.isClientConnected(0) > 0)) {
        TCPSVR.sendRawBytes(0, (char*) &val0, sizeof(int));
        TCPSVR.sendRawBytes(0, (char*) &val1, sizeof(int));

        ofLogVerbose()  << endl;
        ofLogVerbose()  << "[Transmitter::sendFrame] ENVIANDO NUEVO FRAME:";
        ofLogVerbose()  << "[Transmitter::sendFrame] Total de cámaras: " << totalCams;
        ofLogVerbose()  << "[Transmitter::sendFrame] Tamaño original del frame: " << origFrameSize << " bytes";
        if(sys_data->allowCompression) {
            ofLogVerbose()  << "[Transmitter::sendFrame] Tamaño comprimido del frame: " << frameSize << " bytes";
        }
        ofLogVerbose()  << "[Transmitter::sendFrame] v0: " << val0  << ", v1:" << val1;
        ofLogVerbose()  << "[Transmitter::sendFrame] Cantidad de paquetes enviados: " << (val0 + 1);
        ofLogVerbose()  << "[Transmitter::sendFrame] Conexiones activas: " << TCPSVR.getNumClients();
        ofLogVerbose()  << endl;

        // FIN DE INTENTO ENVIAR FOTO A VER SI FALLA

        imageBytesToSend    = frameSize;
        totalBytesSent      = 0;
        messageSize         = sys_data->maxPackageSize;
        int guarda          = 100;
        while( (imageBytesToSend > 1) && (guarda>0) ) {
            ofLogVerbose()  << "[Transmitter::sendFrame] TCPSVR.getNumClients() " << TCPSVR.getNumClients() << ", TCPSVR.isClientConnected(0) " << TCPSVR.isClientConnected(0) << ",  TCPSVR.isConnected() " <<  TCPSVR.isConnected();
            if(imageBytesToSend > messageSize) {
                if(TCPSVR.getNumClients() > 0) {
                    TCPSVR.sendRawBytes(0, (const char*) &bytearray[totalBytesSent], messageSize);
                    imageBytesToSend    -= messageSize;
                    totalBytesSent      += messageSize;
                }
            } else {
                if(TCPSVR.getNumClients() > 0) {
                    TCPSVR.sendRawBytes(0, (char*) &bytearray[totalBytesSent], imageBytesToSend);
                    totalBytesSent += imageBytesToSend;
                    imageBytesToSend = 0;
                }
            }
            //ofLogVerbose() << "[Transmitter::sendFrame] envando " << guarda;
            guarda--;
        }
        if(guarda <= 0) {
            ofLogVerbose() << "[Transmitter::sendFrame] FALLÓ EL ENVÍO ";
        }
    }

    if(TCPSVR.getNumClients() <= 0) {
        ofLogVerbose() << "[Transmitter::sendFrame] NO HAY MÁS CONEXIONES ABIERTAS ";
    } else {
        ofLogVerbose() << "[Transmitter::sendFrame] HAY CONEXIONES ABIERTAS ";
    }

    string llego = TCPSVR.receive(0);

    free(bytearray);
    int i = 0;
    for(i=0; i<totalCams; i++) {
        if(tData[i].state > 0) {
            if((tData[i].state == 1) || (tData[i].state == 3)) {
                free(tData[i].compImg);
            }
        }
    }

    connected = grabber->isConnected();
    if(!connected && (state == 2)) {
        cout << ">> DESCONECTANDO DEL SERVIDOR" << endl;
        ofLogVerbose()  << "[Transmitter::sendFrame] DESCONECTANDO DEL SERVIDOR";
        ofSleepMillis(2000);
        state    = -1;
        retry    = -1;
        int conn = -10;
        cout << ">> DESCONECTADO" << endl;
        TCPSVR.sendRawBytesToAll((char*) &conn, sizeof(int));
        TCPSVR.close();
        ofLogVerbose()  << "[Transmitter::sendFrame] DESCONECTADO";
        return;
    }
}

bool Transmitter::connError(std::string msj, bool unl) {
    if(checkConnError()) {
        //ofLogVerbose() << ">>[Transmitter::receiveFrame] connError - Error " << msj;
        if(unl) unlock();
        return true;
    } else {
        //ofLogVerbose() << ">>[Transmitter::receiveFrame] connError - Ok " << msj;
    }
    return false;
}

void Transmitter::exit() {
    ofLogVerbose() << "[Transmitter::exit]";

    b_exit = true;
    connected   = grabber->isConnected();
    cout << "[Transmitter::exit] " << endl;
    if(connected && (state == 2)) {
        cout << ">> DESCONECTANDO DEL SERVIDOR" << endl;
        ofLogVerbose()  << "[Transmitter::sendFrame] DESCONECTANDO DEL SERVIDOR";
        ofSleepMillis(1000);
        state    = -1;
        retry    = -1;
        int conn = -10;
        TCPSVR.sendRawBytesToAll((char*) &conn, sizeof(int));
        TCPSVR.close();
        cout << ">> DESCONECTADO" << endl;
        ofLogVerbose()  << "[Transmitter::sendFrame] DESCONECTADO";
        return;
    }
}
