#include "DriverTagWindow.h"

bool DriverTagWindow::IsPlaceholderDriver(const DriverData &d) const
{
    return d.displayName.find("Piloto #") == 0 || d.displayName.empty() || !d.isValid;
}

std::string DriverTagWindow::TruncateText(const std::string &text, float maxWidth) const
{
    if (text.empty())
        return text;
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    if (textSize.x <= maxWidth)
        return text;
    std::string truncated = text;
    while (!truncated.empty() && ImGui::CalcTextSize((truncated + "...").c_str()).x > maxWidth)
    {
        truncated.pop_back();
    }
    return truncated.empty() ? "..." : truncated + "...";
}

bool DriverTagWindow::CreateFallbackIcon()
{
    if (m_fallbackIcon)
        return true;
    // Crear una textura 32x32 gris con un "?" simple dibujable (usaremos color sólido, el texto se pinta encima si quisiéramos)
    const int W = 32, H = 32;
    std::vector<unsigned int> pixels(W * H, 0xFF555555); // ARGB (DX11 usará RGBA reinterpretar)
    // Dibujar borde claro
    for (int x = 0; x < W; ++x)
    {
        pixels[x] = 0xFFAAAAAA;
        pixels[x + (H - 1) * W] = 0xFFAAAAAA;
    }
    for (int y = 0; y < H; ++y)
    {
        pixels[y * W] = 0xFFAAAAAA;
        pixels[(y * W) + (W - 1)] = 0xFFAAAAAA;
    }
    // Crear textura
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = W;
    desc.Height = H;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = pixels.data();
    init.SysMemPitch = W * 4;
    ID3D11Texture2D *tex = nullptr;
    if (FAILED(m_device->CreateTexture2D(&desc, &init, &tex)))
        return false;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    if (FAILED(m_device->CreateShaderResourceView(tex, &srvDesc, &m_fallbackIcon)))
    {
        tex->Release();
        return false;
    }
    tex->Release();
    return true;
}

void DriverTagWindow::UpdateTrustLevel(DriverReputation &reputation) const
{
    int positiveCount = 0;
    int negativeCount = 0;
    if (reputation.HasBehavior(DriverFlags::CLEAN_DRIVER))
        positiveCount += 3;
    if (reputation.HasBehavior(DriverFlags::GOOD_RACER))
        positiveCount += 4;
    if (reputation.HasBehavior(DriverFlags::AGGRESSIVE))
        positiveCount += 1;
    if (reputation.HasBehavior(DriverFlags::DIRTY_DRIVER))
        negativeCount += 3;
    if (reputation.HasBehavior(DriverFlags::RAMMER))
        negativeCount += 5;
    if (reputation.HasBehavior(DriverFlags::BLOCKING))
        negativeCount += 2;
    if (reputation.HasBehavior(DriverFlags::UNSAFE_REJOIN))
        negativeCount += 2;
    int netScore = positiveCount - negativeCount;
    if (netScore >= 4)
        reputation.trustLevel = DriverTrustLevel::TRUSTED;
    else if (netScore >= 1)
        reputation.trustLevel = DriverTrustLevel::NEUTRAL;
    else if (netScore <= -3)
        reputation.trustLevel = DriverTrustLevel::AVOID;
    else if (netScore <= -1)
        reputation.trustLevel = DriverTrustLevel::CAUTION;
    else
        reputation.trustLevel = DriverTrustLevel::NEUTRAL;
}

int DriverTagWindow::CountDriversWithFlags() const
{
    int count = 0;
    for (const auto &kv : m_driverReputations)
    {
        if (kv.second.behaviorFlags != static_cast<uint32_t>(DriverFlags::UNKNOWN) && kv.second.behaviorFlags != 0)
            count++;
    }
    return count;
}
