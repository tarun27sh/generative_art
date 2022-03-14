/*
 *
 */
#include <fstream>
#include <iostream>
#include <math.h>

#include "spdlog/spdlog.h"

enum class pixel_color_e {
    RED,
    GREEN,
    BLUE,
};

/*
 * Sine wave
 */
static bool is_point_on_sine_wave(int x, int y, int offset, float scale) {
    bool ret_val = false;

    if (abs((y - offset) - tan(x) * scale) < 5) {
        // if (abs((y-offset) - sin(x^2)*(x)) < 50) {
        ret_val = true;
    }
    return ret_val;
}

uint8_t sin_wave(int i, int j, int dimx, int dimy, int offset, float scale) {
    constexpr uint8_t defult_value = 50;
    if (is_point_on_sine_wave(i, j, offset, scale)) {
        spdlog::info("i={}, j={}", i, j);
        return 255;
    }
    return defult_value;
}

/*
 * Circle
 *  ( x - h )^2 + ( y - k )^2 = r^2,
 *  where ( h, k ) is the center and r is the radius.
 */
static bool is_point_on_circle(int x, int y, int radius) {
    bool ret_val = false;

    if (abs(x * x + y * y - radius * radius) < 500) {
        ret_val = true;
    }
    return ret_val;
}

uint8_t fat_circle(int x, int y, int dimx, int dimy) {
    // circle 1
    for (int i = 3; i < dimx / 2; i = i * 2) {
        if (is_point_on_circle(x, y, i)) {
            spdlog::info("i={}, j={}", x, y);
            return 255;
        }
    }

    return 0;
}

/*
 * Mandelbrot Set
 */
struct complex_t {
    float real;
    float imaginary;

    complex_t operator+(complex_t& other) {
        return complex_t{this->real + other.real, this->imaginary + other.imaginary};
    }
    complex_t operator*(complex_t& other) {
        auto real = (this->real * other.real) - (this->imaginary * other.imaginary);
        auto imaginary = (this->real * other.imaginary) + (this->imaginary* other.real);
        return complex_t{real, imaginary};
    }

    float magnitude() {
        return sqrt(this->real*this->real + this->imaginary*this->imaginary);
    }
};

/*
 * new_val = F(old_val)
 * where F = Z^2 + C
 *  all variables are complex nos
 *
 * A point is to be printed if resulting magnitude of F remains < 2 after 
 * N iterations.
 */
static uint8_t get_mandelbrot_set_pixel_value(int x, int y, int dimx, int dimy, complex_t z) {
    uint8_t pixel_value = 0;
    constexpr int max_iterations = 164;

    // normalize x,y to [-2,2]
    float fx = ((float)x/dimx)*4;
    float fy = ((float)y/dimy)*4;

    complex_t C{fx,fy};

    uint8_t nof_iterations = 0;
    for (int i=0; i<max_iterations && z.magnitude() < 2; ++i) {
        z = z*z + C;
        ++nof_iterations;
    }

    pixel_value = ((float)nof_iterations/max_iterations)*255;
    spdlog::info("{}: pixel_value={}", __func__, pixel_value);

    return pixel_value;
}

uint8_t mandelbrot_set(int i, int j, int dimx, int dimy, int offset) {
    return get_mandelbrot_set_pixel_value(i,j,dimx, dimy, complex_t{0,0});
}

int main() {
    /*
     *  0,dimy             dimx, dimy
     *   ┌──────────────────┐
     *   │                  │
     *   │                  │
     *   │                  │
     *   │                  │
     *   │        .(dimx/2, │
     *   │          dimy/2) │
     *   │                  │
     *   │                  │
     *   │                  │
     *   └──────────────────┘
     *  0,0                dimx, 0
     */
    constexpr auto dimx = 2000u, dimy = 2000u;
    spdlog::set_level(spdlog::level::warn);

    using namespace std;
    ofstream ofs("generated_image.ppm", ios_base::out | ios_base::binary);

    // add PPM header
    ofs << "P6" << endl << dimx << ' ' << dimy << endl << "255" << endl;

    auto prev_val = 2;
    // set pixel data
    for (auto j = 0u; j < dimy; ++j) {
        for (auto i = 0u; i < dimx; ++i) {
            auto red   = mandelbrot_set(i  - (dimx / 2), j - (dimy / 2), dimx, dimy, 10);
            // auto red   = fat_circle(i-(dimx/2),j-(dimy/2), dimx, dimy);
            //auto red = sin_wave(i - (dimx / 2), j - (dimy / 2), dimx, dimy, 10, j);
            auto green = 0; // get_mandelbrot_pixel_value(i+j);
            // auto green   = sin_wave(i-(dimx/2),j-(dimy/2), dimx, dimy, 24);
            //auto blue = sin_wave(i - (dimx / 2), j - (dimy / 2), dimx, dimy, 300, 300);
            auto blue  = 0; // get_mandelbrot_pixel_value(i+j);

            ofs << red << green << blue;
            prev_val = red;
        }
    }

    ofs.close();
    spdlog::info("Saving ppm image..");
    return EXIT_SUCCESS;
}
