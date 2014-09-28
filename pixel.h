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

#ifndef _pixel_h
#define _pixel_h

#ifdef SDL_GFX
#include "SDL.h"
#else
#include "load_png.h"
#endif

typedef struct
{
  unsigned char r,g,b,a;
} COLOR;

#ifdef SDL_GFX
extern void get_pixel(COLOR *c, int x, int y, SDL_Surface *surface);
#else
extern void get_pixel(COLOR *c, int x, int y, pngRawInfo *surface);
#endif

#endif

