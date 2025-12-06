#include "utils.h"
#include <cmath>

double angle_diff(double a, double b)
{
    const double pi = 3.14159265358979323846;
    double d = a - b;
    d = std::fmod(d + pi, 2.0 * pi);
    if (d < 0)
        d += 2.0 * pi;
    return d - pi;
}