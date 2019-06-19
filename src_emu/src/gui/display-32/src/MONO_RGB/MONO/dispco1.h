#ifndef __DISPCONF_H
#define __DISPCONF_H

#define LDD_MODE        DDM_MONO
#define DEVICE_INDEX    SCREEN_SECOND
#define TOUCH_DEVICE    0
#define BIG_ENDIAN      0
#define BRUSH_WIDTH     32
#define BRUSH_HEIGHT    16

#define SLOPELINE_SUPPORTED
//#define CIRCLE_SUPPORTED
//#define POLYGON_SUPPORTED
//#define ELLIPSE_SUPPORTED
//#define ELLIPSEARC_SUPPORTED
#define ROUNDRECT_SUPPORTED
//#define ARC_SUPPORTED

/* polygon nees slopline */
#ifndef SLOPELINE_SUPPORTED
#undef POLYGON_SUPPORTED
#endif

/* circle arc need ellipse arc 
** roundrect need ellipse arc
*/
#ifndef ELLIPSEARC_SUPPORTED
#undef ARC_SUPPORTED
#endif

/* roundrect need ellipse*/
#ifdef ROUNDRECT_SUPPORTED
#ifndef ELLIPSE_SUPPORTED
#define ELLIPSE_SUPPORTED
#endif
#endif


#endif
