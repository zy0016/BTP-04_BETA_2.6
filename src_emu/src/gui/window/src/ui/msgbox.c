/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Message Box support.
 *            
\**************************************************************************/

#include "hpwin.h"

#ifndef NOMB

#include "string.h"
#include "msg_res.h"

#define SPACE_IN_MSGBOX		20

#if (INTERFACE_MONO)
#define SPACE_BETWEEN_BTN	12
#define SPACE_BETWEEN_HTB   16  // space between the icon and the buttion
                                // on the horizontal direction.
#define SPACE_BETWEEN_VTB   4   // space between the icon and the button
                                // on the vertical direction.
#else // INTERFACE_MONO
#define SPACE_BETWEEN_BTN	32
#define SPACE_BETWEEN_HTB   32  // space between the icon and the buttion
                                // on the horizontal direction.
#define SPACE_BETWEEN_VTB   10  // space between the icon and the button
                                // on the vertical direction.
#endif // INTERFACE_MONO

#define ICON_WIDTH			32
#define ICON_HEIGHT			32

#define BUTTON_NUM			3

// the MB_ICONASTERISK bitmap
#if (COLUMN_BITMAP)
static const BYTE pAsteriskBmp[] =
{
    0xFF, 0xE0, 0x3F, 0xFF,     // 11111111111000000011111111111111
    0xFF, 0x80, 0x0F, 0xFF,     // 11111111100000000000111111111111
    0xFF, 0x00, 0x07, 0xFF,     // 11111111000000000000011111111111
    0xFE, 0x3E, 0x03, 0xFF,     // 11111110001111100000001111111111
    0xFC, 0xFF, 0x81, 0xFF,     // 11111100111111111000000111111111
    0xFB, 0xFF, 0xE0, 0xFF,     // 11111011111111111110000011111111
    0xF7, 0xFF, 0xF0, 0x7F,     // 11110111111111111111000001111111
    0xEF, 0xFF, 0xF8, 0x7F,     // 11101111111111111111100001111111
    0xCF, 0xFF, 0xF8, 0x3F,     // 11001111111111111111100000111111
    0xDF, 0xFF, 0xFC, 0x3F,     // 11011111111111111111110000111111
    0x9F, 0xFF, 0xFC, 0x3F,     // 10011111111111111111110000111111
    0xBF, 0xFF, 0xFE, 0x00,     // 10111111111111111111111000000000
    0xBF, 0xFF, 0xDE, 0x00,     // 10111111111111111101111000000000
    0x3F, 0xFF, 0xDE, 0x01,     // 00111111111111111101111000000001
    0x79, 0xC0, 0x1F, 0xFB,     // 01111001110000000001111111111011
    0x70, 0xC0, 0x1F, 0xF7,     // 01110000110000000001111111110111
    0x70, 0xC0, 0x1F, 0xEF,     // 01110000110000000001111111101111
    0x70, 0xC0, 0x1F, 0x1F,     // 01110000110000000001111100011111
    0x70, 0xC0, 0x1F, 0x1F,     // 01110000110000000001111100011111
    0x79, 0xDF, 0xDF, 0x3F,     // 01111001110111111101111100111111
    0x3F, 0xDF, 0xDE, 0x3F,     // 00111111110111111101111000111111
    0xBF, 0xFF, 0xFE, 0x3F,     // 10111111111111111111111000111111
    0xBF, 0xFF, 0xFE, 0x7F,     // 10111111111111111111111001111111
    0x9F, 0xFF, 0xFC, 0x7F,     // 10011111111111111111110001111111
    0xDF, 0xFF, 0xFC, 0xFF,     // 11011111111111111111110011111111
    0xCF, 0xFF, 0xF9, 0xFF,     // 11001111111111111111100111111111
    0xEF, 0xFF, 0xFB, 0xFF,     // 11101111111111111111101111111111
    0xF7, 0xFF, 0xF7, 0xFF,     // 11110111111111111111011111111111
    0xFB, 0xFF, 0xEF, 0xFF,     // 11111011111111111110111111111111
    0xFC, 0xFF, 0x9F, 0xFF,     // 11111100111111111001111111111111
    0xFE, 0x3E, 0x3F, 0xFF,     // 11111110001111100011111111111111
    0xFF, 0x80, 0xFF, 0xFF      // 11111111100000001111111111111111
};
#else // COLUMN_BITMAP
static const BYTE pAsteriskBmp[] =
{
    0xFF, 0xE0, 0x1F, 0xFF,     // 11111111111000000001111111111111
    0xFF, 0x0F, 0xC3, 0xFF,     // 11111111000011111100001111111111
    0xFC, 0x7F, 0xF8, 0xFF,     // 11111100011111111111100011111111
    0xF9, 0xFF, 0xFE, 0x7F,     // 11111001111111111111111001111111
    0xF7, 0xF8, 0x7F, 0xBF,     // 11110111111110000111111110111111
    0xEF, 0xF0, 0x3F, 0xDF,     // 11101111111100000011111111011111
    0xDF, 0xF0, 0x3F, 0xEF,     // 11011111111100000011111111101111
    0x9F, 0xF8, 0x7F, 0xE7,     // 10011111111110000111111111100111
    0xBF, 0xFF, 0xFF, 0xF3,     // 10111111111111111111111111110011
    0x3F, 0xFF, 0xFF, 0xF1,     // 00111111111111111111111111110001
    0x7F, 0xE0, 0x3F, 0xF9,     // 01111111111000000011111111111001
    0x7F, 0xF8, 0x3F, 0xF8,     // 01111111111110000011111111111000
    0x7F, 0xF8, 0x3F, 0xF8,     // 01111111111110000011111111111000
    0x7F, 0xF8, 0x3F, 0xF8,     // 01111111111110000011111111111000
    0x7F, 0xF8, 0x3F, 0xF8,     // 01111111111110000011111111111000
    0x3F, 0xF8, 0x3F, 0xF0,     // 00111111111110000011111111110000
    0xBF, 0xF8, 0x3F, 0xF0,     // 10111111111110000011111111110000
    0x9F, 0xF8, 0x3F, 0xE0,     // 10011111111110000011111111100000
    0xDF, 0xE0, 0x0F, 0xE1,     // 11011111111000000000111111100001
    0xEF, 0xFF, 0xFF, 0xC1,     // 11101111111111111111111111000001
    0xF7, 0xFF, 0xFF, 0x83,     // 11110111111111111111111110000011
    0xF9, 0xFF, 0xFE, 0x07,     // 11111001111111111111111000000111
    0xFC, 0x7F, 0xF8, 0x0F,     // 11111100011111111111100000001111
    0xFE, 0x0F, 0xC0, 0x1F,     // 11111110000011111100000000011111
    0xFF, 0x01, 0xC0, 0x3F,     // 11111111000000011100000000111111
    0xFF, 0xC1, 0xC0, 0xFF,     // 11111111110000011100000011111111
    0xFF, 0xF9, 0xC7, 0xFF,     // 11111111111110011100011111111111
    0xFF, 0xFE, 0xC7, 0xFF,     // 11111111111111101100011111111111
    0xFF, 0xFF, 0x47, 0xFF,     // 11111111111111110100011111111111
    0xFF, 0xFF, 0x87, 0xFF,     // 11111111111111111000011111111111
    0xFF, 0xFF, 0xC7, 0xFF,     // 11111111111111111100011111111111
    0xFF, 0xFF, 0xE7, 0xFF      // 11111111111111111110011111111111
};
#endif // COLUMN_BITMAP

// the MB_ICONEXCLAMATION bitmap
#if (COLUMN_BITMAP)
static const BYTE pExclamationBmp[] =
{
    0xFF, 0xFF, 0xFF, 0xC3,     // 11111111111111111111111111000011
    0xFF, 0xFF, 0xFF, 0x01,     // 11111111111111111111111100000001
    0xFF, 0xFF, 0xFC, 0x00,     // 11111111111111111111110000000000
    0xFF, 0xFF, 0xF0, 0x00,     // 11111111111111111111000000000000
    0xFF, 0xFF, 0xC0, 0xF0,     // 11111111111111111100000011110000
    0xFF, 0xFF, 0x03, 0xF8,     // 11111111111111110000001111111000
    0xFF, 0xFC, 0x0F, 0xF8,     // 11111111111111000000111111111000
    0xFF, 0xF0, 0x3F, 0xF8,     // 11111111111100000011111111111000
    0xFF, 0xC0, 0xFF, 0xF8,     // 11111111110000001111111111111000
    0xFF, 0x03, 0xFF, 0xF8,     // 11111111000000111111111111111000
    0xFC, 0x0F, 0xFF, 0xF8,     // 11111100000011111111111111111000
    0xF0, 0x3F, 0xFF, 0xF8,     // 11110000001111111111111111111000
    0xE0, 0xFF, 0xFF, 0xF8,     // 11100000111111111111111111111000
    0xC3, 0xFF, 0xFF, 0xF8,     // 11000011111111111111111111111000
    0xCF, 0xFF, 0xFF, 0xF8,     // 11001111111111111111111111111000
    0xBF, 0xC0, 0xFE, 0x78,     // 10111111110000001111111001111000
    0x7F, 0x80, 0x1C, 0x38,     // 01111111100000000001110000111000
    0x7F, 0x80, 0x04, 0x38,     // 01111111100000000000010000111000
    0x7F, 0x80, 0x1E, 0x78,     // 01111111100000000001111001111000
    0xBF, 0xC0, 0xFF, 0xF8,     // 10111111110000001111111111111000
    0xCF, 0xFF, 0xFF, 0xF8,     // 11001111111111111111111111111000
    0xF3, 0xFF, 0xFF, 0xF8,     // 11110011111111111111111111111000
    0xFC, 0xFF, 0xFF, 0xF8,     // 11111100111111111111111111111000
    0xFF, 0x3F, 0xFF, 0xF8,     // 11111111001111111111111111111000
    0xFF, 0xCF, 0xFF, 0xF8,     // 11111111110011111111111111111000
    0xFF, 0xF3, 0xFF, 0xF8,     // 11111111111100111111111111111000
    0xFF, 0xFC, 0xFF, 0xF8,     // 11111111111111001111111111111000
    0xFF, 0xFF, 0x3F, 0xF9,     // 11111111111111110011111111111001
    0xFF, 0xFF, 0xCF, 0xFB,     // 11111111111111111100111111111011
    0xFF, 0xFF, 0xF3, 0xFB,     // 11111111111111111111001111111011
    0xFF, 0xFF, 0xFC, 0xF7,     // 11111111111111111111110011110111
    0xFF, 0xFF, 0xFF, 0x0F      // 11111111111111111111111100001111
};
#else // COLUMN_BITMAP
static const BYTE pExclamationBmp[] =
{
    0xFF,0xF8,0xFF,0xFF,     // 11111111111110001111111111111111
    0xFF,0xF7,0x7F,0xFF,     // 11111111111101110111111111111111
    0xFF,0xEF,0x9F,0xFF,     // 11111111111011111001111111111111
    0xFF,0xEF,0x8F,0xFF,     // 11111111111011111000111111111111
    0xFF,0xDF,0xC7,0xFF,     // 11111111110111111100011111111111
    0xFF,0xDF,0xC7,0xFF,     // 11111111110111111100011111111111
    0xFF,0xBF,0xE3,0xFF,     // 11111111101111111110001111111111
    0xFF,0xBF,0xE3,0xFF,     // 11111111101111111110001111111111
    0xFF,0x7F,0xF1,0xFF,     // 11111111011111111111000111111111
    0xFF,0x78,0xF1,0xFF,     // 11111111011110001111000111111111
    0xFE,0xF0,0x78,0xFF,     // 11111110111100000111100011111111
    0xFE,0xF0,0x78,0xFF,     // 11111110111100000111100011111111
    0xFD,0xF0,0x7C,0x7F,     // 11111101111100000111110001111111
    0xFD,0xF0,0x7C,0x7F,     // 11111101111100000111110001111111
    0xFB,0xF0,0x7E,0x3F,     // 11111011111100000111111000111111
    0xFB,0xF0,0x7E,0x3F,     // 11111011111100000111111000111111
    0xF7,0xF8,0xFF,0x1F,     // 11110111111110001111111100011111
    0xF7,0xF8,0xFF,0x1F,     // 11110111111110001111111100011111
    0xEF,0xF8,0xFF,0x8F,     // 11101111111110001111111110001111
    0xEF,0xFD,0xFF,0x8F,     // 11101111111111011111111110001111
    0xDF,0xFD,0xFF,0xC7,     // 11011111111111011111111111000111
    0xDF,0xFF,0xFF,0xC7,     // 11011111111111111111111111000111
    0xBF,0xFC,0xFF,0xE3,     // 10111111111111001111111111100011
    0xBF,0xF8,0x7F,0xE3,     // 10111111111110000111111111100011
    0x7F,0xF8,0x7F,0xF1,     // 01111111111110000111111111110001
    0x7F,0xFC,0xFF,0xF1,     // 01111111111111001111111111110001
    0x7F,0xFF,0xFF,0xF0,     // 01111111111111111111111111110000
    0x7F,0xFF,0xFF,0xF0,     // 01111111111111111111111111110000
    0xBF,0xFF,0xFF,0xE0,     // 10111111111111111111111111100000
    0xC0,0x00,0x00,0x00,     // 11000000000000000000000000000000
    0xF0,0x00,0x00,0x01,     // 11110000000000000000000000000001
    0xF8,0x00,0x00,0x03      // 11111000000000000000000000000011
};
#endif // COLUMN_BITMAP

// the MB_ICONQUESTION bitmap
#if (INTERFACE_MONO)
static const BYTE pQuestionBmp[] =
{
    0xFF, 0xE0, 0x3F, 0xFF,     // 11111111111000000011111111111111
    0xFF, 0x80, 0x0F, 0xFF,     // 11111111100000000000111111111111
    0xFF, 0x00, 0x07, 0xFF,     // 11111111000000000000011111111111
    0xFE, 0x7F, 0x03, 0xFF,     // 11111110011111110000001111111111
    0xFD, 0xFF, 0xC1, 0xFF,     // 11111101111111111100000111111111
    0xFB, 0xFF, 0xE0, 0xFF,     // 11111011111111111110000011111111
    0xF7, 0xFF, 0xF0, 0x7F,     // 11110111111111111111000001111111
    0xEF, 0xFF, 0xF8, 0x7F,     // 11101111111111111111100001111111
    0xDF, 0xFF, 0xFC, 0x3F,     // 11011111111111111111110000111111
    0xDF, 0xFF, 0xFC, 0x3F,     // 11011111111111111111110000111111
    0xBF, 0xFF, 0xFE, 0x3F,     // 10111111111111111111111000111111
    0xBF, 0xFF, 0xFE, 0x00,     // 10111111111111111111111000000000
    0xBE, 0x3F, 0xFE, 0x00,     // 10111110001111111111111000000000
    0x7C, 0x1F, 0xFF, 0x01,     // 01111100000111111111111100000001
    0x78, 0x0F, 0xFF, 0xFB,     // 01111000000011111111111111111011
    0x78, 0x07, 0x9F, 0xF7,     // 01111000000001111001111111110111
    0x79, 0xC1, 0x0F, 0xEF,     // 01111001110000010000111111101111
    0x7B, 0xF1, 0x0F, 0x9F,     // 01111011111100010000111110011111
    0x7B, 0x3F, 0x9F, 0x1F,     // 01111011001111111001111100011111
    0x7B, 0x1F, 0xFF, 0x3F,     // 01111011000111111111111100111111
    0x7C, 0x1F, 0xFF, 0x3F,     // 01111100000111111111111100111111
    0xBE, 0x3F, 0xFE, 0x3F,     // 10111110001111111111111000111111
    0xBF, 0xFF, 0xFE, 0x7F,     // 10111111111111111111111001111111
    0xBF, 0xFF, 0xFE, 0x7F,     // 10111111111111111111111001111111
    0xDF, 0xFF, 0xFC, 0xFF,     // 11011111111111111111110011111111
    0xDF, 0xFF, 0xFD, 0xFF,     // 11011111111111111111110111111111
    0xEF, 0xFF, 0xFB, 0xFF,     // 11101111111111111111101111111111
    0xF7, 0xFF, 0xF7, 0xFF,     // 11110111111111111111011111111111
    0xFB, 0xFF, 0xEF, 0xFF,     // 11111011111111111110111111111111
    0xFD, 0xFF, 0xDF, 0xFF,     // 11111101111111111101111111111111
    0xFE, 0x7F, 0x3F, 0xFF,     // 11111110011111110011111111111111
    0xFF, 0x80, 0xFF, 0xFF      // 11111111100000001111111111111111
};
#else
static const BYTE pQuestionBmp[] =
{
    0xFF, 0xE0, 0x1F, 0xFF,     // 11111111111000000001111111111111
    0xFF, 0x1F, 0xE3, 0xFF,     // 11111111000111111110001111111111
    0xFC, 0xFF, 0xFC, 0xFF,     // 11111100111111111111110011111111
    0xFB, 0xFF, 0xFF, 0x7F,     // 11111011111111111111111101111111
    0xF7, 0xFF, 0xFF, 0xBF,     // 11110111111111111111111110111111
    0xEF, 0xF0, 0x3F, 0xDF,     // 11101111111100000011111111011111
    0xDF, 0xEE, 0x1F, 0xEF,     // 11011111111011100001111111101111
    0xBF, 0xCF, 0x0F, 0xF7,     // 10111111110011110000111111110111
    0xBF, 0xC3, 0x0F, 0xF3,     // 10111111110000110000111111110011
    0x7F, 0xC3, 0x0F, 0xF9,     // 01111111110000110000111111111001
    0x7F, 0xE6, 0x1F, 0xF9,     // 01111111111001100001111111111001
    0x7F, 0xFE, 0x3F, 0xF8,     // 01111111111111100011111111111000
    0x7F, 0xFC, 0x7F, 0xF8,     // 01111111111111000111111111111000
    0x7F, 0xFC, 0xFF, 0xF8,     // 01111111111111001111111111111000
    0x7F, 0xFC, 0xFF, 0xF8,     // 01111111111111001111111111111000
    0x7F, 0xFF, 0xFF, 0xF8,     // 01111111111111111111111111111000
    0xBF, 0xFC, 0xFF, 0xF0,     // 10111111111111001111111111110000
    0xBF, 0xF8, 0x7F, 0xF0,     // 10111111111110000111111111110000
    0xDF, 0xF8, 0x7F, 0xE1,     // 11011111111110000111111111100001
    0xEF, 0xFC, 0xFF, 0xC1,     // 11101111111111001111111111000001
    0xF7, 0xFF, 0xFF, 0x83,     // 11110111111111111111111110000011
    0xFB, 0xFF, 0xFF, 0x07,     // 11111011111111111111111100000111
    0xFC, 0xFF, 0xFC, 0x0F,     // 11111100111111111111110000001111
    0xFE, 0x1F, 0xE0, 0x1F,     // 11111110000111111110000000011111
    0xFF, 0x03, 0xC0, 0x3F,     // 11111111000000111100000000111111
    0xFF, 0xC1, 0xC0, 0xFF,     // 11111111110000011100000011111111
    0xFF, 0xF9, 0xC7, 0xFF,     // 11111111111110011100011111111111
    0xFF, 0xFE, 0xC7, 0xFF,     // 11111111111111101100011111111111
    0xFF, 0xFF, 0x47, 0xFF,     // 11111111111111110100011111111111
    0xFF, 0xFF, 0x87, 0xFF,     // 11111111111111111000011111111111
    0xFF, 0xFF, 0xC7, 0xFF,     // 11111111111111111100011111111111
    0xFF, 0xFF, 0xE7, 0xFF      // 11111111111111111110011111111111
};
#endif // COLUMN_BITMAP

// the MB_ICONHAND bitmap
static const BYTE pHandBmp[] =
{
    0xFF,0xF0,0x0F,0xFF,     // 11111111111100000000111111111111
    0xFF,0x80,0x01,0xFF,     // 11111111100000000000000111111111
    0xFE,0x00,0x00,0x7F,     // 11111110000000000000000001111111
    0xFC,0x00,0x00,0x3F,     // 11111100000000000000000000111111
    0xF8,0x00,0x00,0x1F,     // 11111000000000000000000000011111
    0xF0,0x00,0x00,0x0F,     // 11110000000000000000000000001111
    0xE0,0x00,0x00,0x07,     // 11100000000000000000000000000111
    0xC0,0x40,0x02,0x03,     // 11000000010000000000001000000011
    0xC0,0xE0,0x07,0x03,     // 11000000111000000000011100000011
    0x81,0xF0,0x0F,0x81,     // 10000001111100000000111110000001
    0x80,0xF8,0x1F,0x01,     // 10000000111110000001111100000001
    0x80,0x7C,0x3E,0x01,     // 10000000011111000011111000000001
    0x00,0x3E,0x7C,0x00,     // 00000000001111100111110000000000
    0x00,0x1F,0xF8,0x00,     // 00000000000111111111100000000000
    0x00,0x0F,0xF0,0x00,     // 00000000000011111111000000000000
    0x00,0x07,0xE0,0x00,     // 00000000000001111110000000000000
    0x00,0x07,0xE0,0x00,     // 00000000000001111110000000000000
    0x00,0x0F,0xF0,0x00,     // 00000000000011111111000000000000
    0x00,0x1F,0xF8,0x00,     // 00000000000111111111100000000000
    0x00,0x3E,0x7C,0x00,     // 00000000001111100111110000000000
    0x80,0x7C,0x3E,0x01,     // 10000000011111000011111000000001
    0x80,0xF8,0x1F,0x01,     // 10000000111110000001111100000001
    0x81,0xF0,0x0F,0x81,     // 10000001111100000000111110000001
    0xC0,0xE0,0x07,0x03,     // 11000000111000000000011100000011
    0xC0,0x40,0x02,0x03,     // 11000000010000000000001000000011
    0xE0,0x00,0x00,0x07,     // 11100000000000000000000000000111
    0xF0,0x00,0x00,0x0F,     // 11110000000000000000000000001111
    0xF8,0x00,0x00,0x1F,     // 11111000000000000000000000011111
    0xFC,0x00,0x00,0x3F,     // 11111100000000000000000000111111
    0xFE,0x00,0x00,0x7F,     // 11111110000000000000000001111111
    0xFF,0x80,0x01,0xFF,     // 11111111100000000000000111111111
    0xFF,0xF0,0x0F,0xFF      // 11111111111100000000111111111111
};

typedef struct 
{
    DWORD   type;               // Dialog template type, must be zero
    DWORD   style;              // Dialog style
    DWORD   dwExStyle;          // Dialog extend style
    DWORD   cdit;               // Count of dialog control items
    short   x;                  // x-coordinates of dialog left-top corner
    short   y;                  // y-coordinates of dialog left-top corner
    short   cx;                 // width of dialog left-top corner
    short   cy;                 // height of dialog left-top corner
    PCSTR   lpszMenuName;       // Dialog menu name
    PCSTR   lpszClassName;      // Dialog class name
    PCSTR   lpszWindowText;     // Dialog title
    DLGITEMTEMPLATE dlgItems[BUTTON_NUM];// Dialog items : dlgItems[cdit]
} MSGBOXTEMPLATE;

typedef struct tagParamStruct
{
	PSTR	pMessage;      // address of text in message box
    UINT    uType;         // style of message box
} ParamStruct;

static BOOL CALLBACK MsgBoxProc(HWND hDlg, UINT wMsgCmd, WPARAM wParam, 
                                LPARAM lParam);

/*
**  Funciton : MessageBox
**  Purpsoe  :
**      The MessageBox function creates, displays, and operates a message
**      box. The message box contains an application-defined message and
**      title, plus any combination of predefined icons and push buttons.  
**  Params   :
**      hWnd      : Identifies the owner window of the message box to be 
**                  created. If this parameter is NULL, the message box 
**                  has no owner window. 
**      lpText    : Points to a null-terminated string containing the 
**                  message to be displayed. 
**      lpCaption : Points to a null-terminated string used for the dialog 
**                  box title. If this parameter is NULL, the default title
**                  Error is used. 
**      uType     : Specifies a set of bit flags that determine the 
**                  contents and behavior of the dialog box. This parameter
**                  can be a combination of flags from the following groups
**                  of flags. 
**			MB_OK               The message box contains one push button:
**                              OK. This is the default. 
**			MB_OKCANCEL         The message box contains two push buttons: 
**                              OK and Cancel.
**			MB_ABORTRETRYIGNORE The message box contains three push 
**                              buttons: Abort,	Retry, and Ignore
**			MB_YESNOCANCEL      The message box contains three push 
**                              buttons: Yes, No, and Cancel.
**			MB_YESNO            The message box contains two push buttons: 
**                              Yes and No. 
**			MB_RETRYCANCEL      The message box contains two push buttons: 
**                              Retry and Cancel. 
**
**			MB_ICONHAND,        
**			MB_ICONSTOP         A stop-sign icon appears in the message 
**                              box.
**			MB_ICONQUESTION     A question-mark icon appears in the message
**                              box. 
**			MB_ICONEXCLAMATION  An exclamation-point icon appears in the 
**                              message box. 
**			MB_ICONASTERISK,    
**			MB_ICONINFORMATION	An icon consisting of a lowercase letter i 
**                              in a circleappears in the message box
**								
**  Return   : 
**      The return value is zero if there is not enough memory to create 
**      the message box. If the function succeeds, the return value is one
**      of the following menu-item values returned by the dialog box: 
** 
**		IDOK                OK button was selected
**		IDCANCEL            Cancel button was selected
**		IDABORT             Abort button was selected. 
**		IDRETRY             Retry button was selected
**		IDIGNORE            Ignore button was selected
**		IDYES               Yes button was selected
**		IDNO                No button was selected
**
**/

int WINAPI MessageBox(HWND hWnd, PCSTR pszText, PCSTR pszCaption, 
                      UINT uType)
{
	MSGBOXTEMPLATE	dlgTemplate;
	ParamStruct		psParameter;

	int			iWinTitleHeight;
    int         iFrameWidth, iFrameHeight;
	int			iWinWidth, iWinHeight;
	int			iIconWidth, iIconHeight;
    int         iBtnWidth, iBtnHeight;
	int			iMaxWidthOfMsg, iMinWidthOfMsg;
	int			iLengthOfStr, iLineOfStr, iMaxLenOfStr;
	int			iLastReturnPos, iSubStrLen;
	int			i, iTmp, iCount, nDefbtn;
    RECT        rect;
	BOOL		feed;
	PCSTR		pSubStr;
	SIZE		TextSize;
	HDC			hdc;
    char chOk[10], chCancel[10], chYes[10], chNo[10], 
                chRetry[10], chAbort[10], chIgnore[10];

#ifdef WINDEBUG

    if (!g_bInit)
        return 0;

#endif

    // 如果传入的参数pszText和pszCaption非法（指针为NULL），将指针改为指向
    // 空串.

    if (!pszText)
        pszText = "";

    if (!pszCaption)
        pszCaption = "";

//    if (SysLangue == LANGUE_ENGLISH)
//        uType |= MB_ENGLISH;
    if (hInsMsgBox != NULL)
    {
        LoadString(hInsMsgBox, IDS_OK, chOk, 10);
        LoadString(hInsMsgBox, IDS_CANCEL, chCancel, 10);
        LoadString(hInsMsgBox, IDS_YES, chYes, 10);
        LoadString(hInsMsgBox, IDS_NO, chNo, 10);
        LoadString(hInsMsgBox, IDS_RETRY, chRetry, 10);
        LoadString(hInsMsgBox, IDS_ABORT, chAbort, 10);
        LoadString(hInsMsgBox, IDS_IGNORE, chIgnore, 10);
    }
    else
    {
        strcpy(chOk, "OK");
        strcpy(chCancel, "CANCEL");
        strcpy(chYes, "YES");
        strcpy(chNo, "NO");
        strcpy(chRetry, "RETRY");
        strcpy(chAbort, "ABORT");
        strcpy(chIgnore, "IGNORE");
    }

    psParameter.pMessage = (PSTR)pszText;
	psParameter.uType = uType;

	hdc = GetDC(hWnd);
	
	// caculate the dimension of button
	feed = GetTextExtentPoint32(hdc, chCancel, -1, &TextSize);

	if (!feed)
	{
		SetLastError(1);
        ReleaseDC(hWnd, hdc);
		return 0;
	}

    // SPACE_BETWEEN_HTB stands for the space between text and button frame
    // on the horizontal direction
	iBtnWidth = TextSize.cx + SPACE_BETWEEN_HTB;	

    // SPACE_BETWEEN_VTB stands for the space between text and button frame
    // on the vertical direction
	iBtnHeight = TextSize.cy + SPACE_BETWEEN_VTB;  

	// caculate the icon's dismension
	switch (uType & MB_ICONMASK)
	{
	case MB_ICONHAND:
	case MB_ICONQUESTION:
	case MB_ICONEXCLAMATION:
	case MB_ICONASTERISK:

		iIconWidth = ICON_WIDTH;
		iIconHeight = ICON_HEIGHT;

		break;

	default:

		iIconWidth = 0;
		iIconHeight = 0;

		break;
	}

	// get the height of the window caption.
    // 2 stands for the width of border between caption and client area
	iWinTitleHeight = GetSystemMetrics(SM_CYCAPTION) + 2;

	// get the width and height of window frame
	iFrameWidth = GetSystemMetrics(SM_CXDLGFRAME);
	iFrameHeight = GetSystemMetrics(SM_CYDLGFRAME);

    // get the width and height of the screen
    iWinWidth = GetSystemMetrics(SM_CXSCREEN);
    iWinHeight = GetSystemMetrics(SM_CYSCREEN);

	// caculate the maximum length of message which message box can contain
	if ((iIconWidth != 0) && (iIconHeight != 0))	// has icon
		iMaxWidthOfMsg = 4 * iWinWidth / 5 - 3 * SPACE_IN_MSGBOX - 
						iIconWidth - 2 * iFrameWidth;
	else
		iMaxWidthOfMsg = 4 * iWinWidth / 5 - 2 * SPACE_IN_MSGBOX -
						2 * iFrameWidth;

    // 8 stands for the width of a english alpha, and iMaxWidthOfMsg is 
    // multiple of 8.
    iMaxWidthOfMsg -= (iMaxWidthOfMsg % 8); 

	// set dlgTemplate
	dlgTemplate.type = 0;
	dlgTemplate.style = WS_VISIBLE | WS_DLGFRAME | WS_CAPTION;

    if (uType & MB_TOPMOST)
        dlgTemplate.style |= WS_TOPMOST;

	dlgTemplate.dwExStyle = 0;
	
	// set the default dimension of message box( that is the minimum 
    // dimension)
	dlgTemplate.cx = 2 * SPACE_IN_MSGBOX + iBtnWidth + 2 * iFrameWidth;
	dlgTemplate.cy = iWinTitleHeight + 3 * SPACE_IN_MSGBOX + 
					iIconHeight + iBtnHeight + iFrameHeight;

	// caculate the length of the message which the default window 
    // can contain
	if ((iIconWidth != 0) && (iIconHeight != 0))	// has icon
		iMinWidthOfMsg = dlgTemplate.cx - 3 * SPACE_IN_MSGBOX - 
						iIconWidth - 2 * iFrameWidth;
	else
		iMinWidthOfMsg = dlgTemplate.cx - 2 * SPACE_IN_MSGBOX - 
                        2 * iFrameWidth;

	dlgTemplate.lpszMenuName = NULL;
	dlgTemplate.lpszClassName = NULL;
	dlgTemplate.lpszWindowText = pszCaption;

	// get the length of the specified message
	iLengthOfStr = strlen(pszText);

	feed = GetTextExtentPoint32(hdc, pszText, iLengthOfStr, &TextSize);
	if (!feed)
	{
		SetLastError(1);
        ReleaseDC(hWnd, hdc);
		return 0;
	}

	// caculate the line number of message and the the length of the 
    // longest line
	pSubStr = pszText;
	iLineOfStr = 1;  // initialize the line number of the specified string
	iLastReturnPos = -1; // initialize the position of previous "\n"
	iMaxLenOfStr = 0;	 // initialize the length of the longest sub string
	iCount = 0;			 // initialize the count of chinese character

	for (i = 0; i < iLengthOfStr; i++)
	{
		if ((pszText[i] & 0x80))	// is chinese character
			iCount++;

		// caculate the character numer of sub string
		if ((i + 1 == iLengthOfStr) &&
            (pszText[i] != '\n'))	// arrive at the end of the string
			iSubStrLen = i - iLastReturnPos;
		else if (pszText[i] == '\n')			// arrive at the "\n"
			iSubStrLen = i - iLastReturnPos - 1;
		else
			iSubStrLen = i - iLastReturnPos;

		if (!iSubStrLen)	// the first character of pszText is "\n"
		{
			iLineOfStr++;
			pSubStr = pszText + i + 1;

            // record the last position of "\n"
			iLastReturnPos = i;

			continue;
		}

		feed = GetTextExtentPoint32(hdc, pSubStr, iSubStrLen, &TextSize);
    	if (!feed)
		{
			SetLastError(1);
            ReleaseDC(hWnd, hdc);
			return 0;
		}

		if (pszText[i] == '\n')
		{
			// beyond the maximum height of window
			//if (((iLineOfStr + 1)* TextBmp.height) > iMaxHeightOfMsg)
			//		break;

			iLineOfStr++;

			// record the length of the longest substring.
			iMaxLenOfStr = (iMaxLenOfStr > TextSize.cx) ? iMaxLenOfStr : 
                TextSize.cx;

			// get the next sub string
			pSubStr = pszText + i + 1;

			// record the last position of "\n"
			iLastReturnPos = i;
		}
		else
		{
            // beyond the width of maximum window.
			if (TextSize.cx > iMaxWidthOfMsg)	
			{
				if ((pszText[i] & 0x80))	// is chinese character
				{
					if ((iCount % 2) == 0)	// the current char is the low
                                            // byte of a chinese word
						iTmp = 1;
					else
						iTmp = 0;
				}
				else						// is english character
					iTmp = 0;

				iLineOfStr++;

				// record the length of the longest substring.
				iMaxLenOfStr = iMaxWidthOfMsg;

				// get the next sub string
				pSubStr = pszText + i - iTmp;

				// record the last position of return
				iLastReturnPos = i - 1 - iTmp;

				i = i - 1 - iTmp;
                iCount = 0;       // reset the chinese words counter.
			}
			else if (i + 1 == iLengthOfStr)	// arrive at the end of pszText
				iMaxLenOfStr = (iMaxLenOfStr > TextSize.cx) ? 
                                iMaxLenOfStr : TextSize.cx;
		}
	}

	// set the width and height of message box
	dlgTemplate.cx = dlgTemplate.cx - iMinWidthOfMsg + 
		((iMaxLenOfStr > iMinWidthOfMsg) ? iMaxLenOfStr : iMinWidthOfMsg);

	dlgTemplate.cy = dlgTemplate.cy - iIconHeight + 
		((iIconHeight > (iLineOfStr * (int)(TextSize.cy))) ? 
        iIconHeight : (iLineOfStr * (int)(TextSize.cy)));

    // set the flag of default button
    switch (uType & MB_DEFMASK)
    {
    case MB_DEFBUTTON1:

        nDefbtn = 1;
        break;

    case MB_DEFBUTTON2:

        nDefbtn = 2;
        break;

    case MB_DEFBUTTON3:

        nDefbtn = 3;
        break;

    case MB_DEFBUTTON4:

        nDefbtn = 4;
        break;

    default:

        nDefbtn = 0;
        break;
    }

	// handle buttons in the message box
	switch (uType & MB_TYPEMASK)
	{
	case MB_OKCANCEL:

		// adjust the width and height of the message box
		iTmp = 2 * iBtnWidth + 2 * SPACE_IN_MSGBOX + 
				SPACE_BETWEEN_BTN + 2 * iFrameWidth;

		if ((iTmp > dlgTemplate.cx) && (iTmp <= (4 * iWinWidth / 5)))
			dlgTemplate.cx = iTmp;

		dlgTemplate.cdit = 2;
  
		dlgTemplate.dlgItems[0].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
    	dlgTemplate.dlgItems[0].dwExStyle = 0;

		dlgTemplate.dlgItems[0].y = dlgTemplate.cy - iFrameHeight - 
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[0].cx = iBtnWidth;
		dlgTemplate.dlgItems[0].cy = iBtnHeight;
		dlgTemplate.dlgItems[0].lpszClassName = "BUTTON";

		dlgTemplate.dlgItems[1].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[1].dwExStyle = 0;

		dlgTemplate.dlgItems[1].y = dlgTemplate.cy - iFrameHeight - 
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[1].cx = iBtnWidth;
		dlgTemplate.dlgItems[1].cy = iBtnHeight;
        dlgTemplate.dlgItems[1].lpszClassName = "BUTTON";

        // set the default button style
        if (nDefbtn == 1)   // the OK is the default button
        {
            dlgTemplate.dlgItems[0].id = IDOK;
            dlgTemplate.dlgItems[0].lpszWindowText = chOk;

            dlgTemplate.dlgItems[1].id = IDCANCEL;
            dlgTemplate.dlgItems[1].lpszWindowText = chCancel;

            dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
				SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
            dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 + 
                SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
        }
        else
        {
            if (nDefbtn == 2)  // the CANCEL is the default button
            {
                dlgTemplate.dlgItems[1].id = IDOK;
                dlgTemplate.dlgItems[1].lpszWindowText = chOk;

                dlgTemplate.dlgItems[0].id = IDCANCEL;
                dlgTemplate.dlgItems[0].lpszWindowText = chCancel;

                dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - 
				    SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
                dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 + 
                    SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
            }                                                
            else
            {
                dlgTemplate.dlgItems[0].id = IDOK;
                dlgTemplate.dlgItems[0].lpszWindowText = chOk;
                
                dlgTemplate.dlgItems[1].id = IDCANCEL;
                dlgTemplate.dlgItems[1].lpszWindowText = chCancel;

                dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                    SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
                dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 + 
                    SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
            }
        }

		break;

	case MB_ABORTRETRYIGNORE:

		// adjust the width and height of the message box
		iTmp = 3 * iBtnWidth + 2 * SPACE_IN_MSGBOX + 
				2 * SPACE_BETWEEN_BTN + 2 * iFrameWidth;

		if ((iTmp > dlgTemplate.cx) && (iTmp <= (4 * iWinWidth / 5)))
			dlgTemplate.cx = iTmp;

		dlgTemplate.cdit = 3;

		dlgTemplate.dlgItems[0].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[0].dwExStyle = 0;

		dlgTemplate.dlgItems[0].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[0].cx = iBtnWidth;
		dlgTemplate.dlgItems[0].cy = iBtnHeight;
        dlgTemplate.dlgItems[0].lpszClassName = "BUTTON";

        dlgTemplate.dlgItems[1].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[1].dwExStyle = 0;

		dlgTemplate.dlgItems[1].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[1].cx = iBtnWidth;
		dlgTemplate.dlgItems[1].cy = iBtnHeight;
		dlgTemplate.dlgItems[1].lpszClassName = "BUTTON";

		dlgTemplate.dlgItems[2].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[2].dwExStyle = 0;

		dlgTemplate.dlgItems[2].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[2].cx = iBtnWidth;
		dlgTemplate.dlgItems[2].cy = iBtnHeight;
		dlgTemplate.dlgItems[2].lpszClassName = "BUTTON";

        // set the default button style
        if (nDefbtn == 1)
        {
            dlgTemplate.dlgItems[0].id = IDABORT;
            dlgTemplate.dlgItems[0].lpszWindowText = chAbort;

            dlgTemplate.dlgItems[1].id = IDRETRY;
            dlgTemplate.dlgItems[1].lpszWindowText = chRetry;

            dlgTemplate.dlgItems[2].id = IDIGNORE;
            dlgTemplate.dlgItems[2].lpszWindowText = chIgnore;

            dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                iBtnWidth / 2 - SPACE_BETWEEN_BTN - iBtnWidth - iFrameWidth;
            dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - iBtnWidth / 2 -
                iFrameWidth;
            dlgTemplate.dlgItems[2].x = dlgTemplate.cx / 2 + 
                iBtnWidth / 2 + SPACE_BETWEEN_BTN - iFrameWidth;
        }
        else
        {
            if (nDefbtn == 2)
            {
                dlgTemplate.dlgItems[2].id = IDABORT;
                dlgTemplate.dlgItems[2].lpszWindowText = chAbort;

                dlgTemplate.dlgItems[0].id = IDRETRY;
                dlgTemplate.dlgItems[0].lpszWindowText = chRetry;
                
                dlgTemplate.dlgItems[1].id = IDIGNORE;
                dlgTemplate.dlgItems[1].lpszWindowText = chIgnore;

                dlgTemplate.dlgItems[2].x = dlgTemplate.cx / 2 - 
                    iBtnWidth / 2 - SPACE_BETWEEN_BTN - iBtnWidth - iFrameWidth;
                dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - iBtnWidth / 2 -
                    iFrameWidth;
                dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 + 
                    iBtnWidth / 2 + SPACE_BETWEEN_BTN - iFrameWidth;
            }
            else
            {
                if (nDefbtn == 3)
                {
                    dlgTemplate.dlgItems[1].id = IDABORT;
                    dlgTemplate.dlgItems[1].lpszWindowText = chAbort;

                    dlgTemplate.dlgItems[2].id = IDRETRY;
                    dlgTemplate.dlgItems[2].lpszWindowText = chRetry;
                    
                    dlgTemplate.dlgItems[0].id = IDIGNORE;
                    dlgTemplate.dlgItems[0].lpszWindowText = chIgnore;

                    dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - 
                        iBtnWidth / 2 - SPACE_BETWEEN_BTN - iBtnWidth - iFrameWidth;
                    dlgTemplate.dlgItems[2].x = dlgTemplate.cx / 2 - iBtnWidth / 2 -
                        iFrameWidth;
                    dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 + 
                        iBtnWidth / 2 + SPACE_BETWEEN_BTN - iFrameWidth;
                }
                else
                {
                    dlgTemplate.dlgItems[0].id = IDABORT;
                    dlgTemplate.dlgItems[0].lpszWindowText = chAbort;
                    
                    dlgTemplate.dlgItems[1].id = IDRETRY;
                    dlgTemplate.dlgItems[1].lpszWindowText = chRetry;
                    
                    dlgTemplate.dlgItems[2].id = IDIGNORE;
                    dlgTemplate.dlgItems[2].lpszWindowText = chIgnore;

                    dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                        iBtnWidth / 2 - SPACE_BETWEEN_BTN - iBtnWidth - iFrameWidth;
                    dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - iBtnWidth / 2 -
                        iFrameWidth;
                    dlgTemplate.dlgItems[2].x = dlgTemplate.cx / 2 + 
                        iBtnWidth / 2 + SPACE_BETWEEN_BTN - iFrameWidth;
                }
            }
        }

		break;

	case MB_YESNOCANCEL:

		// adjust the width and height of the message box
		iTmp = 3 * iBtnWidth + 2 * SPACE_IN_MSGBOX +
				2 * SPACE_BETWEEN_BTN + 2 * iFrameWidth;

		if ((iTmp > dlgTemplate.cx) && (iTmp <= (4 * iWinWidth / 5)))
			dlgTemplate.cx = iTmp;

		dlgTemplate.cdit = 3;

		dlgTemplate.dlgItems[0].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[0].dwExStyle = 0;

		dlgTemplate.dlgItems[0].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[0].cx = iBtnWidth;
		dlgTemplate.dlgItems[0].cy = iBtnHeight;
		dlgTemplate.dlgItems[0].lpszClassName = "BUTTON";

		dlgTemplate.dlgItems[1].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[1].dwExStyle = 0;

		dlgTemplate.dlgItems[1].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[1].cx = iBtnWidth;
		dlgTemplate.dlgItems[1].cy = iBtnHeight;
		dlgTemplate.dlgItems[1].lpszClassName = "BUTTON";

        dlgTemplate.dlgItems[2].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[2].dwExStyle = 0;

		dlgTemplate.dlgItems[2].y = dlgTemplate.cy - iFrameHeight -
				SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[2].cx = iBtnWidth;
		dlgTemplate.dlgItems[2].cy = iBtnHeight;
		dlgTemplate.dlgItems[2].lpszClassName = "BUTTON";

        // set the default button style
        if (nDefbtn == 1)
        {
            dlgTemplate.dlgItems[0].id = IDYES;
            dlgTemplate.dlgItems[0].lpszWindowText = chYes;

            dlgTemplate.dlgItems[1].id = IDNO;
            dlgTemplate.dlgItems[1].lpszWindowText = chNo;

            dlgTemplate.dlgItems[2].id = IDCANCEL;
            dlgTemplate.dlgItems[2].lpszWindowText = chCancel;

            dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                iBtnWidth / 2 - SPACE_BETWEEN_BTN - iBtnWidth - iFrameWidth;
            dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - iBtnWidth / 2 -
                iFrameWidth;
            dlgTemplate.dlgItems[2].x = dlgTemplate.cx / 2 + 
                iBtnWidth / 2 + SPACE_BETWEEN_BTN - iFrameWidth;
        }
        else
        {
            if (nDefbtn == 2)
            {
                dlgTemplate.dlgItems[2].id = IDYES;
                dlgTemplate.dlgItems[2].lpszWindowText = chYes;
                
                dlgTemplate.dlgItems[0].id = IDNO;
                dlgTemplate.dlgItems[0].lpszWindowText = chNo;
                
                dlgTemplate.dlgItems[1].id = IDCANCEL;
                dlgTemplate.dlgItems[1].lpszWindowText = chCancel;

                dlgTemplate.dlgItems[2].x = dlgTemplate.cx / 2 - 
                    iBtnWidth / 2 - SPACE_BETWEEN_BTN - iBtnWidth - iFrameWidth;
                dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - iBtnWidth / 2 -
                    iFrameWidth;
                dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 + 
                    iBtnWidth / 2 + SPACE_BETWEEN_BTN - iFrameWidth;
            }
            else
            {
                if (nDefbtn == 3) 
                {
                    dlgTemplate.dlgItems[1].id = IDYES;
                    dlgTemplate.dlgItems[1].lpszWindowText = chYes;
                    
                    dlgTemplate.dlgItems[2].id = IDNO;
                    dlgTemplate.dlgItems[2].lpszWindowText = chNo;
                    
                    dlgTemplate.dlgItems[0].id = IDCANCEL;
                    dlgTemplate.dlgItems[0].lpszWindowText = chCancel;

                    dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - 
                        iBtnWidth / 2 - SPACE_BETWEEN_BTN - iBtnWidth - iFrameWidth;
                    dlgTemplate.dlgItems[2].x = dlgTemplate.cx / 2 - iBtnWidth / 2 -
                        iFrameWidth;
                    dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 + 
                        iBtnWidth / 2 + SPACE_BETWEEN_BTN - iFrameWidth;
                }
                else
                {
                    dlgTemplate.dlgItems[0].id = IDYES;
                    dlgTemplate.dlgItems[0].lpszWindowText = chYes;
                    
                    dlgTemplate.dlgItems[1].id = IDNO;
                    dlgTemplate.dlgItems[1].lpszWindowText = chNo;
                    
                    dlgTemplate.dlgItems[2].id = IDCANCEL;
                    dlgTemplate.dlgItems[2].lpszWindowText = chCancel;

                    dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                        iBtnWidth / 2 - SPACE_BETWEEN_BTN - iBtnWidth - iFrameWidth;
                    dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - iBtnWidth / 2 -
                        iFrameWidth;
                    dlgTemplate.dlgItems[2].x = dlgTemplate.cx / 2 + 
                        iBtnWidth / 2 + SPACE_BETWEEN_BTN - iFrameWidth;
                }
            }
        }

		break;

	case MB_YESNO:

		// adjust the width and height of the message box
		iTmp = 2 * iBtnWidth + 2 * SPACE_IN_MSGBOX + 
				SPACE_BETWEEN_BTN + 2 * iFrameWidth;

		if ((iTmp > dlgTemplate.cx) && (iTmp <= (4 * iWinWidth / 5)))
			dlgTemplate.cx = iTmp;

		dlgTemplate.cdit = 2;

		dlgTemplate.dlgItems[0].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[0].dwExStyle = 0;

		dlgTemplate.dlgItems[0].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[0].cx = iBtnWidth;
		dlgTemplate.dlgItems[0].cy = iBtnHeight;
        dlgTemplate.dlgItems[0].lpszClassName = "BUTTON";

		dlgTemplate.dlgItems[1].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[1].dwExStyle = 0;

		dlgTemplate.dlgItems[1].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[1].cx = iBtnWidth;
		dlgTemplate.dlgItems[1].cy = iBtnHeight;
		dlgTemplate.dlgItems[1].lpszClassName = "BUTTON";

        // set the default button style
        if (nDefbtn == 1)
        {
            dlgTemplate.dlgItems[0].id = IDYES;
            dlgTemplate.dlgItems[0].lpszWindowText = chYes;

            dlgTemplate.dlgItems[1].id = IDNO;
            dlgTemplate.dlgItems[1].lpszWindowText = chNo;

            dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
            dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 + 
                SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
        }
        else
        {
            if (nDefbtn == 2)
            {
                dlgTemplate.dlgItems[1].id = IDYES;
                dlgTemplate.dlgItems[1].lpszWindowText = chYes;
                
                dlgTemplate.dlgItems[0].id = IDNO;
                dlgTemplate.dlgItems[0].lpszWindowText = chNo;

                dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - 
                    SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
                dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 + 
                    SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
            }
            else
            {
                dlgTemplate.dlgItems[0].id = IDYES;
                dlgTemplate.dlgItems[0].lpszWindowText = chYes;
                
                dlgTemplate.dlgItems[1].id = IDNO;
                dlgTemplate.dlgItems[1].lpszWindowText = chNo;

                dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                    SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
                dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 + 
                    SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
            }
        }

		break;

	case MB_RETRYCANCEL:

		// adjust the width and height of the message box
		iTmp = 2 * iBtnWidth + 2 * SPACE_IN_MSGBOX + 
			SPACE_BETWEEN_BTN + 2 * iFrameWidth;

		if ((iTmp > dlgTemplate.cx) && (iTmp <= (4 * iWinWidth / 5)))
			dlgTemplate.cx = iTmp;

		dlgTemplate.cdit = 2;

		dlgTemplate.dlgItems[0].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[0].dwExStyle = 0;

		dlgTemplate.dlgItems[0].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[0].cx = iBtnWidth;
		dlgTemplate.dlgItems[0].cy = iBtnHeight;
		dlgTemplate.dlgItems[0].lpszClassName = "BUTTON";

		dlgTemplate.dlgItems[1].style = WS_VISIBLE | WS_TABSTOP | 
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[1].dwExStyle = 0;

		dlgTemplate.dlgItems[1].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[1].cx = iBtnWidth;
		dlgTemplate.dlgItems[1].cy = iBtnHeight;
		dlgTemplate.dlgItems[1].lpszClassName = "BUTTON";

        // set the default button style
        if (nDefbtn == 1)   // the OK is the default button
        {
            dlgTemplate.dlgItems[0].id = IDRETRY;
            dlgTemplate.dlgItems[0].lpszWindowText = chRetry;

            dlgTemplate.dlgItems[1].id = IDCANCEL;
            dlgTemplate.dlgItems[1].lpszWindowText = chCancel;

            dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
            dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 +
                SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
        }
        else
        {
            if (nDefbtn == 2)  // the CANCEL is the default button
            {
                dlgTemplate.dlgItems[1].id = IDRETRY;
                dlgTemplate.dlgItems[1].lpszWindowText = chRetry;
                
                dlgTemplate.dlgItems[0].id = IDCANCEL;
                dlgTemplate.dlgItems[0].lpszWindowText = chCancel;

                dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 - 
                    SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
                dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 +
                    SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
            }
            else
            {
                dlgTemplate.dlgItems[0].id = IDRETRY;
                dlgTemplate.dlgItems[0].lpszWindowText = chRetry;
                
                dlgTemplate.dlgItems[1].id = IDCANCEL;
                dlgTemplate.dlgItems[1].lpszWindowText = chCancel;

                dlgTemplate.dlgItems[0].x = dlgTemplate.cx / 2 - 
                    SPACE_BETWEEN_BTN / 2 - iBtnWidth - iFrameWidth;
                dlgTemplate.dlgItems[1].x = dlgTemplate.cx / 2 +
                    SPACE_BETWEEN_BTN / 2 -	iFrameWidth;
            }
        }

		break;

	default:

		dlgTemplate.cdit = 1;

		dlgTemplate.dlgItems[0].style = WS_VISIBLE | WS_TABSTOP |
            BS_PUSHBUTTON;
		dlgTemplate.dlgItems[0].dwExStyle = 0;

		dlgTemplate.dlgItems[0].x = (dlgTemplate.cx - iBtnWidth) / 2 -
            iFrameWidth;
		dlgTemplate.dlgItems[0].y = dlgTemplate.cy - iFrameHeight -
			SPACE_IN_MSGBOX - iBtnHeight - iWinTitleHeight;

		dlgTemplate.dlgItems[0].cx = iBtnWidth;
		dlgTemplate.dlgItems[0].cy = iBtnHeight;

		dlgTemplate.dlgItems[0].id = IDOK;
		dlgTemplate.dlgItems[0].lpszClassName = "BUTTON";

		dlgTemplate.dlgItems[0].lpszWindowText = chOk;

		break;
	}

	// set the position of message box

    if (hWnd && IsWindow(hWnd))
    {
        GetWindowRectEx(hWnd, &rect, W_WINDOW, XY_SCREEN);
        iWinWidth = rect.right - rect.left;
        iWinHeight = rect.bottom - rect.top;
    }
    else
    {
        rect.left = 0;
        rect.top = 0;
    }

    if (dlgTemplate.cx >= iWinWidth)
        dlgTemplate.x = (short)rect.left;
    else
    {
	    dlgTemplate.x = (short)(rect.left + 
            (iWinWidth - dlgTemplate.cx) / 2);
    }

	if (dlgTemplate.cy >= iWinHeight)
		dlgTemplate.y = (short)rect.top;
	else
    {
        dlgTemplate.y = (short)(rect.top + 
            (iWinHeight - dlgTemplate.cy) / 2);
    }

    dlgTemplate.x = (dlgTemplate.x < 0) ? 0 : dlgTemplate.x;
    dlgTemplate.y = (dlgTemplate.y < 0) ? 0 : dlgTemplate.y;

	ReleaseDC(hWnd, hdc);

    return DialogBoxIndirectParam(NULL, (LPDLGTEMPLATE)&dlgTemplate, 
        hWnd, MsgBoxProc, (LPARAM)&psParameter);
}

/*
*  the message box's window procedure
*/
static BOOL CALLBACK MsgBoxProc(HWND hDlg, UINT wMsgCmd, WPARAM wParam, 
                                LPARAM lParam)
{
	int			x, y;
	int			iMaxWidthOfMsg;
	HBITMAP		hBitmap;
    BOOL		bResult, feed;
	HDC			hdc;
	int			iIconWidth, iIconHeight;
	int			i, iTmp, iCount;
	int			iLengthOfStr, iLineOfStr;
	int			iLastReturnPos, iSubStrLen;
	PCSTR		pSubStr;
	SIZE		TextSize;
    ParamStruct* pMsgBoxData;
    PSTR        pszText;
	
    bResult = FALSE;

    pMsgBoxData = (ParamStruct*)((DWORD)GetUserData(hDlg) + 
        DLGWINDOWEXTRA - sizeof(ParamStruct));

	switch (wMsgCmd)
    {
    case WM_INITDIALOG :

        pszText = ((ParamStruct *)lParam)->pMessage;
        if (pszText)
        {
            pMsgBoxData->pMessage = (PSTR)MemAlloc(strlen(pszText) + 1);
            if (pMsgBoxData->pMessage)
                strcpy(pMsgBoxData->pMessage, pszText);
        }
        else
            pMsgBoxData->pMessage = NULL;

		pMsgBoxData->uType = ((ParamStruct*)lParam)->uType;

        return TRUE;

    case WM_DESTROY :

        if (pMsgBoxData->pMessage)
            MemFree(pMsgBoxData->pMessage);

        bResult = TRUE;

        break;

	case WM_PAINT:

		hdc = BeginPaint(hDlg, NULL);

		// create icon bitmap
		iIconWidth = ICON_WIDTH;
		iIconHeight = ICON_HEIGHT;

		switch (pMsgBoxData->uType & MB_ICONMASK)
		{
		case MB_ICONHAND:

			hBitmap = CreateBitmap(ICON_WIDTH, ICON_HEIGHT, 1, 1, 
                pHandBmp);

			break;

		case MB_ICONQUESTION:

			hBitmap = CreateBitmap(ICON_WIDTH, ICON_HEIGHT, 1, 1, 
                pQuestionBmp);

			break;

		case MB_ICONEXCLAMATION:

			hBitmap = CreateBitmap(ICON_WIDTH, ICON_HEIGHT, 1, 1, 
                pExclamationBmp);

			break;

		case MB_ICONASTERISK:

			hBitmap = CreateBitmap(ICON_WIDTH, ICON_HEIGHT, 1, 1, 
                pAsteriskBmp);

			break;

		default:

			hBitmap = NULL;
			iIconWidth = 0;
			iIconHeight = 0;

			break;
		}

		// caculate x-coordinate of every ouput lines
		if ((iIconWidth != 0) && (iIconHeight != 0))	// has icon
		{
			x = 2 * SPACE_IN_MSGBOX + iIconWidth;

			iMaxWidthOfMsg = 4 * GetSystemMetrics(SM_CXSCREEN) / 5 - 
						3 * SPACE_IN_MSGBOX - iIconWidth - 
                        2 * GetSystemMetrics(SM_CXDLGFRAME);
		}
		else
		{
			x = SPACE_IN_MSGBOX;
			iMaxWidthOfMsg = 4 * GetSystemMetrics(SM_CXSCREEN) / 5 - 
						2 * SPACE_IN_MSGBOX - 
                        2 * GetSystemMetrics(SM_CXDLGFRAME);
		}

        // 8 stands for the width of a english alpha, and iMaxWidthOfMsg 
        // is multiple of 8.
        iMaxWidthOfMsg -= (iMaxWidthOfMsg % 8);

		if (hBitmap)
        {
			BitBlt(hdc, SPACE_IN_MSGBOX, SPACE_IN_MSGBOX, ICON_WIDTH, 
                ICON_HEIGHT, (HDC) hBitmap, 0, 0, ROP_SRC);
        }

		// output the message in message box
		iLengthOfStr = strlen(pMsgBoxData->pMessage);
		pSubStr = pMsgBoxData->pMessage;
		iLineOfStr = 1;	    // init the line number of the specified string
		iLastReturnPos = -1;	// initialize the position of previous "\n"
		iCount = 0;				// initialize the count of chinese character

		for (i = 0; i < iLengthOfStr; i++)
		{
			if ((pMsgBoxData->pMessage[i] & 0x80))	// is chinese character
				iCount++;

			// caculate the length of sub string
			if ((i + 1 == iLengthOfStr) &&
                (pMsgBoxData->pMessage[i] != '\n'))	// arrive at the end of the string
				iSubStrLen = i - iLastReturnPos;
			else if (pMsgBoxData->pMessage[i] == '\n')			// arrive at the "\n"
				iSubStrLen = i - iLastReturnPos - 1;
			else
				iSubStrLen = i - iLastReturnPos;

			if (!iSubStrLen)	// the first character is "\n"
			{
				iLineOfStr++;
				pSubStr = pMsgBoxData->pMessage + i + 1;
				iLastReturnPos = i;		// record the last position of "\n"

				continue;
			}

			feed = GetTextExtentPoint32(hdc, pSubStr, iSubStrLen, &TextSize);
    		if (!feed)
			{
				SetLastError(1);
                EndPaint(hDlg, NULL);
        		return bResult;
			}

			if (pMsgBoxData->pMessage[i] == '\n')	// arrive at '\n'
			{
				// caculate the y-coordinate of ouput message and draw them
				y = SPACE_IN_MSGBOX + (iLineOfStr - 1) * (int)(TextSize.cy);

				TextOut(hdc, x, y, pSubStr, iSubStrLen);

				iLineOfStr++;

				// get the next sub string
				pSubStr = pMsgBoxData->pMessage + i + 1;

				// record the last position of "\n"
				iLastReturnPos = i;
			}
			else
			{   
                // beyond the width of max window.
				if (TextSize.cx > iMaxWidthOfMsg)	
				{
					if ((pMsgBoxData->pMessage[i] & 0x80))	// is chinese character
					{
						if ((iCount % 2) == 0)// the current char is the
                                              // low byte of a chinese word
							iTmp = 1;
						else
							iTmp = 0;
					}
					else						// is english character
						iTmp = 0;

					// caculate the y-coordinate of ouput message and draw 
                    // them
					y = SPACE_IN_MSGBOX + (iLineOfStr - 1) * 
                        (int)(TextSize.cy);

					TextOut(hdc, x, y, pSubStr, iSubStrLen - 1 - iTmp);
	
					iLineOfStr++;

					// get the next sub string
					pSubStr = pMsgBoxData->pMessage + i - iTmp;

					// record the last position of return
					iLastReturnPos = i - 1 - iTmp;

					i = i - 1 - iTmp;
                    iCount = 0;       // reset the chinese words counter.
				}
                // arrive at the end of the message
				else if (i + 1 == iLengthOfStr) 
				{
					y = SPACE_IN_MSGBOX + (iLineOfStr - 1) * (TextSize.cy);

					TextOut(hdc, x, y, pSubStr, iSubStrLen);
				}
			}
		}

        if (hBitmap)
            DeleteObject(hBitmap);
        
		EndPaint(hDlg, NULL);

		bResult = TRUE;

		break;

	case WM_CLOSE:

		EndDialog(hDlg, IDCANCEL);
		bResult = TRUE;

		break;

	case WM_COMMAND :

        if (HIWORD(wParam) == BN_CLICKED)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }

        break;
    }

    return bResult;
}

/*
**  Function : MessageBeep
**  Purpose  :
**      Plays a waveform sound. 
*/
BOOL WINAPI MessageBeep(UINT uType)
{
    return TRUE;
}

#endif  /* NOMB */
