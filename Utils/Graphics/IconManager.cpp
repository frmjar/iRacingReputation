#include "IconManager.h"
#include "../Logging/Logger.h"

// Use the full STB implementation
#define STB_IMAGE_IMPLEMENTATION
#include "../../External/stb_image.h"

IconManager::~IconManager()
{
    Shutdown();
}

bool IconManager::Initialize(ID3D11Device *device)
{
    if (!device)
    {
        Logger::Error("IconManager: Device is null");
        return false;
    }

    m_device = device;
    Logger::Info("IconManager initialized");
    return true;
}

void IconManager::Shutdown()
{
    for (auto &pair : m_icons)
    {
        if (pair.second.texture)
        {
            pair.second.texture->Release();
        }
    }
    m_icons.clear();
    m_device = nullptr;
}

bool IconManager::LoadIcon(const std::string &name, const std::string &filepath)
{
    IconTexture icon;

    if (LoadTextureFromFile(filepath, &icon.texture, &icon.width, &icon.height))
    {
        icon.loaded = true;
        m_icons[name] = icon;
        Logger::Info("Icon loaded: " + name + " (" + std::to_string(icon.width) + "x" + std::to_string(icon.height) + ")");
        return true;
    }

    Logger::Error("Failed to load icon: " + name + " from " + filepath);
    return false;
}

IconTexture *IconManager::GetIcon(const std::string &name)
{
    auto it = m_icons.find(name);
    if (it != m_icons.end() && it->second.loaded)
    {
        return &it->second;
    }
    return nullptr;
}

bool IconManager::LoadAllIcons()
{
    bool allLoaded = true;

    // Cargar iconos desde Assets/Icons/
    std::string iconPath = "Assets/Icons/";

    allLoaded &= LoadIcon("clean_driver", iconPath + "clean_driver.png");
    allLoaded &= LoadIcon("good_racer", iconPath + "good_racer.png");
    allLoaded &= LoadIcon("aggressive", iconPath + "aggressive.png");
    allLoaded &= LoadIcon("dirty_driver", iconPath + "dirty_driver.png");
    allLoaded &= LoadIcon("rammer", iconPath + "rammer.png");
    allLoaded &= LoadIcon("blocking", iconPath + "blocking.png");
    allLoaded &= LoadIcon("unsafe_rejoin", iconPath + "unsafe_rejoin.png");
    allLoaded &= LoadIcon("newbie", iconPath + "newbie.png");

    if (allLoaded)
    {
        Logger::Info("All icons loaded successfully");
    }
    else
    {
        Logger::Warning("Some icons failed to load, falling back to text");
    }

    return allLoaded;
}

bool IconManager::LoadTextureFromFile(const std::string &filename, ID3D11ShaderResourceView **out_srv, int *out_width, int *out_height)
{
    Logger::Info("Attempting to load texture: " + filename);

    // Verificar si el archivo existe
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        Logger::Error("File not found: " + filename);
        return false;
    }
    fclose(file);

    // Cargar imagen usando stb_image
    int width, height, channels;
    unsigned char *image_data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
    if (!image_data)
    {
        Logger::Error("STB failed to load image: " + filename + " - Error: " + std::string(stbi_failure_reason()));
        return false;
    }

    Logger::Info("Image loaded successfully: " + std::to_string(width) + "x" + std::to_string(height) + " channels: " + std::to_string(channels));

    // Crear textura D3D11
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D *texture = nullptr;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;

    HRESULT hr = m_device->CreateTexture2D(&desc, &subResource, &texture);
    stbi_image_free(image_data);

    if (FAILED(hr))
    {
        return false;
    }

    // Crear shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    hr = m_device->CreateShaderResourceView(texture, &srvDesc, out_srv);
    texture->Release();

    if (FAILED(hr))
    {
        return false;
    }

    *out_width = width;
    *out_height = height;
    return true;
}
