#pragma once

#define _USE_MATH_DEFINES
#include <cmath> // IWYU pragma: keep

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#define EPSILON (std::numeric_limits<float>::epsilon())
#define SQRT_EPSILON (0.000345)
