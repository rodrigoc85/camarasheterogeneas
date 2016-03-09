#include "modelSetting.h"
#include <vector>
#include <limits>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GlobalData.h"


using namespace std;

typedef void (*f_ReadSharedSetting)(int* numberValues, int** idsValues, float** values);

Model_SET::Model_SET() {

}

void Model_SET::FileLoad() {

    GlobalData* gdata = new GlobalData();
    gdata->loadCalibData("settings.xml");

    NValues = gdata->getTotal2D();//Total de camaras 2D

    IdsValues = new int[NValues+1];

    ValuesA = new float[NValues * 16+16];
    ValuesB = new float[NValues * 16+16];

    int count2DCameras = 0;

    ValuesA[count2DCameras*16] = 1;
    ValuesA[count2DCameras*16+1] = 0;
    ValuesA[count2DCameras*16+2] = 0;
    ValuesA[count2DCameras*16+3] = 0;

    ValuesA[count2DCameras*16+4] = 0;
    ValuesA[count2DCameras*16+5] = 1;
    ValuesA[count2DCameras*16+6] = 0;
    ValuesA[count2DCameras*16+7] = 0;

    ValuesA[count2DCameras*16+8] = 0;
    ValuesA[count2DCameras*16+9] = 0;
    ValuesA[count2DCameras*16+10] = 1;
    ValuesA[count2DCameras*16+11] = 0;

    ValuesA[count2DCameras*16+12] = 0;
    ValuesA[count2DCameras*16+13] = 0;
    ValuesA[count2DCameras*16+14] = 0;
    ValuesA[count2DCameras*16+15] = 1;

    ValuesB[count2DCameras*16] = 1;
    ValuesB[count2DCameras*16+1] = 0;
    ValuesB[count2DCameras*16+2] = 0;
    ValuesB[count2DCameras*16+3] = 0;

    ValuesB[count2DCameras*16+4] = 0;
    ValuesB[count2DCameras*16+5] = 1;
    ValuesB[count2DCameras*16+6] = 0;
    ValuesB[count2DCameras*16+7] = 0;

    ValuesB[count2DCameras*16+8] = 0;
    ValuesB[count2DCameras*16+9] = 0;
    ValuesB[count2DCameras*16+10] = 1;
    ValuesB[count2DCameras*16+11] = 0;

    ValuesB[count2DCameras*16+12] = 0;
    ValuesB[count2DCameras*16+13] = 0;
    ValuesB[count2DCameras*16+14] = 0;
    ValuesB[count2DCameras*16+15] = 1;


    IdsValues[count2DCameras] = 0;

    count2DCameras+= 1;

    for (int i = 0 ; i < gdata->nClientes ; i++){// recorro los clientes

        for(int j = 0; j < gdata->sys_data[i].nCamaras ; j++){//recorro las camaras de un cliente
            if (gdata->sys_data[i].camera[j].use2D){
                if (gdata->sys_data[i].camera[j].use3D)
                    IdsValues[count2DCameras] =  gdata->sys_data[i].cliId*100 + 2 * 10 + gdata->sys_data[i].camera[j].id;
                else
                    IdsValues[count2DCameras] =  gdata->sys_data[i].cliId*100 + 1 * 10 + gdata->sys_data[i].camera[j].id;

                //MATRIX A
                ValuesA[count2DCameras*16] = gdata->sys_data[i].camera[j].imgrowA1[0];
                ValuesA[count2DCameras*16+1] = gdata->sys_data[i].camera[j].imgrowA1[1];
                ValuesA[count2DCameras*16+2] = gdata->sys_data[i].camera[j].imgrowA1[2];
                ValuesA[count2DCameras*16+3] = gdata->sys_data[i].camera[j].imgrowA1[3];

                ValuesA[count2DCameras*16+4] = gdata->sys_data[i].camera[j].imgrowA2[0];
                ValuesA[count2DCameras*16+5] = gdata->sys_data[i].camera[j].imgrowA2[1];
                ValuesA[count2DCameras*16+6] = gdata->sys_data[i].camera[j].imgrowA2[2];
                ValuesA[count2DCameras*16+7] = gdata->sys_data[i].camera[j].imgrowA2[3];

                ValuesA[count2DCameras*16+8] = gdata->sys_data[i].camera[j].imgrowA3[0];
                ValuesA[count2DCameras*16+9] = gdata->sys_data[i].camera[j].imgrowA3[1];
                ValuesA[count2DCameras*16+10] = gdata->sys_data[i].camera[j].imgrowA3[2];
                ValuesA[count2DCameras*16+11] = gdata->sys_data[i].camera[j].imgrowA3[3];

                ValuesA[count2DCameras*16+12] = gdata->sys_data[i].camera[j].imgrowA4[0];
                ValuesA[count2DCameras*16+13] = gdata->sys_data[i].camera[j].imgrowA4[1];
                ValuesA[count2DCameras*16+14] = gdata->sys_data[i].camera[j].imgrowA4[2];
                ValuesA[count2DCameras*16+15] = gdata->sys_data[i].camera[j].imgrowA4[3];

                //MATRIX B
                ValuesB[count2DCameras*16] = gdata->sys_data[i].camera[j].imgrowB1[0];
                ValuesB[count2DCameras*16+1] = gdata->sys_data[i].camera[j].imgrowB1[1];
                ValuesB[count2DCameras*16+2] = gdata->sys_data[i].camera[j].imgrowB1[2];
                ValuesB[count2DCameras*16+3] = gdata->sys_data[i].camera[j].imgrowB1[3];

                ValuesB[count2DCameras*16+4] = gdata->sys_data[i].camera[j].imgrowB2[0];
                ValuesB[count2DCameras*16+5] = gdata->sys_data[i].camera[j].imgrowB2[1];
                ValuesB[count2DCameras*16+6] = gdata->sys_data[i].camera[j].imgrowB2[2];
                ValuesB[count2DCameras*16+7] = gdata->sys_data[i].camera[j].imgrowB2[3];

                ValuesB[count2DCameras*16+8] = gdata->sys_data[i].camera[j].imgrowB3[0];
                ValuesB[count2DCameras*16+9] = gdata->sys_data[i].camera[j].imgrowB3[1];
                ValuesB[count2DCameras*16+10] = gdata->sys_data[i].camera[j].imgrowB3[2];
                ValuesB[count2DCameras*16+11] = gdata->sys_data[i].camera[j].imgrowB3[3];

                ValuesB[count2DCameras*16+12] = gdata->sys_data[i].camera[j].imgrowB4[0];
                ValuesB[count2DCameras*16+13] = gdata->sys_data[i].camera[j].imgrowB4[1];
                ValuesB[count2DCameras*16+14] = gdata->sys_data[i].camera[j].imgrowB4[2];
                ValuesB[count2DCameras*16+15] = gdata->sys_data[i].camera[j].imgrowB4[3];

                count2DCameras++;

            }
        }
        alfaCoord = gdata->sys_data[i].alfaCoord;
    }
}
