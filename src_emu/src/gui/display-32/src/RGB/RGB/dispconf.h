#ifndef __DISPCONF_H
#define __DISPCONF_H

#define DEVICE_INDEX    0
#define TOUCH_DEVICE    1
#define DISP_BIG_ENDIAN 0
#define BRUSH_WIDTH     8
#define BRUSH_HEIGHT    8
#define USELINECOPY     1

#define SLOPELINE_SUPPORTED
//#define CIRCLE_SUPPORTED
#define POLYGON_SUPPORTED
#define ELLIPSE_SUPPORTED
#define ELLIPSEARC_SUPPORTED
#define ROUNDRECT_SUPPORTED
#define ARC_SUPPORTED

/* roundrect need ellipse*/
#ifdef ROUNDRECT_SUPPORTED
#ifndef ELLIPSE_SUPPORTED
#define ELLIPSE_SUPPORTED
#endif
#endif

/* polygon nees slopline */
#ifdef POLYGON_SUPPORTED
#ifndef SLOPELINE_SUPPORTED
#define SLOPELINE_SUPPORTED
#endif
#endif

#endif
