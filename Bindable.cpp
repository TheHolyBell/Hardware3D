#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(Graphics& gfx) noexcept
{
    return gfx.m_pImmediateContext.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& gfx) noexcept
{
    return gfx.m_pDevice.Get();
}

DxgiInfoManager& Bindable::GetInfoManager(Graphics& gfx) noexcept(!IS_DEBUG)
{
#ifndef NDEBUG
    return gfx.infoManager;
#else
    throw std::logic_error("You fucked up man! (Tried to access gfx.infoManager in Release config)");
#endif
}
