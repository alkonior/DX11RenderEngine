#include "ModelData.h"
#include "ModelData.h"
#include "ModelData.h"

ModelData::ModelData(std::vector<ModelVertex> verticies, std::vector<uint16_t> indexes, Renderer::PrimitiveType pt, size_t primitiveCount) :
	verticies(verticies), indexes(indexes), pt(pt), primitiveCount(primitiveCount)
{}

FramedModelData::FramedModelData(): primitiveCount(0) {}

FramedModelData::FramedModelData(std::vector<FramedModelCommon> verticies, std::vector<Frame> frames, std::vector<uint16_t> indexes, Renderer::PrimitiveType pt, size_t primitiveCount) :
	verticies(verticies), frames(frames), indexes(indexes), pt(pt), primitiveCount(primitiveCount)
{}

void FramedModelData::AddTriangle(const std::vector<Frame>& verticies3, const FramedModelCommon uvs[3]) {
	if (frames.size() == 0) {
		frames.resize(verticies3.size());
		frames.shrink_to_fit();
	}

	int index = frames[0].size();
	for (size_t i = 0; i < frames.size(); i++) {
		frames[i].push_back(verticies3[i][0]);
		frames[i].push_back(verticies3[i][1]);
		frames[i].push_back(verticies3[i][2]);
	}

	verticies.push_back(uvs[0]);
	verticies.push_back(uvs[1]);
	verticies.push_back(uvs[2]);

	indexes.push_back(index);
	indexes.push_back(index+1);
	indexes.push_back(index+2);

	primitiveCount++;
}
