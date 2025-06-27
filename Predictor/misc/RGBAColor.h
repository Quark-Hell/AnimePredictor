#ifndef ANIMEPREDICTOR_RGBACOLOR_H
#define ANIMEPREDICTOR_RGBACOLOR_H


class RGBAColor {
private:
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

public:
    constexpr RGBAColor(float red, float green, float blue, float alpha)
            : r(red), g(green), b(blue) {}

public:
    [[nodiscard]] float GetR() const;
    [[nodiscard]] float GetG() const;
    [[nodiscard]] float GetB() const;
    [[nodiscard]] float GetA() const;

    void SetR(float value);
    void SetG(float value);
    void SetB(float value);
    void SetA(float value);

    void Set(float red, float green, float blue, float alpha = 1.0f);

    static constexpr RGBAColor Red()     { return {1.0f, 0.0f, 0.0f, 1.0f}; }
    static constexpr RGBAColor Green()   { return {0.0f, 1.0f, 0.0f, 1.0f}; }
    static constexpr RGBAColor Blue()    { return {0.0f, 0.0f, 1.0f, 1.0f}; }
    static constexpr RGBAColor White()   { return {1.0f, 1.0f, 1.0f, 1.0f}; }
    static constexpr RGBAColor Black()   { return {0.0f, 0.0f, 0.0f, 1.0f}; }
    static constexpr RGBAColor Gray()    { return {0.5f, 0.5f, 0.5f, 1.0f}; }
    static constexpr RGBAColor Yellow()  { return {1.0f, 1.0f, 0.0f, 1.0f}; }
    static constexpr RGBAColor Cyan()    { return {0.0f, 1.0f, 1.0f, 1.0f}; }
    static constexpr RGBAColor Magenta() { return {1.0f, 0.0f, 1.0f, 1.0f}; }
    static constexpr RGBAColor Orange()  { return {1.0f, 0.5f, 0.0f, 1.0f}; }
    static constexpr RGBAColor Purple()  { return {0.5f, 0.0f, 0.5f, 1.0f}; }

private:
    void NormalizeColor();
};


#endif //ANIMEPREDICTOR_RGBACOLOR_H
