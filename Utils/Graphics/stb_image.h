// stb_image.h - Librería simple para cargar imágenes
// Esta es una versión simplificada. En un proyecto real, descargarías la versión completa de:
// https://github.com/nothings/stb/blob/master/stb_image.h

#ifndef STB_IMAGE_H
#define STB_IMAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

    // Para este ejemplo, usaremos una implementación simple
    // En un proyecto real, incluirías la librería completa STB

    typedef unsigned char stbi_uc;

    // Funciones principales
    stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
    void stbi_image_free(void *retval_from_stbi_load);

#ifdef __cplusplus
}
#endif

#endif // STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

// Implementación muy básica - solo para compilar
// En proyecto real usarías STB completo
stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels)
{
    // Por ahora retorna nullptr - necesitamos STB real
    *x = 32;
    *y = 32;
    *channels_in_file = 4;
    return nullptr;
}

void stbi_image_free(void *retval_from_stbi_load)
{
    if (retval_from_stbi_load)
    {
        free(retval_from_stbi_load);
    }
}

#endif
