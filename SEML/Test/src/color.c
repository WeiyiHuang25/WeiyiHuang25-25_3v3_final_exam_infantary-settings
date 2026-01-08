#include <math.h>
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
/*  HSV2RGB:
 *  Converts from HSV colorspace to RGB values.
 *  HSV2RGB(iHSV_H / 3.0, 1, 1, &r, &g, &b);
 */
void HSV2RGB(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b)
{
	float f, x, y, z;
	int i;
	v *= 255.0;
	if (s == 0.0)
	{
		*r = *g = *b = (int)v;
	}
	else
	{
		while (h < 0)
			h += 360;
		h = fmod(h, 360) / 60.0;
		i = (int)h;
		f = h - i;
		x = v * (1.0 - s);
		y = v * (1.0 - (s * f));
		z = v * (1.0 - (s * (1.0 - f)));
		switch (i)
		{
		case 0:
			*r = v;
			*g = z;
			*b = x;
			break;
		case 1:
			*r = y;
			*g = v;
			*b = x;
			break;
		case 2:
			*r = x;
			*g = v;
			*b = z;
			break;
		case 3:
			*r = x;
			*g = y;
			*b = v;
			break;
		case 4:
			*r = z;
			*g = x;
			*b = v;
			break;
		case 5:
			*r = v;
			*g = x;
			*b = y;
			break;
		}
	}
}