#include "modelPly.h"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;


Model_PLY::Model_PLY() {
    Id == 0;
}

float* Model_PLY::calculateNormal( float *coord1, float *coord2, float *coord3 )
{
	/* calculate Vector1 and Vector2 */
	float va[3], vb[3], vr[3], val;
	va[0] = coord1[0] - coord2[0];
	va[1] = coord1[1] - coord2[1];
	va[2] = coord1[2] - coord2[2];

	vb[0] = coord1[0] - coord3[0];
	vb[1] = coord1[1] - coord3[1];
	vb[2] = coord1[2] - coord3[2];

	/* cross product */
	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];

	/* normalization factor */
	val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );

	float* norm = new float[3];
	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;

	return norm;
}

void Model_PLY::MemoryLoadCalibrator(FaceStruct* faces, int numberFaces)
{
    Id++;
    TotalConnectedTriangles = numberFaces * 3;
    TotalPoints = numberFaces / 3;
    TotalFaces = numberFaces;
    Faces_Triangles = new float[TotalFaces * 9];
	Normals  = new float[TotalFaces * 9];
    for (int i = 0; i< TotalFaces; i++){
        Faces_Triangles[i*9] = faces[i].p1[0];
        Faces_Triangles[i*9+1] = faces[i].p1[1];
        Faces_Triangles[i*9+2] = faces[i].p1[2];
        Faces_Triangles[i*9+3] = faces[i].p2[0];
        Faces_Triangles[i*9+4] = faces[i].p2[1];
        Faces_Triangles[i*9+5] = faces[i].p2[2];
        Faces_Triangles[i*9+6] = faces[i].p3[0];
        Faces_Triangles[i*9+7] = faces[i].p3[1];
        Faces_Triangles[i*9+8] = faces[i].p3[2];

        float coord1[3] = { Faces_Triangles[i*9], Faces_Triangles[i*9+1], Faces_Triangles[i*9+2] };
        float coord2[3] = { Faces_Triangles[i*9+3], Faces_Triangles[i*9+4], Faces_Triangles[i*9+5] };
        float coord3[3] = { Faces_Triangles[i*9+6], Faces_Triangles[i*9+7], Faces_Triangles[i*9+8] };
        float* norm = calculateNormal(coord1, coord2, coord3);

        Normals[i*9] = norm[0];
        Normals[i*9+1] = norm[1];
        Normals[i*9+2] = norm[2];
        Normals[i*9+3] = norm[0];
        Normals[i*9+4] = norm[1];
        Normals[i*9+5] = norm[2];
        Normals[i*9+6] = norm[0];
        Normals[i*9+7] = norm[1];
        Normals[i*9+8] = norm[2];/**/
    }
    MinCoord = std::numeric_limits<float>::max();
    MaxCoord = std::numeric_limits<float>::min();
    for (int i = 0; i < TotalFaces * 9; i++) {
        if (Faces_Triangles[i] < MinCoord) {
            MinCoord = Faces_Triangles[i];
        }
        if (Faces_Triangles[i] > MaxCoord) {
            MaxCoord = Faces_Triangles[i];
        }
    }
    AlfaCoord = std::max(std::abs(MinCoord), std::abs(MaxCoord));
    for (int i = 0; i < TotalFaces * 9; i++) {
        Faces_Triangles[i] = (Faces_Triangles[i] / AlfaCoord) * 10;
    }

}

void Model_PLY::MemoryLoad()
{
    memoryMappedMeshId.setup("MeshId", sizeof(int), false);
    isConnectedId = memoryMappedMeshId.connect();
    if (isConnectedId) {
        id = memoryMappedMeshId.getData();
    }
    memoryMappedMeshSize.setup("MeshNumberFaces", sizeof(int), false);
    isConnectedNFaces = memoryMappedMeshSize.connect();
    if (isConnectedNFaces) {
        numberFaces = memoryMappedMeshSize.getData();
    }
    memoryMappedMesh.setup("MeshFaces", sizeof(FaceStruct) * (*numberFaces), false);
    isConnectedFaces = memoryMappedMesh.connect();
    if (isConnectedFaces) {
        faces = memoryMappedMesh.getData();
    }

    if (isConnectedId && isConnectedNFaces && isConnectedFaces && *id != Id && *numberFaces > 0) {

        Id = *id;
        TotalConnectedTriangles = (*numberFaces) * 3;
        TotalPoints = (*numberFaces) / 3;
        TotalFaces = *numberFaces;

        FaceStruct* facesAux = new FaceStruct[*numberFaces];
        memcpy(facesAux, faces, sizeof(FaceStruct) * (*numberFaces));

        Faces_Triangles = new float[TotalFaces * 9];
        for (int i = 0; i< TotalFaces; i++){
            Faces_Triangles[i*9] = facesAux[i].p1[0];
            Faces_Triangles[i*9+1] = facesAux[i].p1[1];
            Faces_Triangles[i*9+2] = facesAux[i].p1[2];
            Faces_Triangles[i*9+3] = facesAux[i].p2[0];
            Faces_Triangles[i*9+4] = facesAux[i].p2[1];
            Faces_Triangles[i*9+5] = facesAux[i].p2[2];
            Faces_Triangles[i*9+6] = facesAux[i].p3[0];
            Faces_Triangles[i*9+7] = facesAux[i].p3[1];
            Faces_Triangles[i*9+8] = facesAux[i].p3[2];
        }
        MinCoord = std::numeric_limits<float>::max();
        MaxCoord = std::numeric_limits<float>::min();
        for (int i = 0; i < TotalFaces * 9; i++) {
            if (Faces_Triangles[i] < MinCoord) {
                MinCoord = Faces_Triangles[i];
            }
            if (Faces_Triangles[i] > MaxCoord) {
                MaxCoord = Faces_Triangles[i];
            }
        }
        AlfaCoord = std::max(std::abs(MinCoord), std::abs(MaxCoord));
        for (int i = 0; i < TotalFaces * 9; i++) {
            Faces_Triangles[i] = (Faces_Triangles[i] / AlfaCoord) * 10;
        }
    }
}

void Model_PLY::Load(string filename)
{
	TotalConnectedTriangles = 0;
	TotalPoints = 0;

	char* pch = strstr(filename.c_str(),".ply");

	if (pch != NULL)
	{
		FILE* file = fopen(filename.c_str(),"r");

		fseek(file,0,SEEK_END);
		long fileSize = ftell(file);

		try
		{
			Vertex_Buffer = (float*) malloc (ftell(file));
		}
		catch (char* )
		{
			return;
		}
		if (Vertex_Buffer == NULL) return;
		fseek(file,0,SEEK_SET);

		Faces_Triangles = (float*) malloc(fileSize*sizeof(float));
		Normals  = (float*) malloc(fileSize*sizeof(float));

		if (file)
		{
			int i = 0;
			int triangle_index = 0;
			int normal_index = 0;
			char buffer[1000];

			fgets(buffer,300,file);			// ply

			// READ HEADER
			// Find number of vertexes
			while (  strncmp( "element vertex", buffer,strlen("element vertex")) != 0  )
			{
				fgets(buffer,300,file);			// format
			}
			strcpy(buffer, buffer+strlen("element vertex"));
			sscanf(buffer,"%i", &TotalPoints);

			// Find number of vertexes
			fseek(file,0,SEEK_SET);
			while (  strncmp( "element face", buffer,strlen("element face")) != 0  )
			{
				fgets(buffer,300,file);			// format
			}
			strcpy(buffer, buffer+strlen("element face"));
			sscanf(buffer,"%i", &TotalFaces);

			// go to end_header
			while (  strncmp( "end_header", buffer,strlen("end_header")) != 0  )
			{
				fgets(buffer,300,file);			// format
			}

			// read verteces
			i =0;
			for (int iterator = 0; iterator < TotalPoints; iterator++)
			{
				fgets(buffer,300,file);

				sscanf(buffer,"%f %f %f", &Vertex_Buffer[i], &Vertex_Buffer[i+1], &Vertex_Buffer[i+2]);
				i += 3;
			}

			i = 0;
			int count = 0;
			for (int iterator = 0; iterator < TotalFaces; iterator++)
			{
				fgets(buffer,300,file);

				if (buffer[0] == '3')
				{
					count++;
					int vertex1 = 0, vertex2 = 0, vertex3 = 0;

					buffer[0] = ' ';
					sscanf(buffer,"%i%i%i", &vertex1,&vertex2,&vertex3 );

					//printf("%f %f %f ", Vertex_Buffer[3*vertex1], Vertex_Buffer[3*vertex1+1], Vertex_Buffer[3*vertex1+2]);

					Faces_Triangles[triangle_index] = Vertex_Buffer[3*vertex1];
					Faces_Triangles[triangle_index+1] = Vertex_Buffer[3*vertex1+1];
					Faces_Triangles[triangle_index+2] = Vertex_Buffer[3*vertex1+2];
					Faces_Triangles[triangle_index+3] = Vertex_Buffer[3*vertex2];
					Faces_Triangles[triangle_index+4] = Vertex_Buffer[3*vertex2+1];
					Faces_Triangles[triangle_index+5] = Vertex_Buffer[3*vertex2+2];
					Faces_Triangles[triangle_index+6] = Vertex_Buffer[3*vertex3];
					Faces_Triangles[triangle_index+7] = Vertex_Buffer[3*vertex3+1];
					Faces_Triangles[triangle_index+8] = Vertex_Buffer[3*vertex3+2];

					float coord1[3] = { Faces_Triangles[triangle_index], Faces_Triangles[triangle_index+1], Faces_Triangles[triangle_index+2] };
					float coord2[3] = { Faces_Triangles[triangle_index+3], Faces_Triangles[triangle_index+4], Faces_Triangles[triangle_index+5] };
					float coord3[3] = { Faces_Triangles[triangle_index+6], Faces_Triangles[triangle_index+7], Faces_Triangles[triangle_index+8] };
					float* norm = calculateNormal(coord1, coord2, coord3);

					Normals[normal_index] = norm[0];
					Normals[normal_index+1] = norm[1];
					Normals[normal_index+2] = norm[2];
					Normals[normal_index+3] = norm[0];
					Normals[normal_index+4] = norm[1];
					Normals[normal_index+5] = norm[2];
					Normals[normal_index+6] = norm[0];
					Normals[normal_index+7] = norm[1];
					Normals[normal_index+8] = norm[2];/**/

					normal_index += 9;
					triangle_index += 9;
					TotalConnectedTriangles += 3;
				}

				i += 3;
			}

			fclose(file);
		}

		else { printf("File can't be opened\n"); }
	} else {
		printf("File does not have a .PLY extension. ");
	}

    MinCoord = std::numeric_limits<float>::max();
    MaxCoord = std::numeric_limits<float>::min();
    for (int i = 0; i < TotalConnectedTriangles * 3; i++) {
        if (Faces_Triangles[i] < MinCoord) {
            MinCoord = Faces_Triangles[i];
        }
        if (Faces_Triangles[i] > MaxCoord) {
            MaxCoord = Faces_Triangles[i];
        }
    }
    AlfaCoord = std::max(std::abs(MinCoord), std::abs(MaxCoord));
    for (int i = 0; i < TotalConnectedTriangles * 3; i++) {
        Faces_Triangles[i] = (Faces_Triangles[i] / AlfaCoord) * 10;
    }
    Id++;
}
