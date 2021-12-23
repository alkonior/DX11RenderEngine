#include <cstdint>

namespace Rederer
{

enum FillMode {
	FILLMODE_SOLID,
	FILLMODE_WIREFRAME
};

enum CullMode {
	CULLMODE_NONE,
	CULLMODE_CULLCLOCKWISEFACE,
	CULLMODE_CULLCOUNTERCLOCKWISEFACE
};



struct RasterizerState {
	FillMode fillMode;
	CullMode cullMode;
	float depthBias;
	float slopeScaleDepthBias;
	uint8_t scissorTestEnable;
	uint8_t multiSampleAntiAlias;
};

};