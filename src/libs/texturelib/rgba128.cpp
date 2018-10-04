#include "rgba128.h"

static_assert(sizeof(float) == 4, "Sizeof float != 4");
static_assert(sizeof(Rgba128Float) == 16, "Sizeof Rgba128Float != 16");
static_assert(sizeof(Rgba128Signed) == 16, "Sizeof Rgba128Signed != 16");
static_assert(sizeof(Rgba128) == 16, "Sizeof Rgba128 != 16");
