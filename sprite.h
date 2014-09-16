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
 * sprite.h -- Header for 2d sprite engine
 */

#ifndef _sprite_h
#define _sprite_h

#define DEFAULT_WINDOW_WIDTH	640
#define DEFAULT_WINDOW_HEIGHT	480

#ifdef SDL_GFX
#include <SDL/SDL.h>
#else
#include <GL/gl.h>
#include <GL/glx.h>
#include "load_png.h"
#endif

typedef struct
{
  int w, h;
  int nvsprites, nhsprites;

#ifdef SDL_GFX
  SDL_Surface *img;
#else
  pngRawInfo *img;
#endif

} SPRITE;

extern int window_width;
extern int window_height;

extern void set_sprite_context(void *cx, int w, int h);
extern SPRITE* load_sprite(const char *fn, int w, int h);
extern void free_sprite(SPRITE *spr);
extern void draw_sprite(int x, int y,
			int index, SPRITE *spr,
                     	int clip_x, int clip_y, int clip_w, int clip_h);

#endif

