#ifndef MASTERPLY_H
#define MASTERPLY_H
#include <time.h>

// mesh definition
#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/create/ball_pivoting.h>
#include <wrap/ply/plylib.h>
//#include "plylib.h"

#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/refine.h>

// input output
/**/
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/export_stl.h>
#include <wrap/io_trimesh/export_obj.h>


#include <vcg/complex/algorithms/smooth.h>

// std
#include <vector>

using namespace vcg;
using namespace std;
using namespace tri;

struct FaceStruct
{
    float p1[3];
    float p2[3];
    float p3[3];
};

struct VertexStruct
{
    float v[3];
};

class MyVertex; class MyEdge; class MyFace;
struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
                                           //vcg::Use<MyEdge>     ::AsEdgeType,
                                           vcg::Use<MyFace>     ::AsFaceType>{};
class MyVertex  : public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::BitFlags  >{};
class MyFace    : public vcg::Face<   MyUsedTypes, vcg::face::FFAdj,  vcg::face::VertexRef,   vcg::face::Normal3f, vcg::face::FFAdj, vcg::face::Mark, vcg::face::VFAdj,  vcg::face::BitFlags > {};
//class MyEdge    : public vcg::Edge<   MyUsedTypes> {};
class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace>  > {};


class MasterPly
{
    public:
        MyMesh m;
        MyMesh subM;

        MasterPly();
        void Sampling(char* input, char* output);
        void loadPLY(char* fileName);

        void loadMesh(float* pointsX, float* pointsY, float* pointsZ, int pointsCount);
        void poissonDiskSampling(int sampleNum);
        void calculateNormalsVertex();
        void buildMeshBallPivoting();
        void savePLY(char* fileName, bool binary);

        void laplacianSmooth(int step);

        int totalVertex();
		int totalFaces(void);
        FaceStruct getFace(int i);

        FaceStruct* getFaces();
        VertexStruct getFaceVertex(int faceNumber, int vertexNumber);
        ~MasterPly();
    protected:
    private:
};

#endif // MASTERPLY_H
