#include "ModelData.h"

ModelData::ModelData(std::vector<ModelVertex> verticies, std::vector<uint16_t> indexes, Renderer::PrimitiveType pt, size_t primitiveCount) :
	verticies(verticies), indexes(indexes), pt(pt), primitiveCount(primitiveCount)
{}
