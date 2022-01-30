#include "ModelData.h"

ModelData::ModelData(std::vector<ModelVertex> verticies, std::vector<uint16_t> indexes, Renderer::PrimitiveType pt, size_t primitiveCount) :
	verticies(verticies), indexes(indexes), pt(pt), primitiveCount(primitiveCount)
{}

FramedModelData::FramedModelData(std::vector<FramedModelVertex> verticies, std::vector<Frame> frames, std::vector<uint16_t> indexes, Renderer::PrimitiveType pt, size_t primitiveCount) :
	verticies(verticies), frames(frames), indexes(indexes), pt(pt), primitiveCount(primitiveCount)
{}