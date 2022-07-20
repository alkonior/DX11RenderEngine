// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "GPUResource.h"

using namespace GVM;

GpuResourceView::GpuResourceView(ResourceView* id ,const ConstBufferViewDesc& desc):
id(id), resource(desc.Buffer), type(EViewType::CB), cbViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id ,const DepthStencilViewDesc& desc):
id(id), resource(desc.Resource), type(EViewType::DB), dbViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id ,const RenderTargetViewDesc& desc):
id(id), resource(desc.Resource), type(EViewType::RT), rtViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id ,const UATargetViewDesc& desc):
id(id), resource(desc.Resource), type(EViewType::UA), uaViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id ,const ShaderResourceViewDesc& desc):
id(id), resource(desc.Resource), type(EViewType::SR), srViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id ,const VertexBufferViewDesc& desc):
id(id), resource(desc.Buffer), type(EViewType::VB), vbViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id ,const IndexBufferViewDesc& desc):
id(id), resource(desc.Buffer), type(EViewType::IB), ibViewDescription(desc)
{
}


GpuResourceView::GpuResourceView(const GpuResourceView& copy) noexcept
:id(copy.id), resource(copy.resource), view(copy.view), type(copy.type)
{
    data[0] = copy.data[0];
    data[1] = copy.data[1];
    data[2] = copy.data[2];/*
    switch (type)
    {
    case EViewType::CB:
        {
            this->cbViewDescription = copy.cbViewDescription;
            break;
        }
    case EViewType::DB:
        {
            this->dbViewDescription = copy.dbViewDescription;
            break;
        }
    case EViewType::IB:
        {
            this->ibViewDescription = copy.ibViewDescription;
            break;
        }
    case EViewType::VB:
        {
            this->vbViewDescription = copy.vbViewDescription;
            break;
        }
    case EViewType::RT:
        {
            this->rtViewDescription = copy.rtViewDescription;
            break;
        }
    case EViewType::SR:
        {
            this->srViewDescription = copy.srViewDescription;
            break;
        }
    case EViewType::UA:
        {
            this->uaViewDescription = copy.uaViewDescription;
            break;
        }
    }*/
}

GpuResourceView::GpuResourceView(const GpuResourceView&& copy) noexcept
:id(copy.id), resource(copy.resource), view(copy.view), type(copy.type)
{
    data[0] = copy.data[0];
    data[1] = copy.data[1];
    data[2] = copy.data[2];/*
    switch (type)
    {
    case EViewType::CB:
        {
            this->cbViewDescription = copy.cbViewDescription;
            break;
        }
    case EViewType::DB:
        {
            this->dbViewDescription = copy.dbViewDescription;
            break;
        }
    case EViewType::IB:
        {
            this->ibViewDescription = copy.ibViewDescription;
            break;
        }
    case EViewType::VB:
        {
            this->vbViewDescription = copy.vbViewDescription;
            break;
        }
    case EViewType::RT:
        {
            this->rtViewDescription = copy.rtViewDescription;
            break;
        }
    case EViewType::SR:
        {
            this->srViewDescription = copy.srViewDescription;
            break;
        }
    case EViewType::UA:
        {
            this->uaViewDescription = copy.uaViewDescription;
            break;
        }
    }*/
}

GVM::GpuResource::GpuResource(Resource* id, const ResourceDesc& desc)
: resourceDescription(desc), id(id)
{
}
