// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "GPUResource.h"

using namespace GVM;

GpuResourceView::GpuResourceView(ResourceView* id,const ConstBufferViewDesc& desc):
id(id), type(EViewType::CB), cbViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id,const DepthStencilViewDesc& desc):
id(id), type(EViewType::DB), dbViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id,const RenderTargetViewDesc& desc):
id(id), type(EViewType::RT), rtViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id,const UATargetViewDesc& desc):
id(id), type(EViewType::UA), uaViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id,const ShaderResourceViewDesc& desc):
id(id), type(EViewType::SR), srViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id,const VertexBufferViewDesc& desc):
id(id), type(EViewType::VB), vbViewDescription(desc)
{
}

GpuResourceView::GpuResourceView(ResourceView* id,const IndexBufferViewDesc& desc):
id(id), type(EViewType::IB), ibViewDescription(desc)
{
}


GpuResourceView::GpuResourceView(const GpuResourceView& copy) noexcept
:id(copy.id), view(copy.view), type(copy.type)
{
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
    }
}

GpuResourceView::GpuResourceView(const GpuResourceView&& copy) noexcept
:id(copy.id), view(copy.view), type(copy.type)
{
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
    }
}

GpuResource::GpuResource(Resource* id, const ResourceDesc& desc):
id(id), resourceDescription(desc)
{}
