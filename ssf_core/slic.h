#ifndef SLIC_H
#define SLIC_H

/* Slic.h.
 *
 * Written by: Pascal Mettes.
 *
 * This file contains the class elements of the class Slic. This class is an
 * implementation of the SLIC Superpixel algorithm by Achanta et al. [PAMI'12,
 * vol. 34, num. 11, pp. 2274-2282].
 *
 * This implementation is created for the specific purpose of creating
 * over-segmentations in an OpenCV-based environment.
 */

#include "headers.h"

/* 2d matrices are handled by 2d vectors. */
#define vec2dd vector<vector<double> >
#define vec2di vector<vector<int> >
#define vec2db vector<vector<bool> >
/* The number of iterations run by the clustering algorithm. */
#define NR_ITERATIONS 10 // FIXME criar par�metro para isso aqui

/*
 * class Slic.
 *
 * In this class, an over-segmentation is created of an image, provided by the
 * step-size (distance between initial cluster locations) and the colour
 * distance parameter.
 */
class Slic {
    private:
        /* The cluster assignments and distance values for each pixel. */
        vec2di clusters;
        vec2dd distances;
        
        /* The LAB and xy values of the centers. */
        vec2dd centers;
        /* The number of occurences of each center. */
        vector<int> center_counts;
        
        /* The step size per cluster, and the colour (nc) and distance (ns)
         * parameters. */
        int step, nc, ns;
        
        /* Compute the distance between a center and an individual pixel. */
        double ComputeDist(int ci, Point pixel, Vec3b colour);
        /* Find the pixel with the lowest gradient in a 3x3 surrounding. */
       Point FindLocalMinimum(Mat image, Point center);
        
        /* Remove and initialize the 2d vectors. */
        void ClearData();
        void InitData(Mat image);

    public:
        /* Class constructors and deconstructors. */
        Slic();
        ~Slic();
        
        /* Generate an over-segmentation for an image. */
        void GenerateSuperpixels(Mat image, int step, int nc);
        /* Enforce connectivity for an image. */
        void CreateConnectivity(Mat image);
        
        /* Draw functions. Resp. displayal of the centers and the contours. */
        void DisplayCenterGrid(Mat image, Scalar colour);
        void DisplayContours(Mat image, Vec3b colour);
        void ColourWithClusterMeans(Mat image);
};

#endif

