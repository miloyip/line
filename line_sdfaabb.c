#include "svpng.inc"
#include <math.h>   // ceilf(), floorf(), fminf(), fmaxf(), sinf(), cosf(), sqrtf()
#include <string.h> // memset()
#define PI 3.14159265359f

static const int w = 512, h = 512;
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
            alphablend(x, y, fmaxf(fminf(0.5f - capsuleSDF(x, y, ax, ay, bx, by, r), 1.0f), 0.0f), 0.0f, 0.0f, 0.0f);
}

int main() {
    memset(img, 255, sizeof(img));
    float cx = w * 0.5f, cy = h * 0.5f;
    for (int j = 0; j < 5; j++) {
        float r1 = fmaxf(w, h) * (j + 0.5f) * 0.085f;
        float r2 = fmaxf(w, h) * (j + 1.5f) * 0.085f;
        float t = j * PI / 64.0f, r = (j + 1) * 0.5f;
        for (int i = 1; i <= 64; i++, t += 2.0f * PI / 64.0f) {
            float ct = cosf(t), st = sinf(t);
            lineSDFAABB(cx + r1 * ct, cy - r1 * st, cx + r2 * ct, cy - r2 * st, r);
        }
    }
    svpng(fopen("line_sdfaabb.png", "wb"), w, h, img, 0);
}
