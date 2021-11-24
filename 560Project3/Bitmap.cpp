// Bitmap.cpp : source file
// Implementation of Windows BMP Loader
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com
// Originally from the OpenGL Super Bible by Richard Wright Jr. and Michael Sweet

#include "pch.h"
#include "stdafx.h"
#include "Bitmap.h"

// LoadDIBitmap() - Load a DIB/BMP file from disk.
// Returns a pointer to the bitmap if successful, NULL if otherwise
void* LoadDIBitmap(char* filename, BITMAPINFO** info) {
	FILE* fp; // file pointer
	void* bits; // bitmap pixel bits
	long bitSize; // size of bitmap
	long infoSize; // size of header info
	BITMAPFILEHEADER header; // file header

	// attempt to open file, use "rb" to read the binary file
	if ((fp = fopen(filename, "rb")) == NULL)
		return(NULL);

	// read the file header and any following bitmap info
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		// couldn't read the file header, so return NULL
		fclose(fp);
		return (NULL);
	}

	// check for BM reversed
	if (header.bfType != 'MB') {
		// not a .bmp file, so return NULL
		fclose(fp);
		return (NULL);
	}

	infoSize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	if ((*info = (BITMAPINFO*)malloc(infoSize)) == NULL) {
		// couldn't allocate memory for bitmap info, so return NULL
		fclose(fp);
		return (NULL);
	}

	if (fread(*info, 1, infoSize, fp) < infoSize) {
		// couldn't read the bitmap header, so return NULL
		free(*info);
		fclose(fp);
		return (NULL);
	}

	// All header info has been read in, now allocate memory for
	// the bitmap and read it in
	if ((bitSize = (*info)->bmiHeader.biSizeImage) == 0)
		bitSize = ((*info)->bmiHeader.biWidth * (*info)->bmiHeader.biBitCount + 7) / 8 * abs((*info)->bmiHeader.biHeight);

	if ((bits = malloc(bitSize)) == NULL) {
		// couldn't allocate memory, so return NULL
		free(*info);
		fclose(fp);
		return (NULL);
	}

	if (fread(bits, 1, bitSize, fp) < bitSize) {
		// couldn't read bitmap, free memory and return NULL
		free(*info);
		free(bits);
		fclose(fp);
		return (NULL);
	}

	// everything worked!  So we can return the allocated bitmap
	fclose(fp);
	return (bits);
}

// ConvertBitsToGL() - Convert a DIB/BMP image to 24-bit RGB pixels
// Returns an RGB pixel array if successful, otherwise returns NULL
GLubyte* ConvertBitsToGL(BITMAPINFO* info, void* bits) {
	int i, j; // looping vars
	int bitSize; // Total size of bitmap
	int width; // aligned width of bitmap
	GLubyte* newBits; // new RGB bits
	GLubyte* from, * to; // RGB looping vars
	GLubyte temp; // temporary var for swapping

	// allocate memory for the RGB bitmap
	width = 3 * info->bmiHeader.biWidth;
	width = (width + 3) & ~3;
	bitSize = width * info->bmiHeader.biHeight;
	if ((newBits = (GLubyte*)calloc(bitSize, 1)) == NULL)
		return (NULL);

	// copy the original bitmap to the new array, converting as necessary
	switch (info->bmiHeader.biCompression)
	{
		case BI_RGB:
			if (info->bmiHeader.biBitCount == 24) {
				// swap red and blue in a 24-bit image
				for (i = 0; i < info->bmiHeader.biHeight; i++)
					for (j = 0, from = ((GLubyte*)bits) + i * width, to = newBits + i * width;
						j < info->bmiHeader.biWidth;
						j++, from += 3, to += 3)
				{
					to[0] = from[2];
					to[1] = from[1];
					to[2] = from[0];
				}
			}
			break;
		case BI_RLE4 :
		case BI_RLE8 :
		case BI_BITFIELDS :
			break;
	}

	return (newBits);
}