// SPDX-License-Identifier: GPL-2.0
/* Acorn-like font definition, with PC graphics characters */

#include <freax/font.h>

#define FONTDATAMAX 2048

static const struct font_data acorndata_8x8 = {
{ 0, 0, FONTDATAMAX, 0 }, {
/* 00 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ^@ */
/* 01 */  0x7e, 0x81, 0xa5, 0x81, 0xbd, 0x99, 0x81, 0x7e, /* ^A */
/* 02 */  0x7e, 0xff, 0xbd, 0xff, 0xc3, 0xe7, 0xff, 0x7e, /* ^B */
/* 03 */  0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00, /* ^C */
/* 04 */  0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00, /* ^D */
/* 05 */  0x00, 0x18, 0x3c, 0xe7, 0xe7, 0x3c, 0x18, 0x00, /* ^E */
/* 06 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 07 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 08 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 09 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 0A */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 0B */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 0C */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 0D */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 0E */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 0F */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 10 */  0x00, 0x60, 0x78, 0x7e, 0x7e, 0x78, 0x60, 0x00, /* |> */
/* 11 */  0x00, 0x06, 0x1e, 0x7e, 0x7e, 0x1e, 0x06, 0x00, /* <| */
/* 12 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 13 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 14 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 15 */  0x3c, 0x60, 0x3c, 0x66, 0x3c, 0x06, 0x3c, 0x00,
/* 16 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 17 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 18 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 19 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 1A */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 1B */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 1C */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 1D */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 1E */  0x00, 0x18, 0x18, 0x3c, 0x3c, 0x7e, 0x7e, 0x00, /* /\ */
/* 1F */  0x00, 0x7e, 0x7e, 0x3c, 0x3c, 0x18, 0x18, 0x00, /* \/ */
/* 20 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*   */
/* 21 */  0x18, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x18, 0x00, /* ! */
/* 22 */  0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, /* " */
/* 23 */  0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00, /* # */
/* 24 */  0x0C, 0x3F, 0x68, 0x3E, 0x0B, 0x7E, 0x18, 0x00, /* $ */
/* 25 */  0x60, 0x66, 0x0C, 0x18, 0x30, 0x66, 0x06, 0x00, /* % */
/* 26 */  0x38, 0x6C, 0x6C, 0x38, 0x6D, 0x66, 0x3B, 0x00, /* & */
/* 27 */  0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' */
/* 28 */  0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00, /* ( */
/* 29 */  0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00, /* ) */
/* 2A */  0x00, 0x18, 0x7E, 0x3C, 0x7E, 0x18, 0x00, 0x00, /* * */
/* 2B */  0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00, /* + */
/* 2C */  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, /* , */
/* 2D */  0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, /* - */
/* 2E */  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, /* . */
/* 2F */  0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x00, /* / */
/* 30 */  0x3C, 0x66, 0x6E, 0x7E, 0x76, 0x66, 0x3C, 0x00, /* 0 */
/* 31 */  0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, /* 1 */
/* 32 */  0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00, /* 2 */
/* 33 */  0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00, /* 3 */
/* 34 */  0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00, /* 4 */
/* 35 */  0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00, /* 5 */
/* 36 */  0x1C, 0x30, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00, /* 6 */
/* 37 */  0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00, /* 7 */
/* 38 */  0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00, /* 8 */
/* 39 */  0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00, /* 9 */
/* 3A */  0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, /* : */
/* 3B */  0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30, /* ; */
/* 3C */  0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00, /* < */
/* 3D */  0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, /* = */ 
/* 3E */  0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x00, /* > */
/* 3F */  0x3C, 0x66, 0x0C, 0x18, 0x18, 0x00, 0x18, 0x00, /* ? */
/* 40 */  0x3C, 0x66, 0x6E, 0x6A, 0x6E, 0x60, 0x3C, 0x00, /* @ */
/* 41 */  0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, /* A */
/* 42 */  0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00, /* B */
/* 43 */  0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00, /* C */
/* 44 */  0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00, /* D */
/* 45 */  0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00, /* E */
/* 46 */  0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00, /* F */
/* 47 */  0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00, /* G */
/* 48 */  0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, /* H */
/* 49 */  0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, /* I */
/* 4A */  0x3E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00, /* J */
/* 4B */  0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00, /* K */
/* 4C */  0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00, /* L */
/* 4D */  0x63, 0x77, 0x7F, 0x6B, 0x6B, 0x63, 0x63, 0x00, /* M */
/* 4E */  0x66, 0x66, 0x76, 0x7E, 0x6E, 0x66, 0x66, 0x00, /* N */
/* 4F */  0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, /* O */
/* 50 */  0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00, /* P */
/* 51 */  0x3C, 0x66, 0x66, 0x66, 0x6A, 0x6C, 0x36, 0x00, /* Q */
/* 52 */  0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00, /* R */
/* 53 */  0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00, /* S */
/* 54 */  0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, /* T */
/* 55 */  0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, /* U */
/* 56 */  0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, /* V */
/* 57 */  0x63, 0x63, 0x6B, 0x6B, 0x7F, 0x77, 0x63, 0x00, /* W */
/* 58 */  0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00, /* X */
/* 59 */  0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00, /* Y */
/* 5A */  0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00, /* Z */
/* 5B */  0x7C, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7C, 0x00, /* [ */
/* 5C */  0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00, 0x00, /* \ */
/* 5D */  0x3E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3E, 0x00, /* ] */
/* 5E */  0x3C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ^ */
/* 5F */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, /* _ */
/* 60 */  0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ` */
/* 61 */  0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00, /* a */
/* 62 */  0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00, /* b */
/* 63 */  0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C, 0x00, /* c */
/* 64 */  0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x00, /* d */
/* 65 */  0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, /* e */
/* 66 */  0x1C, 0x30, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x00, /* f */
/* 67 */  0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x3C, /* g */
/* 68 */  0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00, /* h */
/* 69 */  0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00, /* i */
/* 6A */  0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x70, /* j */
/* 6B */  0x60, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x00, /* k */
/* 6C */  0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, /* l */
/* 6D */  0x00, 0x00, 0x36, 0x7F, 0x6B, 0x6B, 0x63, 0x00, /* m */
/* 6E */  0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00, /* n */
/* 6F */  0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00, /* o */
/* 70 */  0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, /* p */
/* 71 */  0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x07, /* q */
/* 72 */  0x00, 0x00, 0x6C, 0x76, 0x60, 0x60, 0x60, 0x00, /* r */
/* 73 */  0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00, /* s */
/* 74 */  0x30, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x1C, 0x00, /* t */
/* 75 */  0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, /* u */
/* 76 */  0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, /* v */
/* 77 */  0x00, 0x00, 0x63, 0x6B, 0x6B, 0x7F, 0x36, 0x00, /* w */
/* 78 */  0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00, /* x */
/* 79 */  0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C, /* y */
/* 7A */  0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00, /* z */
/* 7B */  0x0C, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0C, 0x00, /* { */
/* 7C */  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, /* | */
/* 7D */  0x30, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x30, 0x00, /* } */
/* 7E */  0x31, 0x6B, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, /* ~ */
/* 7F */  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /*  */
/* 80 */  0x3c, 0x66, 0x60, 0x60, 0x66, 0x3c, 0x30, 0x60,
/* 81 */  0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x00,
/* 82 */  0x0c, 0x18, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00,
/* 83 */  0x18, 0x66, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00,
/* 84 */  0x66, 0x00, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00,
/* 85 */  0x30, 0x18, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00,
/* 86 */  0x3c, 0x66, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00,
/* 87 */  0x00, 0x00, 0x3c, 0x66, 0x60, 0x66, 0x3c, 0x60,
/* 88 */  0x3c, 0x66, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00,
/* 89 */  0x66, 0x00, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00,
/* 8A */  0x30, 0x18, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00,
/* 8B */  0x66, 0x00, 0x00, 0x38, 0x18, 0x18, 0x3c, 0x00,
/* 8C */  0x3c, 0x66, 0x00, 0x38, 0x18, 0x18, 0x3c, 0x00,
/* 8D */  0x30, 0x18, 0x00, 0x38, 0x18, 0x18, 0x3c, 0x00,
/* 8E */  0x66, 0x66, 0x00, 0x3c, 0x66, 0x7e, 0x66, 0x00,
/* 8F */  0x18, 0x66, 0x00, 0x3c, 0x66, 0x7e, 0x66, 0x00,
/* 90 */  0x0c, 0x18, 0x7e, 0x60, 0x7c, 0x60, 0x7e, 0x00,
/* 91 */  0x00, 0x00, 0x3f, 0x0d, 0x3f, 0x6c, 0x3f, 0x00,
/* 92 */  0x3f, 0x66, 0x66, 0x7f, 0x66, 0x66, 0x67, 0x00,
/* 93 */  0x3c, 0x66, 0x00, 0x3c, 0x66, 0x66, 0x3c, 0x00,
/* 94 */  0x66, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x3c, 0x00,
/* 95 */  0x30, 0x18, 0x00, 0x3c, 0x66, 0x66, 0x3c, 0x00,
/* 96 */  0x3c, 0x66, 0x00, 0x66, 0x66, 0x66, 0x3e, 0x00,
/* 97 */  0x30, 0x18, 0x00, 0x66, 0x66, 0x66, 0x3e, 0x00,
/* 98 */  0x66, 0x00, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x3c,
/* 99 */  0x66, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x00,
/* 9A */  0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00,
/* 9B */  0x08, 0x3e, 0x6b, 0x68, 0x6b, 0x3e, 0x08, 0x00,
/* 9C */  0x1c, 0x36, 0x30, 0x7c, 0x30, 0x30, 0x7e, 0x00,
/* 9D */  0x66, 0x3c, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00,
/* 9E */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* 9F */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* A0 */  0x0c, 0x18, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00,
/* A1 */  0x0c, 0x18, 0x00, 0x38, 0x18, 0x18, 0x3c, 0x00,
/* A2 */  0x0c, 0x18, 0x00, 0x3c, 0x66, 0x66, 0x3c, 0x00,
/* A3 */  0x0c, 0x18, 0x00, 0x66, 0x66, 0x66, 0x3e, 0x00,
/* A4 */  0x36, 0x6c, 0x00, 0x7c, 0x66, 0x66, 0x66, 0x00,
/* A5 */  0x36, 0x6c, 0x00, 0x66, 0x76, 0x6e, 0x66, 0x00,
/* A6 */  0x1c, 0x06, 0x1e, 0x36, 0x1e, 0x00, 0x3e, 0x00,
/* A7 */  0x1c, 0x36, 0x36, 0x36, 0x1c, 0x00, 0x3e, 0x00,
/* A8 */  0x18, 0x00, 0x18, 0x18, 0x30, 0x66, 0x3c, 0x00,
/* A9 */  0x7e, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* AA */  0x7e, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* AB */  0x40, 0xc0, 0x40, 0x4f, 0x41, 0x0f, 0x08, 0x0f,
/* AC */  0x40, 0xc0, 0x40, 0x48, 0x48, 0x0a, 0x0f, 0x02,
/* AD */  0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
/* AE */  0x00, 0x33, 0x66, 0xcc, 0xcc, 0x66, 0x33, 0x00,
/* AF */  0x00, 0xcc, 0x66, 0x33, 0x33, 0x66, 0xcc, 0x00,
/* B0 */  0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88,
/* B1 */  0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa,
/* B2 */  0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77,
/* B3 */  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
/* B4 */  0x18, 0x18, 0x18, 0xf8, 0x18, 0x18, 0x18, 0x18,
/* B5 */  0x18, 0x18, 0xf8, 0x18, 0xf8, 0x18, 0x18, 0x18,
/* B6 */  0x66, 0x66, 0x66, 0xe6, 0x66, 0x66, 0x66, 0x66,
/* B7 */  0x00, 0x00, 0x00, 0xfe, 0x66, 0x66, 0x66, 0x66,
/* B8 */  0x00, 0x00, 0xf8, 0x18, 0xf8, 0x18, 0x18, 0x18,
/* B9 */  0x66, 0x66, 0xe6, 0x06, 0xe6, 0x66, 0x66, 0x66,
/* BA */  0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
/* BB */  0x00, 0x00, 0xfe, 0x06, 0xe6, 0x66, 0x66, 0x66,
/* BC */  0x66, 0x66, 0xe6, 0x06, 0xfe, 0x00, 0x00, 0x00,
/* BD */  0x66, 0x66, 0x66, 0xfe, 0x00, 0x00, 0x00, 0x00,
/* BE */  0x18, 0x18, 0xf8, 0x18, 0xf8, 0x00, 0x00, 0x00,
/* BF */  0x00, 0x00, 0x00, 0xf8, 0x18, 0x18, 0x18, 0x18,
/* C0 */  0x18, 0x18, 0x18, 0x1f, 0x00, 0x00, 0x00, 0x00,
/* C1 */  0x18, 0x18, 0x18, 0xff, 0x00, 0x00, 0x00, 0x00,
/* C2 */  0x00, 0x00, 0x00, 0xff, 0x18, 0x18, 0x18, 0x18,
/* C3 */  0x18, 0x18, 0x18, 0x1f, 0x18, 0x18, 0x18, 0x18,
/* C4 */  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
/* C5 */  0x18, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18, 0x18,
/* C6 */  0x18, 0x18, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18,
/* C7 */  0x66, 0x66, 0x66, 0x67, 0x66, 0x66, 0x66, 0x66,
/* C8 */  0x66, 0x66, 0x67, 0x60, 0x7f, 0x00, 0x00, 0x00,
/* C9 */  0x00, 0x00, 0x7f, 0x60, 0x67, 0x66, 0x66, 0x66,
/* CA */  0x66, 0x66, 0xe7, 0x00, 0xff, 0x00, 0x00, 0x00,
/* CB */  0x00, 0x00, 0xff, 0x00, 0xe7, 0x66, 0x66, 0x66,
/* CC */  0x66, 0x66, 0x67, 0x60, 0x67, 0x66, 0x66, 0x66,
/* CD */  0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00,
/* CE */  0x66, 0x66, 0xe7, 0x00, 0xe7, 0x66, 0x66, 0x66,
/* CF */  0x18, 0x18, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00,
/* D0 */  0x66, 0x66, 0x66, 0xff, 0x00, 0x00, 0x00, 0x00,
/* D1 */  0x00, 0x00, 0xff, 0x00, 0xff, 0x18, 0x18, 0x18,
/* D2 */  0x00, 0x00, 0x00, 0xff, 0x66, 0x66, 0x66, 0x66,
/* D3 */  0x66, 0x66, 0x66, 0x7f, 0x00, 0x00, 0x00, 0x00,
/* D4 */  0x18, 0x18, 0x1f, 0x18, 0x1f, 0x00, 0x00, 0x00,
/* D5 */  0x00, 0x00, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18,
/* D6 */  0x00, 0x00, 0x00, 0x7f, 0x66, 0x66, 0x66, 0x66,
/* D7 */  0x66, 0x66, 0x66, 0xff, 0x66, 0x66, 0x66, 0x66,
/* D8 */  0x18, 0x18, 0xff, 0x18, 0xff, 0x18, 0x18, 0x18,
/* D9 */  0x18, 0x18, 0x18, 0xf8, 0x00, 0x00, 0x00, 0x00,
/* DA */  0x00, 0x00, 0x00, 0x1f, 0x18, 0x18, 0x18, 0x18,
/* DB */  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* DC */  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
/* DD */  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
/* DE */  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
/* DF */  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
/* E0 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* E1 */  0x3c, 0x66, 0x66, 0x6c, 0x66, 0x66, 0x6c, 0xc0,
/* E2 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* E3 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* E4 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* E5 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* E6 */  0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x3e, 0x60,
/* E7 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* E8 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* E9 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* EA */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* EB */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* EC */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* ED */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* EE */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* EF */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* F0 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* F1 */  0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x7e, 0x00,
/* F2 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* F3 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* F4 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* F5 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* F6 */  0x00, 0x18, 0x00, 0xff, 0x00, 0x18, 0x00, 0x00,
/* F7 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* F8 */  0x3c, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00,
/* F9 */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* FA */  0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
/* FB */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* FC */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* FD */  0x38, 0x04, 0x18, 0x20, 0x3c, 0x00, 0x00, 0x00,
/* FE */  0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
/* FF */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
} };

const struct font_desc font_acorn_8x8 = {
	.idx	= ACORN8x8_IDX,
	.name	= "Acorn8x8",
	.width	= 8,
	.height	= 8,
	.charcount = 256,
	.data	= acorndata_8x8.data,
#ifdef CONFIG_ARCH_ACORN
	.pref	= 20,
#else
	.pref	= 0,
#endif
};
