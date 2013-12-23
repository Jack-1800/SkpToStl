#include "slapi.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define pos(a,b) ((a)+((b)*4))
using namespace std;



class Vector3{
public:
    double x,y,z;
    Vector3 Cross(const Vector3& V){
        Vector3 v;
        v.x = y * V.z - z * V.y;
        v.y = z * V.x - x * V.z;
        v.z = x * V.y - y * V.x;
        return v;
    }
    double Dot(const Vector3& V){
        double res;
        res = x*V.x + y*V.y + z * V.z;
        return res;
    }
    Vector3 operator - (const Vector3 V){
        Vector3 v;
        v.x = x - V.x;
        v.y = y - V.y;
        v.z = z - V.z;
        return v;
    }
};
class cFacet{
public:
    Vector3 normal, vertex[3];

    Vector3 Middle(){
        Vector3 v;
        v.x = v.y = v.z = 0;
        for (int i = 0; i < 3; i++){
            v.x += vertex[i].x;
            v.y += vertex[i].y;
            v.z += vertex[i].z;
        }
        v.x /= 3;
        v.y /= 3;
        v.z /= 3;
        return v;
    }
};



FILE *fp;
char* skpFileName;
char* stlFileName;
int triangleCount, faceCount, entitiesCount;
SUTransformation matrixI;
SUMeshHelperRef faceMesh;
vector<cFacet> facetArr;
/*
判断三角形和射线是否相交
*/

bool IntersectTriangle(Vector3& orig, Vector3& dir,
    Vector3& v0, Vector3& v1, Vector3& v2)
{
    double t,  u,  v;
    // E1
    Vector3 E1 = v1 - v0;

    // E2
    Vector3 E2 = v2 - v0;

    // P
    Vector3 P = dir.Cross(E2);

    // determinant
    double det = E1.Dot(P);

    // keep det > 0, modify T accordingly
    Vector3 T;
    if( det >0 )
    {
        T = orig - v0;
    }
    else
    {
        T = v0 - orig;
        det = -det;
    }

    // If determinant is near zero, ray lies in plane of triangle
    if( det < 0.0001f )
        return false;

    // Calculate u and make sure u <= 1
    u = T.Dot(P);
    if( u < 0.0f || u > det )
        return false;

    // Q
    Vector3 Q = T.Cross(E1);

    // Calculate v and make sure u + v <= 1
    v = dir.Dot(Q);
    if( v < 0.0f || u + v > det )
        return false;

    // Calculate t, scale parameters, ray intersects triangle
    t = E2.Dot(Q);

    if (t < 0.0f) return false;
    return true;
}

/*
将skp中的拆分成三角片面
*/



void countFaces(SUEntitiesRef entities, SUTransformation transforMation){
	size_t faceLen = 0;
	SUEntitiesGetNumFaces(entities, &faceLen);
	if (faceLen > 0){
	    std::vector<SUFaceRef> faces(faceLen);
		SUEntitiesGetFaces(entities, faceLen, &faces[0], &faceLen);
		for (size_t i = 0; i < faceLen; i++){

			SUMeshHelperCreate(&faceMesh, faces[i]);

			size_t triangleLen;
			SUMeshHelperGetNumTriangles(faceMesh, &triangleLen);
			std::vector<SUVector3D> normalArr(triangleLen);
			SUMeshHelperGetNormals(faceMesh, triangleLen, &normalArr[0], &triangleLen);

            size_t vertexLen;
            SUMeshHelperGetNumVertices(faceMesh, &vertexLen);
            std::vector<SUPoint3D> vertexArr(vertexLen);
            SUMeshHelperGetVertices(faceMesh, vertexLen, &vertexArr[0], &vertexLen);

            size_t indexLen = triangleLen*3;
            std::vector<size_t> indexArr(indexLen);
            SUMeshHelperGetVertexIndices(faceMesh, indexLen, &indexArr[0], &indexLen);
			SUMeshHelperRelease(&faceMesh);

            for (size_t i = 0; i < triangleLen; i++){

                    double normal[3]= {normalArr[i].x, normalArr[i].y, normalArr[i].z};
                    double normalTrans[3] = {0};
                    double vertexTrans[3][3] = {0};
                    for (int ii = 0; ii < 3; ii++)
                        for (int jj = 0; jj < 3; jj++)
                            normalTrans[ii] += normal[jj] *  transforMation.values[pos(ii, jj)];
                    for (size_t j = 0; j < 3; j++){
                        double vertex[3] = {vertexArr[indexArr[i*3+j]].x, vertexArr[indexArr[i*3+j]].y, vertexArr[indexArr[i*3+j]].z};
                        for (int ii = 0; ii < 3; ii++)
                            for (int jj = 0; jj < 3; jj++)
                                vertexTrans[j][ii] += vertex[jj] *  transforMation.values[pos(ii, jj)];
                        for (int ii = 0; ii < 3; ii++)
                            vertexTrans[j][ii] += transforMation.values[pos(ii, 3)];
                    }

                    double x0 = vertexTrans[1][0] -  vertexTrans[0][0];
                    double y0 = vertexTrans[1][1] -  vertexTrans[0][1];
                    double z0 = vertexTrans[1][2] -  vertexTrans[0][2];

                    double x1 = vertexTrans[2][0] -  vertexTrans[0][0];
                    double y1 = vertexTrans[2][1] -  vertexTrans[0][1];
                    double z1 = vertexTrans[2][2] -  vertexTrans[0][2];

                    double xi = y0 * z1 - z0 * y1;
                    double yi = z0 * x1 - x0 * z1;
                    double zi = x0 * y1 - y0 * x1;

                    double docProduct = xi * normalTrans[0] + yi * normalTrans[1] + zi * normalTrans[2];

                    if (docProduct < 0){
                        for (int ii = 0; ii < 3; ii++){
                             std::swap(vertexTrans[1][ii], vertexTrans[2][ii]);
                        }
                    }

                    cFacet aFacet;
                    aFacet.normal.x = normalTrans[0];
                    aFacet.normal.y = normalTrans[1];
                    aFacet.normal.z = normalTrans[2];


                    for (int jj = 0; jj < 3; jj++){
                        aFacet.vertex[jj].x = vertexTrans[jj][0];
                        aFacet.vertex[jj].y = vertexTrans[jj][1];
                        aFacet.vertex[jj].z = vertexTrans[jj][2];
                    }

                    facetArr.push_back(aFacet);

                    faceCount ++;
            }
			SUMeshHelperRelease(&faceMesh);
		}
	}
}

void getEntities(SUEntitiesRef entities, SUTransformation transforMation){
    countFaces(entities, transforMation);
	getGroupEntity(entities, transforMation);
	getComponentEntity(entities, transforMation);
}

/*
    从group中获取entites并且递归处理
*/

void getGroupEntity(SUEntitiesRef entities, SUTransformation transforMation){
	size_t groupLen;
	SUEntitiesGetNumGroups(entities, &groupLen);
	if (groupLen > 0){
		std::vector<SUGroupRef> groupArr(groupLen);
		SUEntitiesGetGroups(entities, groupLen, &groupArr[0], &groupLen);
		for (size_t i = 0; i < groupLen; i++){
			SUEntitiesRef entitiesInGroup;
			SUGroupGetEntities(groupArr[i], &entitiesInGroup);

            SUTransformation transforMation2;
            SUTransformation resTransforMation;
            memset(resTransforMation.values, 0, sizeof(resTransforMation.values));

			SUGroupGetTransform(groupArr[i], &transforMation2);

			for (int kk = 0; kk < 4; kk++)
                for (int ii = 0; ii < 4; ii++)
                    for (int jj = 0; jj < 4; jj++)
                        resTransforMation.values[pos(ii, jj)] +=  transforMation.values[pos(ii, kk)]* transforMation2.values[pos(kk, jj)];

			getEntities(entitiesInGroup, resTransforMation);
		}
	}
}

/*
    从component中获取entites并递归处理
*/

void getComponentEntity(SUEntitiesRef entities, SUTransformation transforMation){
    size_t componentInstanceLen;
    SUEntitiesGetNumInstances(entities, &componentInstanceLen);
	if (componentInstanceLen > 0){
		std::vector<SUComponentInstanceRef> componentInstanceArr(componentInstanceLen);
		SUEntitiesGetInstances(entities, componentInstanceLen, &componentInstanceArr[0], &componentInstanceLen);
		for (size_t i = 0; i < componentInstanceLen; i++){

			/*
				从component的Instance中获取definition
			*/
			SUComponentDefinitionRef definitionOfInstance;
			SUComponentInstanceGetDefinition(componentInstanceArr[i], &definitionOfInstance);
			/*
				从componentdefinition中获取entities
			*/
			SUEntitiesRef entitiesInDefinition;
			SUComponentDefinitionGetEntities(definitionOfInstance,  &entitiesInDefinition);


            SUTransformation transforMation2;
            SUTransformation resTransforMation;
            memset(resTransforMation.values, 0, sizeof(resTransforMation.values));

			SUComponentInstanceGetTransform(componentInstanceArr[i], &transforMation2);

			for (int kk = 0; kk < 4; kk++)
                for (int ii = 0; ii < 4; ii++)
                    for (int jj = 0; jj < 4; jj++)
                        resTransforMation.values[pos(ii, jj)] +=  transforMation.values[pos(ii, kk)]* transforMation2.values[pos(kk, jj)];


			getEntities(entitiesInDefinition, resTransforMation);
		}
	}
}

/*
进行一些简单的初始化

*/
void init(){
    triangleCount = faceCount = entitiesCount = 0;
    for (size_t i = 0; i < 16; i++) matrixI.values[i] = 0;
    for (size_t i = 0; i < 4; i++) matrixI.values[ pos(i,i) ] = 1;
}
/*
	转换函数
	para:
		argv[1] : skp的文件名
		argv[2] : stl的文件名
        argv[3] : stl不包括后缀的文件名
        argv[4] : 是否自动检测
    返回值:
        0:成功
        3:打开skp文件出错
        4:写入stl文件出错
*/
int main(int argc, char* argv[])
{
  //  freopen("1.in","w",stdout);
    init();
	printf("start executing.....\n");
    SUInitialize();
    SUModelRef model = SU_INVALID;
    skpFileName = argv[1];
    printf("%s\n", skpFileName);
    SUResult res = SUModelCreateFromFile(&model, skpFileName);
    if (res != SU_ERROR_NONE) return 3;
    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);
    getEntities(entities,  matrixI);

    if (argc == 5){
        printf("start checking...\n");
        for (int i = 0; i < facetArr.size(); i++){
            Vector3 orig;
            orig = facetArr[i].Middle();
            Vector3 dir;
            dir.x = -facetArr[i].normal.x;
            dir.y = -facetArr[i].normal.y;
            dir.z = -facetArr[i].normal.z;
            bool flag = true;
            for (int j = 0; j < facetArr.size(); j++) if (i != j) {
                if (IntersectTriangle(orig, dir, facetArr[j].vertex[0], facetArr[j].vertex[1], facetArr[j].vertex[2])){
                        flag = false;
               //         printf("%d ===X===%d\n", i, j);
                        break;
                }
            }
            if (flag){
                    facetArr[i].normal.x = -facetArr[i].normal.x;
                    facetArr[i].normal.y = -facetArr[i].normal.y;
                    facetArr[i].normal.z = -facetArr[i].normal.z;
                    swap(facetArr[i].vertex[1], facetArr[i].vertex[2]);
            }
        }
    }

    stlFileName = argv[2];
    fp = fopen(stlFileName , "w");

    if (fp == NULL) return 4;

    fprintf(fp, "solid %s\n", argv[3]);

    for (int i = 0; i < facetArr.size(); i++){
        fprintf(fp, "facet normal %lf %lf %lf\nouter loop\n", facetArr[i].normal.x , facetArr[i].normal.y , facetArr[i].normal.z);
        for (int j = 0; j < 3; j++)
            fprintf(fp, "vertex %lf %lf %lf\n", facetArr[i].vertex[j].x, facetArr[i].vertex[j].y, facetArr[i].vertex[j].z);
        fprintf(fp, "endloop\nendfacet\n");
    }
    fprintf(fp, "endsolid %s\n", argv[3]);
    fclose(fp);
    SUModelRelease(&model);
    SUTerminate();
    return 0;
}
