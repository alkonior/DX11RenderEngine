#pragma once
#include"SimpleMath.h"

#define float4 DirectX::SimpleMath::Vector4
#define matrix DirectX::SimpleMath::Matrix
#define float3 DirectX::SimpleMath::Vector3
#define float2 DirectX::SimpleMath::Vector2

class Transform {
	matrix transform;

	Transform(matrix transform);
public:

	Transform();
	Transform(float3 position, float3 roitation, float3 scale);
	matrix GetTransform() const;
	matrix GetInverseTransform() const;
	Transform operator*(const Transform& rhs);
};

 