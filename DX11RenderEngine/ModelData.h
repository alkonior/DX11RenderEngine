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
	std::vector<FramedModelCommon> verticies;
	std::vector<Frame> frames;
	std::vector<uint16_t> indexes;
	Renderer::PrimitiveType pt;
	size_t primitiveCount;

	FramedModelData();
	FramedModelData(std::vector<FramedModelCommon> verticies, std::vector<Frame> frames, std::vector<uint16_t> indexes, Renderer::PrimitiveType pt, size_t primitiveCount);


	void AddTriangle(const std::vector<Frame>& verticies3, const FramedModelCommon uvs[3]);

};

