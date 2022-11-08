#include "Lights.h"


#pragma region SpotLight
StaticMeshId SpotLight::Spot = 0;

void SpotLight::CheckAngles()
{
	if (phiPenumbra < thetaUmbra) phiPenumbra = thetaUmbra;

	float total = thetaUmbra + phiPenumbra;
	if (total >= M_PI)
	{
		thetaUmbra = (thetaUmbra / total) * (M_PI - 0.001);
		phiPenumbra = (phiPenumbra / total) * (M_PI - 0.001);
	}
}

void SpotLight::SetAngles(float inThetaUmbra, float inPhiPenumbra)
{
	thetaUmbra = inThetaUmbra;
	phiPenumbra = inPhiPenumbra;
	CheckAngles();
}

SpotLight::SpotLight(float3 position, float3 direction, float length,
						float umbra, float penumbra, float falloff, float intensity,
						color color) : thetaUmbra(0), phiPenumbra(0), LightBase()
{

	PosDir = position;

	SetAngles(umbra, penumbra);
	Falloff = falloff;
	Length = length;
	LightColor = color;
	Intensity = intensity;

	direction.Normalize(Direction);

	//if (Spot == nullptr) Spot =
	//	LoadFbxStaticMesh<VertexPositionColor>("./Content/Lights/spot0.fbx");
	//todo LoadStaticModel

	MeshRep = Spot;
}

matrix SpotLight::CalcTransformmatrix() const
{
	float3 forward;
	Direction.Normalize(forward);
	//TODO : Check correctness of this func
	float3 right = forward.Cross( float3::Up);
	if (right.Length() < 0.0001f)
	{
		right = forward.Cross(float3::Backward);
	}
	right.Normalize();

	float3 up = forward.Cross(right);

	auto m = matrix::Identity;

	m.Forward(right);
	m.Right(forward);
	m.Up(up);

	auto tanPhi = (float)std::tan(phiPenumbra);
	auto ret = matrix::CreateScale(1, Length * tanPhi, Length * tanPhi) * matrix::CreateScale(Length, 1, 1) * m * matrix
		::CreateTranslation(PosDir);

	return ret;
}

void SpotLight::DrawDebugData()
{
	if (isDrawDebug)
	{
		//todo
		//Game::Instance->DebugRender->DrawStaticMesh(
		//	*MeshRep,
		//	CalcTransformmatrix(),
		//	Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
}

LightData SpotLight::GetLightData()
{
	return LightData{
		Vector4(PosDir.x, PosDir.y, PosDir.z, 1.0f),
		Vector4(Direction.x, Direction.y, Direction.z, Intensity),
		Vector4(Length, thetaUmbra, phiPenumbra, Falloff),
		LightColor
	};
}

PixelFlagsLighting SpotLight::GetLightFlags()
{
	return PixelFlagsLighting::SpotLight;
}

matrix SpotLight::GetTransformmatrix()
{
	return CalcTransformmatrix();
}

#pragma endregion SpotLight


#pragma region PointLight

StaticMeshId PointLight::SphereId = 0;


PointLight::PointLight(float3 position, float radius, float attenRadius, float intensity,
                       Color color)
{
	PosDir = position;
	LightColor = color;
	Intensity = intensity;
	Radius = radius < attenRadius ? radius : attenRadius;
	AttenRadius = radius < attenRadius ? attenRadius : radius;

	//if (Sphere == nullptr) Sphere = LoadFbxStaticMesh<VertexPositionColor>("./Content/Lights/point0.fbx");

	MeshRep = SphereId;
}


void PointLight::DrawDebugData()
{
	if (isDrawDebug)
	{
		//Game::Instance->DebugRender->DrawSphere(Radius, Color(0.4f, 0.4f, 0.4f, 1.0f), matrix::CreateTranslation(PosDir));
		//Game.Instance.DebugRender.DrawSphere(AttenRadius, SharpDX.Color.YellowGreen, matrix.Translation(PosDir));
	}
}


LightData PointLight::GetLightData()
{
	return LightData
	{
		Vector4(PosDir.x, PosDir.y, PosDir.z, 1.0f),
		Vector4(PosDir.x, PosDir.y, PosDir.z, 1.0f),
		Vector4(Radius, AttenRadius, Intensity, 0),
		LightColor,
	};
}


void PointLight::Update(float dt)
{
}


matrix PointLight::GetTransformmatrix()
{
	float scale = 1.0f;
	return matrix::CreateScale(Radius * scale, Radius * scale, Radius * scale) * matrix::CreateTranslation(PosDir);
}


PixelFlagsLighting PointLight::GetLightFlags()
{
	return PixelFlagsLighting::PointLight;
}

#pragma endregion PointLight


#pragma region AmbientLight

AmbientLight::AmbientLight(float intensity, float specIntensity, Color color) : SpecIntensity(specIntensity)
{
	Intensity = intensity;
	LightColor = color;
}


void AmbientLight::DrawDebugData()
{
	// how?
}

LightData AmbientLight::GetLightData()
{
	return LightData
	{
		Vector4(0, 0, 0, 1.0f),
		Vector4(0, 0, 0, 1.0f),
		Vector4(Intensity, SpecIntensity, 0, 0),
		LightColor
	};
}

PixelFlagsLighting AmbientLight::GetLightFlags()
{
	return PixelFlagsLighting::AmbientLight;
}

matrix AmbientLight::GetTransformmatrix()
{
	return matrix::Identity;
}


#pragma endregion AmbientLight


#pragma region DirectionalLight

DirectionalLight::DirectionalLight(float3 direction, float intensity, Color color)
{

	direction.Normalize();
	PosDir = direction;
	LightColor = color;
	Intensity = intensity;

	PosDir.Normalize();
}

void DirectionalLight::DrawDebugData()
{
	//if (isDrawDebug) Game::Instance->DebugRender->DrawArrow(float3::Zero, float3::Zero + PosDir * 10,
	//                                                        Color(1.0f, 0.0f, 0.0f, 1.0f), float3::Up);
}

LightData DirectionalLight::GetLightData()
{
	return LightData
	{
		Vector4(PosDir.x, PosDir.y, PosDir.z, 0.0f),
		Vector4(PosDir.x, PosDir.y, PosDir.z, 0.0f),
		Vector4(Intensity, 0, 0, 0),
		LightColor
	};
}

PixelFlagsLighting DirectionalLight::GetLightFlags()
{
	return PixelFlagsLighting::DirectionalLight;
}

matrix DirectionalLight::GetTransformmatrix()
{
	return matrix::Identity;
}

UniversalLight::UniversalLight(const AmbientLight ambientLight):
LightType(LightTypes::Ambient),
ambientLight(ambientLight)
{
}

UniversalLight::UniversalLight(const DirectionalLight directionalLight):
LightType(LightTypes::Directional),
directionalLight(directionalLight)
{
}

UniversalLight::UniversalLight(const SpotLight spotLight):
LightType(LightTypes::Spot),
spotLight(spotLight)
{
}

UniversalLight::UniversalLight(const PointLight pointLight):
LightType(LightTypes::Point),
pointLight(pointLight)
{
}


#pragma endregion DirectionalLight

