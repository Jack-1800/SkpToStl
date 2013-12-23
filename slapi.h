#include <slapi/slapi.h>
#include <slapi/geometry.h>
#include <slapi/initialize.h>
#include <slapi/unicodestring.h>
#include <slapi/model/model.h>
#include <slapi/model/entities.h>
#include <slapi/model/face.h>
#include <slapi/model/edge.h>
#include <slapi/model/vertex.h>
#include <slapi/model/group.h>
#include <slapi/model/component_instance.h>
#include <slapi/model/component_definition.h>
#include <slapi/model/mesh_helper.h>
#include <slapi/geometry.h>
#include <slapi/transformation.h>

/*
������ȡgroup�е�entities����
*/
void getGroupEntity(SUEntitiesRef entities, SUTransformation transforMation);

/*
������ȡcomponent instance�е�entities����
*/
void getComponentEntity(SUEntitiesRef entities, SUTransformation transforMation);

/*
ͳ��ÿ��������entities��faces�ĸ���
*/
void countFaces(SUEntitiesRef entities, SUTransformation transforMation);

/*
�ݹ��ÿ��entities�������ҳ�entities
*/
void calcEntities(SUEntitiesRef entities, SUTransformation transforMation);
