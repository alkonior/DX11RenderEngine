#pragma once
#include <d3d.h>
#include <d3d11.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "TransformUtils.h"
#include "Utils/ModelMesh.h"
#include "Utils/PBRFlags.h"


enum class LightTypes
{
    Ambient = 0,
    Directional = 1,
    Point = 2,
    Spot = 3,
};


#pragma pack(push, 4)
struct LightData
{
    float4 Pos;
    float4 Dir;
    float4 Params;
    float4 Color;
};
#pragma pack(pop)


class LightBase
{
public:
    //StaticMeshId MeshRep = 0;
    DirectX::SimpleMath::Vector3 PosDir;
    float4 LightColor;
    float Intensity = 1.0f;
    bool isDrawDebug = false;
};


class SpotLight : public LightBase
{
protected:
    float thetaUmbra;
    float phiPenumbra;

public:
    static StaticMeshId Spot;

    float Falloff;
    float Length;
    DirectX::SimpleMath::Vector3 Direction;


    inline void SetThetaUmbra(float thetaUmbraRad)
    {
        thetaUmbra = thetaUmbraRad;
        CheckAngles();
    }

    inline float GetThetaUmbra() const { return thetaUmbra; } // [0, pi)
    inline void SetPhiPenumbra(float phiPenumbraRad)
    {
        phiPenumbra = phiPenumbraRad;
        CheckAngles();
    } // [theta, pi)
    inline float GetPhiPenumbra() const { return phiPenumbra; }


    void CheckAngles();
    void SetAngles(float inThetaUmbra, float inPhiPenumbra);


    SpotLight(float3 position, float3 direction, float length, float umbra,
              float penumbra, float falloff, float intensity, color color);

    [[nodiscard]] matrix CalcTransformMatrix() const;

    void DrawDebugData();
    LightData GetLightData() const;
    PixelFlagsLighting GetLightFlags();

    matrix GetTransformMatrix();
};


class PointLight : public LightBase
{
public:
    float Radius;
    float AttenRadius;

    static StaticMeshId SphereId;

    PointLight(DirectX::SimpleMath::Vector3 position, float radius, float attenRadius, float intensity,
               color color);

    void Update(float dt);
    void DrawDebugData();
    matrix GetTransformMatrix();
    LightData GetLightData() const;
    PixelFlagsLighting GetLightFlags();
};


class AmbientLight : public LightBase
{
public:
    float SpecIntensity;

    AmbientLight(float intensity, float specIntensity, Color color);

    void DrawDebugData();
    LightData GetLightData() const ;
    PixelFlagsLighting GetLightFlags();
    matrix GetTransformMatrix();
};


class DirectionalLight : public LightBase
{
public:
    float AmbientAmount;


    DirectionalLight(Vector3 direction, float intensity, Color color);

    void DrawDebugData();


    LightData GetLightData() const;

    PixelFlagsLighting GetLightFlags();

    matrix GetTransformMatrix();
};


struct UniversalLight
{

    UniversalLight(const AmbientLight& ambientLight);
    UniversalLight(const DirectionalLight& directionalLight);
    UniversalLight(const SpotLight& spotLight);
    UniversalLight(const PointLight& pointLight );

    union
    {
        AmbientLight ambientLight;
        DirectionalLight directionalLight;
        SpotLight spotLight;
        PointLight pointLight;
    };
private:
    LightTypes LightType;
    friend class LightRenderPass;
    LightData GetLightData() const;
};
