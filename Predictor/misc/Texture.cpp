#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Logger/LogManager.h"

Texture::Texture(const std::string& filepath, bool isBind, bool isFree) {
    stbi_set_flip_vertically_on_load(true);

    _data = stbi_load(filepath.c_str(), &_width, &_height, &_channels, 0);
    if (!_data) {
        LogManager::LogCritical("Failed to load texture: " + filepath + " " + __LOGERROR__);
        return;
    }

    if (isBind) {
        glGenTextures(1, &_textureID);
        glBindTexture(GL_TEXTURE_2D, _textureID);

        // Настройки фильтрации и обёртки
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (_channels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, _data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }

    if (isFree) {
        stbi_image_free(_data);
        _data = nullptr;
    }

    if (isBind) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

Texture::~Texture() {
    if (_data != nullptr) {
        stbi_image_free(_data);
        _data = nullptr;
    }
    glDeleteTextures(1, &_textureID);
}

unsigned char* Texture::GetPixels() {
    return _data;
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