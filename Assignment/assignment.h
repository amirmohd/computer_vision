#ifndef ASSIGNMENT_H_
#define ASSIGNMENT_H_

#include<stdint.h>

typedef struct
{
	int width;
	int height;
	int channel;
	uint32_t *image_data;
}IMAGE2D;

#endif