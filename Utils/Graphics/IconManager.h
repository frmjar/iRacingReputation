#pragma once

#include <d3d11.h>
#include <string>
#include <unordered_map>

// Estructura para almacenar información de texturas de iconos
struct IconTexture
{
    ID3D11ShaderResourceView *texture = nullptr;
    int width = 0;
    int height = 0;
    bool loaded = false;
};

class IconManager
{
private:
    ID3D11Device *m_device = nullptr;
    std::unordered_map<std::string, IconTexture> m_icons;

public:
    IconManager() = default;
    ~IconManager();

    bool Initialize(ID3D11Device *device);
    void Shutdown();

    bool LoadIcon(const std::string &name, const std::string &filepath);
    IconTexture *GetIcon(const std::string &name);

    // Métodos para cargar iconos específicos
    bool LoadAllIcons();

private:
    bool LoadTextureFromFile(const std::string &filename, ID3D11ShaderResourceView **out_srv, int *out_width, int *out_height);
};
