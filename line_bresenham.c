#include "svpng.inc"
#include <math.h>   // fmaxf(), cosf(), sinf()
#include <stdlib.h> // abs()
#include <string.h> // memset()
#define PI 3.14159265359f

static const int w = 512, h = 512;
static unsigned char img[w * h * 3];

void setpixel(int x, int y) {
    unsigned char* p = img + (y * w + x) * 3;
    p[0] = p[1] = p[2] = 0;
}

// Modified from  https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void bresenham(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    while (setpixel(x0, y0), x0 != x1 || y0 != y1) {
        int e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
}

int main() {
    memset(img, 255, sizeof(img));
    float cx = w * 0.5f - 0.5f, cy = h * 0.5f - 0.5f;
    for (int j = 0; j < 5; j++) {
        float r1 = fmaxf(w, h) * (j + 0.5f) * 0.085f;
        float r2 = fmaxf(w, h) * (j + 1.5f) * 0.085f;
        float t = j * PI / 64.0f;
        for (int i = 1; i <= 64; i++, t += 2.0f * PI / 64.0f) {
            float ct = cosf(t), st = sinf(t);
            bresenham((int)(cx + r1 * ct), (int)(cy - r1 * st), (int)(cx + r2 * ct), (int)(cy - r2 * st));
        }
    }
    svpng(fopen("line_bresenham.png", "w"), 512, 512, img, 0);
}
