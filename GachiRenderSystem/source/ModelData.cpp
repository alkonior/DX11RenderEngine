#include "pch.h"
#include "Utils/ModelData.h"

ModelData::ModelData(std::vector<ModelVertex> verticies, std::vector<uint16_t> indexes, EPrimitiveType pt, size_t primitiveCount) :
	verticies(verticies), indexes(indexes), pt(pt), primitiveCount(primitiveCount)
{}

