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
声明获取group中的entities函数
*/
void getGroupEntity(SUEntitiesRef entities, SUTransformation transforMation);

/*
声明获取component instance中的entities函数
*/
void getComponentEntity(SUEntitiesRef entities, SUTransformation transforMation);

/*
统计每个独立的entities的faces的个数
*/
void countFaces(SUEntitiesRef entities, SUTransformation transforMation);

/*
递归从每个entities集合中找出entities
*/
void calcEntities(SUEntitiesRef entities, SUTransformation transforMation);
