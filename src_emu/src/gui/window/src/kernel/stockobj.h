/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Define logical stock object.
 *            
\**************************************************************************/

#define PEN_HANDLE_BASE    ((long)OBJ_PEN << TYPE_SHIFT_BITS)
#define BRUSH_HANDLE_BASE  ((long)OBJ_BRUSH << TYPE_SHIFT_BITS)
#define BITMAP_HANDLE_BASE ((long)OBJ_BITMAP << TYPE_SHIFT_BITS)
#define FONT_HANDLE_BASE   ((long)OBJ_FONT << TYPE_SHIFT_BITS)

// Define color constant
#define BLACK       RGB(0,     0,   0)
#define DKGRAY      RGB( 64,  64,  64)
#define GRAY        RGB(128, 128, 128)
#define LTGRAY      RGB(192, 192, 192)
#define WTGRAY      RGB(223, 223, 223)
#define WHITE       RGB(255, 255, 255)

// Null pen
static CONST PENOBJ NullPen = 
{
    {
        {
            (HANDLE)PEN_HANDLE_BASE        // object handle
        },
        0,
        0,
        FALSE
    },
    {
        PS_NULL,            // pen style
        1,                  // pen width
        PES_DEFAULT,        // pen end style
        PFM_CENTER,         // pen frame mode
        BLACK               // pen color
    }
};

// Black Pen
static CONST PENOBJ BlackPen = 
{
    {
        {
            (HANDLE)(PEN_HANDLE_BASE + 1)    // object handle
        },
        0,
        0,
        FALSE
    },
    {
        PS_SOLID,           // pen style
        1,                  // pen width
        PES_DEFAULT,        // pen end style
        PFM_CENTER,         // pen frame mode
        BLACK               // pen color
    } 
};

// Dark Gray Pen
static CONST PENOBJ DKGrayPen = 
{
    {
        {
            (HANDLE)(PEN_HANDLE_BASE + 2)    // object handle
        },
        0,
        0,
        FALSE
    },
    {
        PS_SOLID,           // pen style
        1,                  // pen width
        PES_DEFAULT,        // pen end style
        PFM_CENTER,         // pen frame mode
        DKGRAY              // pen color
    } 
};

// Gray Pen
static CONST PENOBJ GrayPen = 
{
    {
        {
            (HANDLE)(PEN_HANDLE_BASE + 3)    // object handle
        },
        0,
        0,
        FALSE
    },
    {
        PS_SOLID,           // pen style
        1,                  // pen width
        PES_DEFAULT,        // pen end style
        PFM_CENTER,         // pen frame mode
        GRAY                // pen color
    } 
};

// Light Gray Pen
static CONST PENOBJ LTGrayPen = 
{
    {
        {
            (HANDLE)(PEN_HANDLE_BASE + 4)    // object handle
        },
        0,
        0,
        FALSE
    },
    {
        PS_SOLID,           // pen style
        1,                  // pen width
        PES_DEFAULT,        // pen end style
        PFM_CENTER,         // pen frame mode
        LTGRAY              // pen color
    } 
};

// White Gray Pen
static CONST PENOBJ WTGrayPen = 
{
    {
        {
            (HANDLE)(PEN_HANDLE_BASE + 5)    // object handle
        },
        0,
        0,
        FALSE
    },
    {
        PS_SOLID,           // pen style
        1,                  // pen width
        PES_DEFAULT,        // pen end style
        PFM_CENTER,         // pen frame mode
        WTGRAY              // pen color
    } 
};

// White pen
static CONST PENOBJ WhitePen = 
{
    {
        {
            (HANDLE)(PEN_HANDLE_BASE + 6)    // object handle
        },
        0,
        0,
        FALSE
    },
    {
        PS_SOLID,           // pen style
        1,                  // pen width
        PES_DEFAULT,        // pen end style
        PFM_CENTER,         // pen frame mode
        WHITE               // pen color
    } 
};

// Null brush
static CONST BRUSHOBJ NullBrush = 
{
    {
        {
            (HANDLE)(BRUSH_HANDLE_BASE + 7)    // object handle
        },
        0,
        0,
        FALSE
    },
    {
        16,                 // brush width
        16,                 // brush height
        BS_NULL,            // brush style
        0,                  // brush hatch style
        WHITE,              // brush color
        NULL                // brush pattern
    }
};

// Black brush
static CONST BRUSHOBJ BlackBrush = 
{
    {
        {
            (HANDLE)(BRUSH_HANDLE_BASE + 8)  // object handle
        },
        0,
        0,
        FALSE
    },
    {
        16,                 // brush width
        16,                 // brush height
        BS_SOLID,           // brush style
        0,                  // brush hatch style
        BLACK,              // brush color
        NULL                // brush pattern
    }
};

// Dark gray brush
static CONST BRUSHOBJ DKGrayBrush = 
{
    {
        {
            (HANDLE)(BRUSH_HANDLE_BASE + 9)  // object handle
        },
        0,
        0,
        FALSE
    },
    {
        16,                 // brush width
        16,                 // brush height
        BS_SOLID,           // brush style
        0,                  // brush hatch style
        DKGRAY,             // brush color
        NULL                // brush pattern
    }
};

static CONST BRUSHOBJ GrayBrush = 
{
    {
        {
            (HANDLE)(BRUSH_HANDLE_BASE + 10)  // object handle
        },
        0,
        0,
        FALSE
    },
    {
        16,                 // brush width
        16,                 // brush height
        BS_SOLID,           // brush style
        0,                  // brush hatch style
        GRAY,               // brush color
        NULL                // brush pattern
    }
};

// Light gray brush
static CONST BRUSHOBJ LTGrayBrush =
{
    {
        {
            (HANDLE)(BRUSH_HANDLE_BASE + 11)  // object handle
        },
        0,
        0,
        FALSE
    },
    {
        16,                 // brush width
        16,                 // brush height
        BS_SOLID,           // brush style
        0,                  // brush hatch style
        LTGRAY,             // brush color
        NULL                // brush pattern
    }
};

// White gray brush
static CONST BRUSHOBJ WTGrayBrush =
{
    {
        {
            (HANDLE)(BRUSH_HANDLE_BASE + 12)  // object handle
        },
        0,
        0,
        FALSE
    },
    {                       // select flag, not used 
        16,                 // brush width
        16,                 // brush height
        BS_SOLID,           // brush style
        0,                  // brush hatch style
        WTGRAY,             // brush color
        NULL                // brush pattern
    }
};

// White brush
static CONST BRUSHOBJ WhiteBrush =
{
    {
        {
            (HANDLE)(BRUSH_HANDLE_BASE + 13)  // object handle
        },
        0,
        0,
        FALSE
    },
    {                       // select flag, not used 
        16,                 // brush width
        16,                 // brush height
        BS_SOLID,           // brush style
        0,                  // brush hatch style
        WHITE,              // brush color
        NULL                // brush pattern
    }
};

static CONST FONTOBJ SystemFont = 
{
    {
        {
            (HANDLE)(FONT_HANDLE_BASE + 14)  // object handle
        },
        0,
        0,
        FALSE
    },
    {
        SYSFONTHEIGHT,      // height
        SYSFONTWIDTH,       // width
        0,                  // escapement
        0,                  // orientation
        FW_NORMAL,          // weight
        0,                  // italic
        0,                  // underline
        0,                  // strike out
        GB2312_CHARSET,     // charset
        0,                  // out precision
        0,                  // clip precision
        0,                  // quality
        FF_DONTCARE,        // pitch and family
        "ËÎÌו",             // face name
    } 
};

// Buffer for default bitmap buffer
//static char DefaultBitmapBuffer[4];

// Default bitmap for compatible DC, the bmWidthBytes, bmPlanes, 
// bmBitsPixel field will be calulated during initialization.
static const BMPOBJ DefaultBitmap = 
{
    {
        {
            (HANDLE)(BITMAP_HANDLE_BASE + 15)  // object handle
        },
        0,
        0,
        FALSE
    },
    {
        0,                              // bitmap type
        1,                              // bitmap width
        1,                              // bitmap height
        4,                              // bitmap width bytes per line
        1,                              // bitmap planes
        1,                              // bitmap bits per pixel
        NULL                            // bitmap buffer
    }, 
    NULL,                               // physical bitmap pointer
//    0,                                  // select flag
//    0                                   // deleted flag
};

