#include "svpng.inc"
#include <math.h>   // ceilf(), floorf(), fminf(), fmaxf(), sinf(), cosf(), sqrtf()
#include <string.h> // memset()
#define PI 3.14159265359f

static const int w = 1024, h = 1024;
static unsigned char img[w * h * 3];

float capsuleSDF(float px, float py, float ax, float ay, float bx, float by, float r) {
    float pax = px - ax, pay = py - ay, bax = bx - ax, bay = by - ay;
    float h = fmaxf(fminf((pax * bax + pay * bay) / (bax * bax + bay * bay), 1.0f), 0.0f);
    float dx = pax - bax * h, dy = pay - bay * h;
    return sqrtf(dx * dx + dy * dy) - r;
}

void alphablend(int x, int y, float alpha, float r, float g, float b) {
    unsigned char* p = img + (y * w + x) * 3;
    p[0] = (unsigned char)(p[0] * (1 - alpha) + r * alpha * 255);
    p[1] = (unsigned char)(p[1] * (1 - alpha) + g * alpha * 255);
    p[2] = (unsigned char)(p[2] * (1 - alpha) + b * alpha * 255);
}

void lineSDFAABB(float ax, float ay, float bx, float by, float r) {
    int x0 = (int)floorf(fminf(ax, bx) - r);
    int x1 = (int) ceilf(fmaxf(ax, bx) + r);
    int y0 = (int)floorf(fminf(ay, by) - r);
    int y1 = (int) ceilf(fmaxf(ay, by) + r);
    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++)
            alphablend(x, y, fmaxf(fminf(0.5f - capsuleSDF(x, y, ax, ay, bx, by, r), 1.0f), 0.0f), 1.0f, 0.0f, 0.0f);
}

int main() {
    memset(img, 255, sizeof(img));
    float cx = w * 0.5f, cy = h * 0.5f;
    float r = fminf(w, h) * 0.45f;
#define S(a, b) lineSDFAABB(cx + r * cosf(a), cy - r * sinf(a), cx + r * cosf(b), cy - r * sinf(b), 0.5f)
    for (int i = 0; i <= 64; i++) {
        float t = i * (0.5f * PI / 64.0f);
        S(t, t - PI * 0.5f);
        S(t + PI, t + PI * 0.5f);
        S(t, t * 2.0f + PI * 0.5f);
        S(PI - t, PI * 0.5f - t * 2.0f);
        S(PI * 1.5f, PI * 1.5f - t);
        S(PI * 1.5f, PI * 1.5f + t);
    }
    svpng(fopen("stitchheart.png", "wb"), w, h, img, 0);
}
