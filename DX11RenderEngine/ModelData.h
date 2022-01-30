#pragma once
#include "ModelRendererUtils.h"
#include "Utils.h"


class ModelData {
public:
	std::vector<ModelVertex> verticies;
	std::vector<uint16_t> indexes;
	Renderer::PrimitiveType pt;
	size_t primitiveCount;
	ModelData(std::vector<ModelVertex> verticies, std::vector<uint16_t> indexes, Renderer::PrimitiveType pt, size_t primitiveCount);

};

class FramedModelData {
public:
	std::vector<FramedModelVertex> verticies;
	std::vector<Frame> frames;
	std::vector<uint16_t> indexes;
	Renderer::PrimitiveType pt;
	size_t primitiveCount;
	FramedModelData(std::vector<FramedModelVertex> verticies, std::vector<Frame> frames, std::vector<uint16_t> indexes, Renderer::PrimitiveType pt, size_t primitiveCount);

};

