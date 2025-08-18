#pragma once

// Interfaz base para todos los componentes de DriverTag
class IDriverTagComponent
{
public:
    virtual ~IDriverTagComponent() = default;
    virtual void Render() = 0;
    virtual bool Initialize() { return true; }
    virtual void Shutdown() {}
};
