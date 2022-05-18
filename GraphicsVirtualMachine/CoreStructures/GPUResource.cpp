#include "GPUResource.h"

GVM::GpuResourceView::GpuResourceView(IResourceView* view, const ConstBufferViewDesc& desc):
view(view), type(EViewType::CB), cbViewDescription(desc)
{}

GVM::GpuResourceView::GpuResourceView(IResourceView* view, const DepthStencilViewDesc& desc):
view(view), type(EViewType::DB), dsViewDescription(desc)
{}

GVM::GpuResourceView::GpuResourceView(IResourceView* view, const RenderTargetViewDesc& desc):
view(view), type(EViewType::RTB), rtViewDescription(desc)
{}

GVM::GpuResourceView::GpuResourceView(IResourceView* view, const ShaderResourceViewDesc& desc):
view(view), type(EViewType::SRB), shViewDescription(desc)
{}

GVM::GpuResourceView::GpuResourceView(IResourceView* view, const VertexBufferViewDesc& desc):
view(view), type(EViewType::VB), vViewDescription(desc)
{}

GVM::GpuResourceView::GpuResourceView(IResourceView* view, const IndexBufferViewDesc& desc):
view(view), type(EViewType::IB), iViewDescription(desc)
{}
