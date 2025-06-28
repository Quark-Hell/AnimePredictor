#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Logger/LogManager.h"

Texture::Texture(const std::string& filepath, bool isBind) {
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

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::FreeBuffer() {
    if (_data != nullptr) {
        stbi_image_free(_data);
        _data = nullptr;
    }
}

void Texture::Flip() {
    int totalPixels = _width * _height;
    int halfPixels = totalPixels / 2;
    int pixelSize = _channels;

    for (int i = 0; i < halfPixels; ++i) {
        int oppositeIndex = totalPixels - 1 - i;

        for (int c = 0; c < _channels; ++c) {
            std::swap(
                    _data[i * pixelSize + c],
                    _data[oppositeIndex * pixelSize + c]
            );
        }
    }
}
void Texture::Mirror() {
    int rowSize = _width * _channels;
    for (int y = 0; y < _height; ++y) {
        unsigned char* row = _data + y * rowSize;
        for (int x = 0; x < _width / 2; ++x) {
            int leftIndex = x * _channels;
            int rightIndex = (_width - 1 - x) * _channels;
            for (int c = 0; c < _channels; ++c) {
                std::swap(row[leftIndex + c], row[rightIndex + c]);
            }
        }
    }
}

Texture::~Texture() {
    Texture::FreeBuffer();
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