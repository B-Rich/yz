/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of yz.
*   Copyright (C) 2014 Surplus Users Ham Society
*
*   Yz is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 2 of the License, or
*   (at your option) any later version.
*
*   Yz is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with Yz.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/*
 * draw_text.h -- Header for 2d text engine
 */

#ifndef _draw_text_h
#define _draw_text_h

#define TEXTBOX_LEFT_SPRITE 		96
#define TEXTBOX_RIGHT_SPRITE 		97
#define TEXTBOX_BOTTOM_SPRITE 		98
#define TEXTBOX_TOP_SPRITE 		99
#define TEXTBOX_TOPLEFT_SPRITE		100
#define TEXTBOX_TOPRIGHT_SPRITE		101
#define TEXTBOX_BOTTOMRIGHT_SPRITE	102
#define TEXTBOX_BOTTOMLEFT_SPRITE	103
#define TEXTBOX_BACKGROUND_SPRITE	104
#define TEXTBOX_LEFTARROW_SPRITE	105
#define TEXTBOX_RIGHTARROW_SPRITE	106
#define TEXTBOX_DOT_SPRITE		107

typedef struct {
  SPRITE *img;
} FONT;

FONT *load_font(const char *fn, int w, int h);
void free_font(FONT *fnt);
void draw_text_line(int x, int y, const char *msg, FONT *fnt);
void draw_text_background(int x, int y, int w, int h, FONT *fnt);
void draw_text_box(int x, int y,
                   char *msg, FONT *fnt);
void draw_menu_box(int x, int y,
                   char *msg, int r, int c, FONT *fnt);

#endif

