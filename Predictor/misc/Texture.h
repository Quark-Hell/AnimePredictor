#ifndef ANIMEPREDICTOR_TEXTURE_H
#define ANIMEPREDICTOR_TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Texture {
public:
    explicit Texture(const std::string& filepath, bool isBind = true, bool isFree = true);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;

    [[nodiscard]] int GetTextureID() const;

    unsigned char* GetPixels();

private:
    GLuint _textureID = 0;
    int _width = 0;
    int _height = 0;
    int _channels = 0;

    unsigned char* _data = nullptr;
};


#endif //ANIMEPREDICTOR_TEXTURE_H
