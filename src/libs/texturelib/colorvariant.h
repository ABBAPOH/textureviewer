#ifndef COLORVARIANT_H
#define COLORVARIANT_H

#include <TextureLib/RgbaTypes>

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

    enum class Type {
        Invalid = 0,
        Rgba32Signed,
        Rgba32Unsigned,
        Rgba32 = Rgba32Unsigned,
        Rgba64Float,
        Rgba64Signed,
        Rgba64Unsigned,
        Rgba64 = Rgba64Unsigned,
        Rgba128Float,
        Rgba128Signed,
        Rgba128Unsigned,
        Rgba128 = Rgba128Unsigned,
    };

    constexpr ColorVariant() noexcept = default;
    template<typename T, std::enable_if_t<!std::is_same_v<T, ColorVariant>, int> = 0>
    explicit constexpr ColorVariant(T &&value) noexcept : d(std::forward(value)) {}
    constexpr ColorVariant(const ColorVariant &) = default;
    constexpr ColorVariant(ColorVariant &&) = default;
    ~ColorVariant() = default;

    ColorVariant &operator=(const ColorVariant &) = default;
    ColorVariant &operator=(ColorVariant &&) = default;

    constexpr const Data& data() const noexcept { return d; }

    constexpr Type type() const noexcept { return Type(d.index()); }
    constexpr bool isEmpty() const noexcept { return d.index() == 0; }

    template<typename T>
    constexpr T value(T defaultValue = T()) const noexcept
    {
        const auto ptr = std::get_if<T>(d);
        return ptr ? *ptr : defaultValue;
    }

    template<typename T>
    constexpr T convert() const noexcept { return Private::convertColorVariant<T>(data()); }

private:
    Data d;
};

constexpr bool operator==(const ColorVariant &lhs, const ColorVariant &rhs)
{ return lhs.data() == rhs.data(); }

constexpr bool operator!=(const ColorVariant &lhs, const ColorVariant &rhs)
{ return lhs.data() != rhs.data(); }

// helpers

constexpr inline Rgba32Signed rgba32Signed(const ColorVariant &color) noexcept
{ return color.convert<Rgba32Signed>(); }
constexpr inline QRgb qRgba(const ColorVariant &color) noexcept
{ return color.convert<QRgb>(); }
constexpr inline Rgba64Float rgba64Float(const ColorVariant &color) noexcept
{ return color.convert<Rgba64Float>(); }
constexpr inline Rgba64Signed rgba64Signed(const ColorVariant &color) noexcept
{ return color.convert<Rgba64Signed>(); }
constexpr inline QRgba64 qRgba64(const ColorVariant &color) noexcept
{ return color.convert<QRgba64>(); }
constexpr inline Rgba128Float rgba128Float(const ColorVariant &color) noexcept
{ return color.convert<Rgba128Float>(); }

#endif // COLORVARIANT_H
