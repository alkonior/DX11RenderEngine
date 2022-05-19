// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "GPUResource.h"

using namespace GVM;

GpuResourceView::GpuResourceView(IResourceView* view, const ConstBufferViewDesc& desc):
view(view), type(EViewType::CB), cbViewDescription(desc)
{}

GpuResourceView::GpuResourceView(IResourceView* view, const DepthStencilViewDesc& desc):
view(view), type(EViewType::DB), dbViewDescription(desc)
{}

GpuResourceView::GpuResourceView(IResourceView* view, const RenderTargetViewDesc& desc):
view(view), type(EViewType::RTB), rtViewDescription(desc)
{}

GpuResourceView::GpuResourceView(IResourceView* view, const ShaderResourceViewDesc& desc):
view(view), type(EViewType::SRB), srViewDescription(desc)
{}

GpuResourceView::GpuResourceView(IResourceView* view, const VertexBufferViewDesc& desc):
view(view), type(EViewType::VB), vbViewDescription(desc)
{}

GpuResourceView::GpuResourceView(IResourceView* view, const IndexBufferViewDesc& desc):
view(view), type(EViewType::IB), ibViewDescription(desc)
{}


GpuResourceView::GpuResourceView(const GpuResourceView& copy) noexcept
: view(copy.view), type(copy.type) 
{
    switch (type)
    {
        case EViewType::CB :
            {
                this->cbViewDescription = copy.cbViewDescription;
                break;
            }   
        case EViewType::DB :
            {
                this->dbViewDescription = copy.dbViewDescription;
                break;
            }   
        case EViewType::IB :
            {
                this->ibViewDescription = copy.ibViewDescription;
                break;
            }   
        case EViewType::VB :
            {
                this->vbViewDescription = copy.vbViewDescription;
                break;
            }   
        case EViewType::RTB :
            {
                this->rtViewDescription = copy.rtViewDescription;
                break;
            }   
        case EViewType::SRB :
            {
                this->srViewDescription = copy.srViewDescription;
                break;
            }   
    }
}

GpuResourceView::GpuResourceView(const GpuResourceView&& copy) noexcept
{
    *this = std::ref(copy);
}
