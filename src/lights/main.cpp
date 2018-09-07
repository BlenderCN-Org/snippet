#include <cstdio>
#include <cstdint>
#include <random>


typedef struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

typedef struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv)
{
    RgbColor rgb;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6;

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
    case 0:
        rgb.r = hsv.v; rgb.g = t; rgb.b = p;
        break;
    case 1:
        rgb.r = q; rgb.g = hsv.v; rgb.b = p;
        break;
    case 2:
        rgb.r = p; rgb.g = hsv.v; rgb.b = t;
        break;
    case 3:
        rgb.r = p; rgb.g = q; rgb.b = hsv.v;
        break;
    case 4:
        rgb.r = t; rgb.g = p; rgb.b = hsv.v;
        break;
    default:
        rgb.r = hsv.v; rgb.g = p; rgb.b = q;
        break;
    }

    return rgb;
}

void main()
{
    /*
    <Light type="SphereLight">
    <float3 name="center" x="-1.8" y="1.6" z="-1.3"/>
    <float name="radius" value="0.003"/>
    <rgb name="radiance" value="30.0 4.0 4.0"/>
    </Light>

    <Light type="SphereLight">
    <float3 name="center" x="-1.4" y="1.6" z="-1.3"/>
    <float name="radius" value="0.007"/>
    <rgb name="radiance" value="15.0 4.0 30.0"/>
    </Light>
    */
    std::mt19937 engine(0x12345);
    std::uniform_int_distribution<> dist(0x00, 0xFF);

    const int32_t boxsize = 14;
    const float offset = float(boxsize / 2) - 0.5f;

#if 1
    for (int32_t y = 0; y < boxsize; ++y)
    {
        for (int32_t x = 0; x < boxsize; ++x)
        {
            if ((x + y) % 2 != 0)
            {
                continue;
            }
            //
            HsvColor hsv;
            hsv.h = dist(engine);
            hsv.s = 0xFF * 6 / 7;
            hsv.v = 0xFF / 2;
            const RgbColor rgb = HsvToRgb(hsv);
            printf("  <Light type = \"SphereLight\">\n");
            printf("    <float3 name = \"center\" x =\"%f\" y=\"%f\" z=\"0.0\"/>\n", float(x)*1.0f - offset,float(y)*1.0f-offset);
            printf("    <float name = \"radius\" value = \"0.2\"/>\n");
            const float scale = 1.0f / 10.0f;
            printf("    <rgb name = \"radiance\" value=\"%2.2f %2.2f %2.2f\"/>\n", float(rgb.r)*scale, float(rgb.g)*scale, float(rgb.b)*scale);
            printf("  </Light>\n");
            printf("\n");
        }
    }
#endif

#if 1
    for (int32_t y = 0; y < boxsize; ++y)
    {
        for (int32_t x = 0; x < boxsize; ++x)
        {
            if ((x + y) % 2 == 0)
            {
                continue;
            }
            printf("  <Shape type = \"Sphere\">\n");
            printf("    <Point name = \"center\" x = \"%f\" y = \"%f\" z = \"-0.0\" />\n", float(x)*1.0f - offset, float(y)*1.0f - offset);
            printf("    <Point name = \"radius\" value = \"0.45\" />\n");
            printf("    <ref name = \"bsdf\" value = \"matWhite\" />\n");
            printf("  </Shape>\n");
            printf("\n");
        }
    }
#endif
}
