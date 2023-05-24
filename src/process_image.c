#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if (x >= im.w)
        x = im.w - 1;
    if (y >= im.h)
        y = im.h - 1;
    if (c >= im.c)
        c = im.c - 1;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (c < 0)
        c = 0;

    int index = x + im.w * y + im.h * im.w * c;

    return im.data[index];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    int index = x + im.w * y + im.h * im.w * c;

    if (x >= im.w)
        return;
    if (y >= im.h)
        return;
    if (c >= im.c)
        return;
    if (x < 0)
        return;
    if (y < 0)
        return;
    if (c < 0)
        return;

    im.data[index] = v;
    return;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    for (int x = 0; x < im.w; x++)
        for (int y = 0; y < im.h; y++)
            for (int c = 0; c < im.c; c++)
                set_pixel(copy, x, y, c, get_pixel(im, x, y, c));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for (int x = 0; x < im.w; x++)
    {
        for (int y = 0; y < im.h; y++)
        {
            float luma = 0.299 * get_pixel(im, x, y, 0) + 0.587 * get_pixel(im, x, y, 1) + 0.114 * get_pixel(im, x, y, 2);
            set_pixel(gray, x, y, 0, luma);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int x = 0; x < im.w; x++)
    {
        for (int y = 0; y < im.h; y++)
        {
            set_pixel(im, x, y, c, (get_pixel(im, x, y, c)) + v);
        }
    }
}

void clamp_image(image im)
{
    for (int x = 0; x < im.w; x++)
        for (int y = 0; y < im.h; y++)
            for (int c = 0; c < im.c; c++)
            {
                if (get_pixel(im, x, y, c) > 1)
                    set_pixel(im, x, y, c, 1);
                if (get_pixel(im, x, y, c) < 0)
                    set_pixel(im, x, y, c, 0);
            }
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

void rgb_to_hsv(image im)
{
    for (int x = 0; x < im.w; x++)
        for (int y = 0; y < im.h; y++)
        {
            float hue = 0.0, saturation, value;

            value = three_way_max(get_pixel(im, x, y, 0), get_pixel(im, x, y, 1), get_pixel(im, x, y, 2));   // value
            float m = three_way_min(get_pixel(im, x, y, 0), get_pixel(im, x, y, 1), get_pixel(im, x, y, 2)); // min
            float c = value - m;

            if (value <= 0)
                saturation = 0.0;
            else
                saturation = c / value;

            if (c > 0.0)
            {
                if (value == get_pixel(im, x, y, 0))
                    hue = ((get_pixel(im, x, y, 1) - get_pixel(im, x, y, 2)) / c);
                else if (value == get_pixel(im, x, y, 1))
                    hue = ((get_pixel(im, x, y, 2) - get_pixel(im, x, y, 0)) / c) + 2.0;
                else
                    hue = ((get_pixel(im, x, y, 0) - get_pixel(im, x, y, 1)) / c) + 4.0;
            }

            hue = (hue < 0.0) ? (hue / 6.0) + 1 : (hue / 6.0);

            set_pixel(im, x, y, 0, hue);
            set_pixel(im, x, y, 1, saturation);
            set_pixel(im, x, y, 2, value);
        }
}

void hsv_to_rgb(image im)
{
    for (int x = 0; x < im.w; x++)
        for (int y = 0; y < im.h; y++)
        {
            float h, s, v;
            h = get_pixel(im, x, y, 0);
            s = get_pixel(im, x, y, 1);
            v = get_pixel(im, x, y, 2);

            float r, g, b;

            float c = s * v;
            float max = v;
            float min = max - c;

            h = h * 6.0;

            if (v == 0)
            {
                r = 0;
                b = 0;
                g = 0;
            }
            else if (h < 1.0)
            {
                r = max;
                b = min;
                g = h * c + b;
            }
            else if (h < 2.0)
            {
                g = max;
                b = min;
                r = b - (h - 2) * c;
            }
            else if (h < 3.0)
            {
                g = max;
                r = min;
                b = (h - 2) * c + r;
            }
            else if (h < 4.0)
            {
                b = max;
                r = min;
                g = r - (h - 4) * c;
            }
            else if (h < 5.0)
            {
                b = max;
                g = min;
                r = (h - 4) * c + g;
            }
            else
            {
                r = max;
                g = min;
                b = g - (h - 6) * c;
            }

            set_pixel(im, x, y, 0, r);
            set_pixel(im, x, y, 1, g);
            set_pixel(im, x, y, 2, b);
        }
}

void scale_image(image im, int c, float v)
{
    for (int x = 0; x < im.w; x++)
    {
        for (int y = 0; y < im.h; y++)
        {
            set_pixel(im, x, y, c, (get_pixel(im, x, y, c)) * v);
        }
    }
}

float f(float a)
{
    float del = 6.0/29.0;
    if(a > del * del * del) return pow(a,1/3.0);
    return (a/(3*del*del) + 2*del/3.0);
}

void rgb_to_hcl(image im)
{
    for (int x = 0; x < im.w; x++)
        for (int y = 0; y < im.h; y++)
        {
            float R, G, B;
            float r, g, b;
            R = get_pixel(im, x, y, 0);
            G = get_pixel(im, x, y, 1);
            B = get_pixel(im, x, y, 2);

            // Gamma Decompression
            r = pow(R, 2.2);
            g = pow(G, 2.2);
            b = pow(B, 2.2);

            // CIE-RGB Colour-Space to CIE-XYZ Conversion
            // Matrix(M) sourced from http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
            //[X Y Z]' = M [R G B]'

            float X = 0.4887180 * r + 0.3106803 * g + 0.2006017 * b;
            float Y = 0.1762044 * r + 0.8129847 * g + 0.0108109 * b;
            float Z = 0.0000000 * r + 0.0102048 * g + 0.9897952 * b;

            // CIE-RGB Colour-Space to CIE-XYZ Conversion
            // http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_Lab.html

            float Xr,Yr,Zr;

            // Assuming Reference White Point values ar Tc = 2000K,
            Xr = 1.2743;
            Yr = 1;
            Zr = 0.1452;

            float L,a,b;

            L = 116 * f(Y/Yr) - 16;
            a = 500 * (f(X/Xr) - f(Y/Yr));
            b = 200 * (f(Y/Yr) - f(Z/Zr));

            //Finally, Conversion to HCL Colour Space.

            float H = (b/a > 0) ? tanh(b/a) : tanh(b/a) + 2*3.14;
            float C = sqrt(a*a + b*b);

            set_pixel(im, x, y, 0, H);
            set_pixel(im, x, y, 1, C);
            set_pixel(im, x, y, 2, L);
        }
}