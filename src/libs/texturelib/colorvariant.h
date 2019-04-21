#ifndef COLORVARIANT_H
#define COLORVARIANT_H

#include <TextureLib/Rgba128>
#include <TextureLib/Rgba32Signed>
#include <TextureLib/Rgba64Float>
#include <TextureLib/Rgba64Signed>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

#include <variant>

namespace Private {

using ColorVariantData = std::variant<
    std::monostate,
    Rgba32Signed,
    QRgb,
    Rgba64Float,
    Rgba64Signed,
    QRgba64,
    Rgba128Float,
    Rgba128Signed,
    Rgba128
>;

template<typename Color>
constexpr Color convertColorVariant(const ColorVariantData &d) noexcept
{
    auto result = Color();
    auto visitor = [&result](auto&& value)
    {
        using T = std::decay_t<decltype(value)>;
        if constexpr(std::is_same_v<T, std::monostate>) {
            result = Color();
        } else if constexpr(std::is_same_v<T, Color>) {
            result = value;
        } else {
            result = Private::convertRgba<Color, T>(value);
        }
    };
    std::visit(visitor, d);
    return result;
}

} // namespace Private

class ColorVariant
{
public:
    using Data = Private::ColorVariantData;

    constexpr ColorVariant() noexcept = default;
    template<typename T, std::enable_if_t<!std::is_same_v<T, ColorVariant>, int> = 0>
    explicit constexpr ColorVariant(T &&value) noexcept : d(std::forward(value)) {}
    constexpr ColorVariant(const ColorVariant &) = default;
    constexpr ColorVariant(ColorVariant &&) = default;
    ~ColorVariant() = default;

    ColorVariant &operator=(const ColorVariant &) = default;
    ColorVariant &operator=(ColorVariant &&) = default;

    constexpr const Data& data() const noexcept { return d; }

    constexpr bool isEmpty() const noexcept { return d.index() == 0; }

    constexpr Rgba32Signed toRgba32Signed() const noexcept
    { return Private::convertColorVariant<Rgba32Signed>(data()); }

    constexpr QRgb toRgba32Unsigned() const noexcept
    { return Private::convertColorVariant<QRgb>(data()); }

    constexpr Rgba64Float toRgba64Float() const noexcept
    { return Private::convertColorVariant<Rgba64Float>(data()); }

    constexpr Rgba64Signed toRgba64Signed() const noexcept
    { return Private::convertColorVariant<Rgba64Signed>(data()); }

    constexpr QRgba64 toRgba64Unsigned() const noexcept
    { return Private::convertColorVariant<QRgba64>(data()); }

    constexpr Rgba128Float toRgba128Float() const noexcept
    { return Private::convertColorVariant<Rgba128Float>(data()); }

    constexpr Rgba128Signed toRgba128Signed() const noexcept
    { return Private::convertColorVariant<Rgba128Signed>(data()); }

    constexpr Rgba128 toRgba128Unsigned() const noexcept
    { return Private::convertColorVariant<Rgba128>(data()); }

private:
    Data d;
};

// helpers

constexpr inline Rgba32Signed rgba32Signed(const ColorVariant &color) noexcept { return color.toRgba32Signed(); }
constexpr inline QRgb qRgba(const ColorVariant &color) noexcept { return color.toRgba32Unsigned(); }
constexpr inline Rgba64Float rgba64Float(const ColorVariant &color) noexcept { return color.toRgba64Float(); }
constexpr inline Rgba64Signed rgba64Signed(const ColorVariant &color) noexcept { return color.toRgba64Signed(); }
constexpr inline QRgba64 qRgba64(const ColorVariant &color) noexcept { return color.toRgba64Unsigned(); }
constexpr inline Rgba128Float rgba128Float(const ColorVariant &color) noexcept { return color.toRgba128Float(); }

#endif // COLORVARIANT_H
