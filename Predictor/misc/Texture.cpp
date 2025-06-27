#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

Texture::Texture(const std::string& filepath) {
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filepath.c_str(), &_width, &_height, &_channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return;
    }

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    // Настройки фильтрации и обёртки
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    if (_channels == 4) format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    glDeleteTextures(1, &_textureID);
}

void Texture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _textureID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::GetWidth() const  { return _width; }
int Texture::GetHeight() const { return _height; }

int Texture::GetTextureID() const {
    return _textureID;
}