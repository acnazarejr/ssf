#ifndef _FES_DEFS_H_
#define _FES_DEFS_H_


// Data type
#define FES_DATATYPE_INVALID -1
#define FES_DATATYPE_2D 1
#define FES_DATATYPE_TEMPORAL 2

// processing unit type
#define FES_PROCESSING_INVALID -1
#define FES_PROCESSING_GPU 1
#define FES_PROCESSING_CPU 2

// region type
#define FES_REGIONTYPE_INVALID -1
#define FES_REGIONTYPE_WINDOW 1		// window - data must be passed
#define FES_REGIONTYPE_PATCH 2		// patch - data is already in the region

// request type
#define FES_REQUESTTYPE_INVALID -1
#define FES_REQUESTTYPE_REGIONS 1
#define FES_REQUESTTYPE_INTPOINTS 2

#endif