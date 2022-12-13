#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>

#define WIDTH 1600
#define HEIGHT 800
#define abs(value) (value<0?value*-1:value)
#define round(value) (value<(int)value+0.5?(int)value:(int)value+1)
#define floor(value) ((int)value)

typedef struct Vector {
    double dx;
    double dy;
} Vector;

typedef struct Color {
    int r;
    int g;
    int b;
} Color;

typedef Color Canvas[HEIGHT][WIDTH];

static Canvas canvas;
int permutations[512];
int perm_size = sizeof(permutations)/sizeof(permutations[0]);

void saveAsPPM(const char *filepath)
{
    FILE *file = fopen(filepath, "wb");
    if(!file) exit(1);

    fprintf(file, "P6\n %i %i 255\n", WIDTH, HEIGHT);

    int y; for(y = 0; y < HEIGHT; ++y) {
        int x; for(x = 0; x < WIDTH; ++x) {
            Color pixel = canvas[y][x];
            uint8_t bytes[3] = {
                pixel.r,
                pixel.g,
                pixel.b
            };
            fwrite(bytes, sizeof(bytes), 1, file);
        }
    }
}

void fill_canvas(Color color)
{
    int y; for(y = 0; y < HEIGHT; ++y) {
        int x; for(x = 0; x < WIDTH; ++x) {
            canvas[y][x] = color;
        }
    }
}

double map(int number, int left_range, int right_range, int left_bound, int right_bound)
{
    return (number-left_range)/(double)(right_range-left_range) * (right_bound-left_bound) + left_bound;
}

// This part of code was copied from https://gist.github.com/nowl/828013

static int SEED = 0;

static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y)
{
    int tmp = hash[(y + SEED) % 256];
    return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y-x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3-2*s));
}

float noise2d(float x, float y)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int);
    int t = noise2(x_int+1, y_int);
    int u = noise2(x_int, y_int+1);
    int v = noise2(x_int+1, y_int+1);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

float perlin2D(float x, float y, float freq, int depth)
{
    float xa = x*freq;
    float ya = y*freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for(i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}

// Copied until here

int main(void)
{
    srand(369);
    fill_canvas((Color){18, 18, 18});
    int y; for(y = 0; y < HEIGHT; ++y) {
        int x; for(x = 0; x < WIDTH; ++x) {
            float noise = perlin2D(x, y, 0.01, 10);
            Color temp_color = round(noise) == 0?(Color){0, 255, 0}:(Color){0, 0, 255};
            canvas[y][x] = temp_color;
        }
    }
    saveAsPPM("canvas.ppm");
    return 0;
}
