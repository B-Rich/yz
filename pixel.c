/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of yz.
*   Copyright (C) 2013 Surplus Users Ham Society
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

#include "pixel.h"

#ifdef SDL_GFX

void get_pixel(COLOR *c, int x, int y, SDL_Surface *surface)
{
    /* Extracting color components from a 32-bit color value */
    SDL_PixelFormat *fmt;
    Uint32 temp, *pixel;

    fmt=surface->format;
    SDL_LockSurface(surface);
    pixel=(Uint32*)surface->pixels+y*surface->pitch/4+x;
    SDL_UnlockSurface(surface);

    /* Get Red component */
    temp=*pixel&fmt->Rmask; /* Isolate red component */
    temp=temp>>fmt->Rshift;/* Shift it down to 8-bit */
    temp=temp<<fmt->Rloss; /* Expand to a full 8-bit number */
    c->r=(Uint8)temp;

    /* Get Green component */
    temp=*pixel&fmt->Gmask; /* Isolate green component */
    temp=temp>>fmt->Gshift;/* Shift it down to 8-bit */
    temp=temp<<fmt->Gloss; /* Expand to a full 8-bit number */
    c->g=(Uint8)temp;

    /* Get Blue component */
    temp=*pixel&fmt->Bmask; /* Isolate blue component */
    temp=temp>>fmt->Bshift;/* Shift it down to 8-bit */
    temp=temp<<fmt->Bloss; /* Expand to a full 8-bit number */
    c->b=(Uint8)temp;

    /* Get Alpha component */
    temp=*pixel&fmt->Amask; /* Isolate alpha component */
    temp=temp>>fmt->Ashift;/* Shift it down to 8-bit */
    temp=temp<<fmt->Aloss; /* Expand to a full 8-bit number */
    c->a=(Uint8)temp;
}

#else

void get_pixel(COLOR *c, int x, int y, pngRawInfo *surface)
{
  unsigned char *pixel;

  /* Find pixel */
  pixel = surface->Data + (4 * x  +  (surface->Height - y) * surface->Stride);

  c->r = pixel[0];
  c->g = pixel[1];
  c->b = pixel[2];
  c->a = pixel[3];
}

#endif

