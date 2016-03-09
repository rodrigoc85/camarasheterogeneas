#define MAX_RECEIVE_SIZE 60000
#define MAX_THREADED_SERVERS 100

/**
* SERVER: Ip donde estará publicado el servidor.
* PORT_0: Puerto en el que estará publicado el servidor. Los threads
* que atienden a cada cliente utilizan puertos consecutivos a partir
* de PORT_0.
*/
#define  SERVER "127.0.0.1"
#define  PORT_0 11969

#define  SYNC_FACTOR_VALUE 10
#define  MAX_MAIN_BUFF_SIZE 100


/**
* MAX_BUFFER_SIZE: Tamaño máximo del buffer que almacena los frames recibidos del cliente.
* Usada en FrameBuffer y ThreadServer;
*/
#define  MAX_BUFFER_SIZE 10

/**
* FPS: Velocidad de procesamiento del servidor.
* Usada en ofSetFrameRate(FPS);
*/
#define  FPS    1

/**
* SERVER_BUFF_FPS: Granularidad a la que el servidor procesa y
* sincroniza los frames que recibe.
*/
#define  SERVER_BUFF_FPS 10

#define DEVICE_2D 1
#define DEVICE_3D 2
