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
    // image img = load_image("figs/dog.hsv.png");
    // int p , q;
    // p = 200;
    // q = 200;
    // printf("\n\n\n%f %f %f\n",get_pixel(im,p,q,0),get_pixel(im,p,q,1),get_pixel(im,p,q,2));
    for (int x = 0; x < im.w; x++)
        for (int y = 0; y < im.h; y++)
        {
            float r, g, b;
            r = get_pixel(im, x, y, 0);
            g = get_pixel(im, x, y, 1);
            b = get_pixel(im, x, y, 2);

            float hue, saturation, value;
            float c = three_way_max(r, g, b) - three_way_min(r, g, b);

            value = three_way_max(r, g, b); // value
            if (value == 0)
            {
                saturation = 0;
                hue = 0;
            }
            else
            {
                saturation = 1 - three_way_min(r, g, b) / three_way_max(r,g,b);

                if (value == r)
                {
                    hue = ((g - b) / c);
                    if (b > g)
                        hue += 6.0;
                }
                else if (value == g)
                    hue = ((b - r) / c) + 2.0;
                else
                    hue = ((r - g) / c) + 4.0;
            }

            hue = hue / 6;

            set_pixel(im, x, y, 0, hue);
            set_pixel(im, x, y, 1, saturation);
            set_pixel(im, x, y, 2, value);
        }

    // printf("%f %f %f\n",get_pixel(im,p,q,0),get_pixel(im,p,q,1),get_pixel(im,p,q,2));
    // printf("%f %f %f\n",get_pixel(img,p,q,0),get_pixel(img,p,q,1),get_pixel(img,p,q,2));
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

            if(v == 0)
            {
                r = 0;
                b = 0;
                g = 0;
            } 
            else if(h < 1.0)
            {
                r = max;
                b = min;
                g = h * c + b;
            }
            else if(h < 2.0)
            {
                g = max;
                b = min;
                r = b - (h - 2) * c;
            }
            else if(h < 3.0)
            {
                g = max;
                r = min;
                b = (h - 2) * c + r;
            }
            else if(h < 4.0)
            {
                b = max;
                r = min;
                g = r - (h - 4) * c;
            }
            else if(h < 5.0)
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
    
    // int p , q;
    // p = 200;
    // q = 200;
    // printf("%f %f %f\n",get_pixel(im,p,q,0),get_pixel(im,p,q,1),get_pixel(im,p,q,2));
}
