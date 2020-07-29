/*********************************************************************
 * Copyright (C) 2002 Maenpaa
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.Redistributions of source code must retain all copyright
 *   notices, this list of conditions and the following disclaimer.
 *
 * 2.Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 *
 * 3.The name(s) of the author(s) may not be used to endorse or
 *   promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/*********************************************************************
 * Contact information
 *
 * Topi Maenpaa
 * Machine Vision Group
 * Department of Electrical and Information Engineering
 * University of Oulu
 * P.O. Box 4500
 * 90014 University of Oulu
 * topiolli@ee.oulu.fi
 * topi.maenpaa@intopii.fi
 *
 * What this file is all about
 * An implementation of the 8-bit LBP operator.
 * This code is a C version of the original C++ code. It is a bit
 * messy because of optimizations and the fact that the code was
 * originally designed for an object-oriented libary.
 * If you find a bug, please inform me.
 ******************************************************************/


#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#include "LBP.h"
#include <stdio.h>
#include <string.h>


integerpoint points[bits];
doublepoint offsets[bits];

/*
 * Get a bilinearly interpolated value for a pixel.
 */
inline double interpolate_at_ptr(int* upperLeft, int i, int columns)
{
	double dx = 1-offsets[i].x;
	double dy = 1-offsets[i].y;
	//printf( "%d,%d,%d,%d\n", *upperLeft, *(upperLeft+1), *(upperLeft+columns), *(upperLeft+columns+1) );
	return
		*upperLeft*dx*dy +
		*(upperLeft+1)*offsets[i].x*dy +
		*(upperLeft+columns)*dx*offsets[i].y +
		*(upperLeft+columns+1)*offsets[i].x*offsets[i].y;
}
	
/*
 * Calculate the point coordinates for circular sampling of the neighborhood.
 */
void calculate_points(void)
{
	double step = 2 * M_PI / bits, tmpX, tmpY;
	int i;
	for (i=0;i<bits;i++)
		{
			tmpX = predicate * cos(i * step);
			tmpY = predicate * sin(i * step);
			points[i].x = (int)tmpX;
			points[i].y = (int)tmpY;
			offsets[i].x = tmpX - points[i].x;
			offsets[i].y = tmpY - points[i].y;
			if (offsets[i].x < 1.0e-10 && offsets[i].x > -1.0e-10) /* rounding error */
				offsets[i].x = 0;
			if (offsets[i].y < 1.0e-10 && offsets[i].y > -1.0e-10) /* rounding error */
				offsets[i].y = 0;
			
			if (tmpX < 0 && offsets[i].x != 0)
				{
					points[i].x -= 1;
					offsets[i].x += 1;
				}
			if (tmpY < 0 && offsets[i].y != 0)
				{
					points[i].y -= 1;
					offsets[i].y += 1;
				}
		}
}

// jhchoi
void calculate_points( int radius )
{
	double step = 2 * M_PI / bits, tmpX, tmpY;
	int i;
	for (i=0;i<bits;i++)
		{
			//tmpX = predicate * cos(i * step);
			//tmpY = predicate * sin(i * step);
			tmpX = (double) radius * cos(i * step);
			tmpY = (double) radius * sin(i * step);
			points[i].x = (int)tmpX;
			points[i].y = (int)tmpY;
			offsets[i].x = tmpX - points[i].x;
			offsets[i].y = tmpY - points[i].y;
			if (offsets[i].x < 1.0e-10 && offsets[i].x > -1.0e-10) /* rounding error */
				offsets[i].x = 0;
			if (offsets[i].y < 1.0e-10 && offsets[i].y > -1.0e-10) /* rounding error */
				offsets[i].y = 0;
			
			if (tmpX < 0 && offsets[i].x != 0)
				{
					points[i].x -= 1;
					offsets[i].x += 1;
				}
			if (tmpY < 0 && offsets[i].y != 0)
				{
					points[i].y -= 1;
					offsets[i].y += 1;
				}
			//if( points[i].x < 0 || points[i].y < 0 )
			//{
			//	printf( "%d, %d\n", points[i].x,  points[i].y );
			//}
		}
}


/*
 * Calculate the LBP histogram for an integer-valued image. This is an
 * optimized version of the basic 8-bit LBP operator. Note that this
 * assumes 4-byte integers. In some architectures, one must modify the
 * code to reflect a different integer size.
 * 
 * img: the image data, an array of rows*columns integers arranged in
 * a horizontal raster-scan order
 * rows: the number of rows in the image
 * columns: the number of columns in the image
 * result: an array of 256 integers. Will hold the 256-bin LBP histogram.
 * interpolated: if != 0, a circular sampling of the neighborhood is
 * performed. Each pixel value not matching the discrete image grid
 * exactly is obtained using a bilinear interpolation. You must call
 * calculate_points (only once) prior to using the interpolated version.
 * return value: result
 */
int* lbp_histogram(int* img, int rows, int columns, int* result, int interpolated)
{
	int leap = columns*predicate;
	/*Set up a circularly indexed neighborhood using nine pointers.*/
	int
		*p0 = img,
		*p1 = p0 + predicate,
		*p2 = p1 + predicate,
		*p3 = p2 + leap,
		*p4 = p3 + leap,
		*p5 = p4 - predicate,
		*p6 = p5 - predicate,
		*p7 = p6 - leap,
		*center = p7 + predicate;
	unsigned int value;
	int pred2 = predicate << 1;
	int r,c;

	memset(result,0,sizeof(int)*256); /* Clear result histogram */
		
	if (!interpolated)
		{
			for (r=0;r<rows-pred2;r++)
				{
					for (c=0;c<columns-pred2;c++)
						{
							value = 0;

							/* Unrolled loop */
							compab_mask_inc(p0,0);
							compab_mask_inc(p1,1);
							compab_mask_inc(p2,2);
							compab_mask_inc(p3,3);
							compab_mask_inc(p4,4);
							compab_mask_inc(p5,5);
							compab_mask_inc(p6,6);
							compab_mask_inc(p7,7);
							center++;

							result[value]++; /* Increase histogram bin value */
						}
					p0 += pred2;
					p1 += pred2;
					p2 += pred2;
					p3 += pred2;
					p4 += pred2;
					p5 += pred2;
					p6 += pred2;
					p7 += pred2;
					center += pred2;
				}
		}
	else
		{
			p0 = center + points[5].x + points[5].y * columns;
			p2 = center + points[7].x + points[7].y * columns;
			p4 = center + points[1].x + points[1].y * columns;
			p6 = center + points[3].x + points[3].y * columns;
				
			for (r=0;r<rows-pred2;r++)
			{
				for (c=0;c<columns-pred2;c++)
				{
					value = 0;

					/* Unrolled loop */
					compab_mask_inc(p1,1);
					compab_mask_inc(p3,3);
					compab_mask_inc(p5,5);
					compab_mask_inc(p7,7);

					/* Interpolate corner pixels */
					compab_mask((int)(interpolate_at_ptr(p0,5,columns)+0.5),0);
					compab_mask((int)(interpolate_at_ptr(p2,7,columns)+0.5),2);
					compab_mask((int)(interpolate_at_ptr(p4,1,columns)+0.5),4);
					compab_mask((int)(interpolate_at_ptr(p6,3,columns)+0.5),6);
					p0++;
					p2++;
					p4++;
					p6++;
					center++;
						
					result[value]++;
				}
				p0 += pred2;
				p1 += pred2;
				p2 += pred2;
				p3 += pred2;
				p4 += pred2;
				p5 += pred2;
				p6 += pred2;
				p7 += pred2;
				center += pred2;
			}
		}
	return result;
}

// jhchoi
int* lbp_histogram( int* img, int rows, int columns, int* result, int interpolated, int radius )
{
	int pred2 = 2; //radius << 1;	// Step
	int leap = columns*radius;
	/*Set up a circularly indexed neighborhood using nine pointers.*/
	int
		*p0 = img,
		*p1 = p0 + radius,
		*p2 = p1 + radius,
		*p3 = p2 + leap,
		*p4 = p3 + leap,
		*p5 = p4 - radius,
		*p6 = p5 - radius,
		*p7 = p6 - leap,
		*center = p7 + radius;
	unsigned int value;
	int r,c;

	memset( result, 0, sizeof(int)*256 ); /* Clear result histogram */
		
	if (!interpolated)
		{
			for (r=0;r<rows-(radius*2);r++) // r += pred2 ? (jhchoi)
				{
					for (c=0;c<columns-(radius*2);c++) // c += pred2 ? (jhchoi)
						{
							value = 0;

							/* Unrolled loop */
							compab_mask_inc(p0,0);
							compab_mask_inc(p1,1);
							compab_mask_inc(p2,2);
							compab_mask_inc(p3,3);
							compab_mask_inc(p4,4);
							compab_mask_inc(p5,5);
							compab_mask_inc(p6,6);
							compab_mask_inc(p7,7);
							center++;

							result[value]++; /* Increase histogram bin value */
						}
					p0 += pred2;
					p1 += pred2;
					p2 += pred2;
					p3 += pred2;
					p4 += pred2;
					p5 += pred2;
					p6 += pred2;
					p7 += pred2;
					center += pred2;
				}
		}
	else
		{
			p0 = center + points[5].x + points[5].y * columns;
			p2 = center + points[7].x + points[7].y * columns;
			p4 = center + points[1].x + points[1].y * columns;
			p6 = center + points[3].x + points[3].y * columns;
				
			for ( r=0; r<rows-(radius*2); r += pred2 )
				{
					for ( c=0; c<columns-(radius*2); c += pred2 )
						{
							value = 0;

							/* Unrolled loop */
							compab_mask_inc(p1,1);
							compab_mask_inc(p3,3);
							compab_mask_inc(p5,5);
							compab_mask_inc(p7,7);

							/* Interpolate corner pixels */
							compab_mask((int)(interpolate_at_ptr(p0,5,columns)+0.5),0);
							compab_mask((int)(interpolate_at_ptr(p2,7,columns)+0.5),2);
							compab_mask((int)(interpolate_at_ptr(p4,1,columns)+0.5),4);
							compab_mask((int)(interpolate_at_ptr(p6,3,columns)+0.5),6);

							p0+= pred2;
							p2+= pred2;
							p4+= pred2;
							p6+= pred2;
							center+= pred2;
								
							result[value]++;
						}
					p0 += pred2;
					p1 += pred2;
					p2 += pred2;
					p3 += pred2;
					p4 += pred2;
					p5 += pred2;
					p6 += pred2;
					p7 += pred2;
					center += pred2;
				}
		}
	return result;
}
