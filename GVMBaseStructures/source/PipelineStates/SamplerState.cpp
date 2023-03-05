#include "GVMBaseStructures/PipelineStates/SamplerState.h"

GVM::Compressed::SamplerStateDesc::SamplerStateDesc(): SamplerStateDesc(GVM::SamplerStateDesc())
{
}

GVM::Compressed::SamplerStateDesc::SamplerStateDesc(const SamplerStateDesc& copy):
    MipLODBias(copy.MipLODBias),
    MaxLOD(copy.MaxLOD),
    MinLOD(copy.MinLOD),
    Data(copy.Data)
{
}

GVM::Compressed::SamplerStateDesc& GVM::Compressed::SamplerStateDesc::operator=(const SamplerStateDesc& copy)
{
    MipLODBias = (copy.MipLODBias);
    MaxLOD = (copy.MaxLOD);
    MinLOD = (copy.MinLOD);
    Data = (copy.Data);
    return *this;
}

bool GVM::Compressed::SamplerStateDesc::operator==(const SamplerStateDesc& copy) const
{
    return  MipLODBias == (copy.MipLODBias) &&
            MaxLOD == (copy.MaxLOD) &&
            MinLOD == (copy.MinLOD) &&
            Data == (copy.Data);
}

GVM::Compressed::SamplerStateDesc::SamplerStateDesc(const SamplerStateDesc&& copy) noexcept:
    MipLODBias(copy.MipLODBias),
    MaxLOD(copy.MaxLOD),
    MinLOD(copy.MinLOD),
    Data(copy.Data)
{
}

GVM::Compressed::SamplerStateDesc::SamplerStateDesc(const GVM::SamplerStateDesc& desc)
{
    this->MaxLOD = desc.MaxLOD;
    this->MinLOD = desc.MinLOD;
    this->MipLODBias = desc.MipLODBias;


    this->Fields.BorderColor[0] = static_cast<uint8_t>(std::lroundf(255.f * desc.BorderColor[0]));
    this->Fields.BorderColor[1] = static_cast<uint8_t>(std::lroundf(255.f * desc.BorderColor[1]));
    this->Fields.BorderColor[2] = static_cast<uint8_t>(std::lroundf(255.f * desc.BorderColor[2]));
    this->Fields.BorderColor[3] = static_cast<uint8_t>(std::lroundf(255.f * desc.BorderColor[3]));

    //this->t.ColorArray =  r  | (g << 8) | (b << 16) | (b << 24);

    this->Fields.Filter = to_underlying(desc.Filter);
    this->Fields.AddressU = to_underlying(desc.AddressU);
    this->Fields.AddressV = to_underlying(desc.AddressV);
    this->Fields.AddressW = to_underlying(desc.AddressW);
    this->Fields.ComparisonFunc = to_underlying(desc.ComparisonFunc);

    this->Fields.MaxAnisotropy = desc.MaxAnisotropy;
}

GVM::SamplerStateDesc::SamplerStateDesc(const CompressedType& desc)
{
    this->Filter = to_enum<ESamplerFilter>(desc.Fields.Filter);
    this->AddressU = to_enum<ETextureAddressMode>(desc.Fields.AddressU);
    this->AddressV = to_enum<ETextureAddressMode>(desc.Fields.AddressV);
    this->AddressW = to_enum<ETextureAddressMode>(desc.Fields.AddressW);
    this->MipLODBias = desc.MipLODBias;
    this->MaxAnisotropy = desc.Fields.MaxAnisotropy;
    this->ComparisonFunc = to_enum<EComparisonFunc>(desc.Fields.ComparisonFunc);

    this->BorderColor[0] = desc.Fields.BorderColor[0] * 1.0f / 255.f;
    this->BorderColor[1] = desc.Fields.BorderColor[1] * 1.0f / 255.f;
    this->BorderColor[2] = desc.Fields.BorderColor[2] * 1.0f / 255.f;
    this->BorderColor[3] = desc.Fields.BorderColor[3] * 1.0f / 255.f;

    this->MinLOD = desc.MinLOD;
    this->MaxLOD = desc.MaxLOD;
}
