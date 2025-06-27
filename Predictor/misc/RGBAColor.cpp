#include "RGBAColor.h"
#include <algorithm>

float RGBAColor::GetR() const { return r; }
float RGBAColor::GetG() const { return g; }
float RGBAColor::GetB() const { return b; }
float RGBAColor::GetA() const { return a; }

void RGBAColor::SetR(float value) { r = value; r = std::clamp(r, 0.0f, 1.0f); }
void RGBAColor::SetG(float value) { g = value; g = std::clamp(g, 0.0f, 1.0f); }
void RGBAColor::SetB(float value) { b = value; b = std::clamp(b, 0.0f, 1.0f); }
void RGBAColor::SetA(float value) { a = value; a = std::clamp(a, 0.0f, 1.0f); }

void RGBAColor::Set(float red, float green, float blue, float alpha) {
    r = red;
    g = green;
    b = blue;
    a = alpha;

    RGBAColor::NormalizeColor();
}

void RGBAColor::NormalizeColor() {
    r = std::clamp(r, 0.0f, 1.0f);
    g = std::clamp(g, 0.0f, 1.0f);
    b = std::clamp(b, 0.0f, 1.0f);
    a = std::clamp(a, 0.0f, 1.0f);
}