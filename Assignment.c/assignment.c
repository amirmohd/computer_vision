#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "assignment.h"

#define RADIUS 50

void free_image(IMAGE2D* in_image)
{
	if(in_image->image_data != NULL)
	{
		free(in_image->image_data);
		in_image->image_data = NULL;
	}
	in_image->channel = 0;
	in_image->height = 0;
	in_image->width = 0;
}
int transform_image(const IMAGE2D* in_image, float percX, float percY, IMAGE2D* out_image)
{
	int width, height, channels,i,j;
	int out_width,out_height;
	int shift_x,  shift_y;

	width = in_image->width;
	height = in_image->height;
	channels = in_image->channel;

	if (channels != 4)
		return -1;

	shift_x = (int)((width * percX)/100.0f + 0.5f);
	if (shift_x < 0)
		return -1;

	shift_y = (int)((height * percY)/100.0f + 0.5f);
	if (shift_y < 0)
		return -1;

	out_width = width + shift_x;
	out_height = height + shift_y;

	//Allocating memory
	out_image->image_data = (uint32_t*)calloc(out_width*out_height,sizeof(unsigned char)*channels);
	if(out_image->image_data == NULL)
		return -1;

	memset(out_image->image_data,0xff,out_width*out_height*sizeof(unsigned char)*channels);

	out_image->channel = channels;
	out_image->height = out_height;
	out_image->width = out_width;

	//Transforming image with shift_x and shift_y values
	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			*(out_image->image_data + (i+shift_y)*out_width + j + shift_x) = *(in_image->image_data + i*width + j);
		}
	}
	return 1;
}

int rotate_image_by_90(const IMAGE2D* in_image, float angle, IMAGE2D* out_image)
{
	int width, height, channels,i,j;
	int out_width,out_height;

	width = in_image->width;
	height = in_image->height;
	channels = in_image->channel;

	if (channels != 4)
		return -1;

	if(abs(angle) != 90.0f)
		return -1;

	out_width = height;
	out_height = width;

	//Allocating memory
	out_image->image_data = (uint32_t*)calloc(out_width*out_height,sizeof(unsigned char)*channels);
	if(out_image->image_data == NULL)
		return -1;

	memset(out_image->image_data,0xff,out_width*out_height*sizeof(unsigned char)*channels);

	out_image->channel = channels;
	out_image->height = out_height;
	out_image->width = out_width;

	//Rotate anti clock
	if(angle == -90.0f)
	{
		for(i = 0; i < height; i++)
		{
			for(j = 0; j < width; j++)
			{
				*(out_image->image_data +  (out_height - j -1)*out_width + i) = *(in_image->image_data + i*width + j);
			}
		}
	}
	else //Rotate anti clock
	{
		for(i = 0; i < height; i++)
		{
			for(j = 0; j < width; j++)
			{
				*(out_image->image_data + j*out_width + i) = *(in_image->image_data + (height - i -1)*width + j);
			}
		}
	}
	return 1;
}

int circular_ehnace_image(const IMAGE2D* in_image, IMAGE2D* out_image)
{
	int width, height, channels,i,j;
	int c_x,c_y;
	int y_cord = 0;
	int x_cord = 0;
	int dist = 0;
	int red,green,blue;

	width = in_image->width;
	height = in_image->height;
	channels = in_image->channel;

	if (channels != 4)
		return -1;

	//Allocating memory
	out_image->image_data = (uint32_t*)calloc(width*height,sizeof(unsigned char)*channels);
	if(out_image->image_data == NULL)
		return -1;

	//Copy input image data
	memcpy(out_image->image_data,in_image->image_data,width*height*sizeof(unsigned char)*channels);

	out_image->channel = channels;
	out_image->height = height;
	out_image->width = width;

	c_x = width>>1;
	c_y = height>>1;

	// loop over circular region
	for(i = (c_y - RADIUS); i < (c_y + RADIUS); i++)
	{
		for(j = (c_x - RADIUS); j < (c_x + RADIUS); j++)
		{
			dist = (int)(sqrt((float)((j-c_x)*(j-c_x)) + (float)((i-c_y)*(i-c_y))) + 0.5f);

			//Getting the desired circular region with radius less than 50 pixels
			if(dist <= 49)
			{
				red = *((uint8_t*)in_image->image_data + i*4*width + 4*j);
				green = *((uint8_t*)in_image->image_data + i*4*width + 4*j+1);
				blue = *((uint8_t*)in_image->image_data + i*4*width + 4*j+2);

				//Enhancing pixel values wrt the distance
				red += (red*(50-dist)/100.0f);
				green += (green*(50-dist)/100.0f);
				blue += (blue*(50-dist)/100.0f);
				if(red > 255)
					red = 255;
				if(green > 255)
					green = 255;
				if(blue > 255)
					blue = 255;
				*((uint8_t*)out_image->image_data + i*4*width + 4*j) = red;
				*((uint8_t*)out_image->image_data + i*4*width + 4*j+1) = green;
				*((uint8_t*)out_image->image_data + i*4*width + 4*j+2) = blue;
			}
		}
	}
	return 1;
}

int main()
{
	int width, height, channels,ret;
	int desired_no_channels = 4;

	IMAGE2D input_image = {0};
	IMAGE2D out_image = {0};
	unsigned char *img = stbi_load("images.jpg", &width, &height, &channels, desired_no_channels);
	if(img == NULL) {
		printf("Error in loading the image\n");
		exit(1);
	}

	input_image.channel = desired_no_channels;
	input_image.height = height;
	input_image.width = width;
	input_image.image_data = (uint32_t*)img;

	/*
	******************************************************************
	//Transform the image in the +x direction by 25%, and create an image
	********************************************************************
	*/
	ret = transform_image(&input_image,25.0f,0.0f,&out_image);
	if(ret != -1)
		printf("transform_image function failed \n");

	stbi_write_jpg("transform_x.jpg", out_image.width, out_image.height, out_image.channel, out_image.image_data, 100);

	free_image(&out_image);

	/*
	******************************************************************
	//Transform the image in the +y direction by 25%, and create an image
	********************************************************************
	*/
	ret = transform_image(&input_image,0.0f,25.0f,&out_image);
	if(ret != -1)
		printf("transform_image function failed \n");

	stbi_write_jpg("transform_y.jpg", out_image.width, out_image.height, out_image.channel, out_image.image_data, 100);

	free_image(&out_image);

	/*
	******************************************************************
	//Rotate the image in Z by 90 degree
	********************************************************************
	*/
	ret = rotate_image_by_90(&input_image,90.0f,&out_image);
	if(ret != -1)
		printf("rotate_image_by_90 function failed \n");

	stbi_write_jpg("rotate_p90.jpg", out_image.width, out_image.height, out_image.channel, out_image.image_data, 100);

	free_image(&out_image);

	/*
	******************************************************************
	//Rotate the image in Z by -90 degree
	********************************************************************
	*/
	ret = rotate_image_by_90(&input_image,-90.0f,&out_image);
	if(ret != -1)
		printf("rotate_image_by_90 function failed \n");

	stbi_write_jpg("rotate_n90.jpg", out_image.width, out_image.height, out_image.channel, out_image.image_data, 100);

	free_image(&out_image);

	/*
	******************************************************************
	//From the center of the image, in all directions, increase the RGB values
	of the pixels in a manner that, each pixel from the center, the percentage
	drops by 1%. i.e. the center pixel's RGB will increase by 50%, and the next
	pixels in x and y directions will be 49%. This goes on and on 
	until the increase becomes 0 %.
	********************************************************************
	*/
	ret = circular_ehnace_image(&input_image,&out_image);
	if(ret != -1)
		printf("rotate_image_by_90 function failed \n");

	stbi_write_jpg("circular_ehnace_image.jpg", out_image.width, out_image.height, out_image.channel, out_image.image_data, 100);

	free_image(&out_image);
}
