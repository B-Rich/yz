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
 * sprite.c -- 2d sprite engine
 */

#include <stdlib.h>

#include "sprite.h"

#ifdef SDL_GFX
#include <SDL/SDL_image.h>
#endif

int window_width = DEFAULT_WINDOW_WIDTH;
int window_height = DEFAULT_WINDOW_HEIGHT;

#ifdef SDL_GFX
static SDL_Surface *dest;
#else
static GLXContext dest;
#endif

void set_sprite_context(void *cx, int w, int h)
{
  window_width = w;
  window_height = h;

#ifdef SDL_GFX
  dest = (SDL_Surface *) cx;
#else
  dest = (GLXContext) cx;
#endif
}

SPRITE* load_sprite(const char *fn, int w, int h)
{
  SPRITE *spr;

  spr = malloc(sizeof(SPRITE));
  if (spr == NULL) return NULL;

  spr->w = w;
  spr->h = h;

#ifdef SDL_GFX

  spr->img = IMG_Load(fn);
  if (spr->img == NULL)
  {
    free(spr);
    return NULL;
  }

  spr->nhsprites=(spr->img->w-1)/(w+1);
  spr->nvsprites=(spr->img->h-1)/(h+1);

#else

  if ((spr->img = (pngRawInfo *) malloc(sizeof(pngRawInfo))) == NULL) {
    free(spr);
    return NULL;
  }

  pngLoadRaw(fn, spr->img);

  spr->nhsprites=(spr->img->Width-1)/(w+1);
  spr->nvsprites=(spr->img->Height-1)/(h+1);

#endif

  return spr;
}

void free_sprite(SPRITE *spr)
{

#ifdef SDL_GFX
  SDL_FreeSurface(spr->img);
#else
  free(spr->img);
#endif
  free(spr);
}

#ifdef SDL_GFX

void draw_sprite(int x, int y,
		 int index, SPRITE *spr,
		 int clip_x, int clip_y, int clip_w, int clip_h)
{
  int tx,ty,sx,sy;
  int bx,by,bw,bh;
  SDL_Rect dest_rect, src_rect;

  /* Cache and precalculate variables */
  tx=x;
  ty=y;
  bx=0;
  by=0;
  bw=spr->w;
  bh=spr->h;

  /* Test if sprite is complete outside clipping box */
  if((tx+spr->w<=clip_x)||(tx>=clip_w)||(ty+spr->h<=clip_y)||(ty>=clip_h))
    return;

  /* Get starting position in pixels of indexed tile */
  sx=1+(index%spr->nhsprites)*(spr->w+1);
  sy=1+(index/spr->nhsprites)*(spr->h+1);

  /* Test if clipping is needed */
  if( tx>=clip_x || tx+spr->w<=clip_w ||
      ty>=clip_y || ty+spr->h<=clip_h )
  {
    /* x left of clipping box */
    if(tx<clip_x)
    {
      bx=clip_x-tx;
      bw=spr->w-bx;
      tx=clip_x;
    }

    /* x+w right of clipping box */
    else if(tx+spr->w>clip_w)
    {
      bx=0;
      bw=clip_w-tx;
    }

    /* y over of clipping box */
    if(ty<clip_y)
    {
      by=clip_y-ty;
      bh=spr->h-by;
      ty=clip_y;
    }

    /* y+h under clipping box */
    else if(ty+spr->h>clip_h)
    {
      by=0;
      bh=clip_h-ty;
    }
  }

  /* Calculate rects for SDL blitter */
  dest_rect.x = tx;
  dest_rect.y = ty;

  src_rect.x = sx+bx;
  src_rect.y = sy+by;
  src_rect.w = bw;
  src_rect.h = bh;

  /* Draw sprite */
  SDL_BlitSurface(spr->img, &src_rect, dest, &dest_rect);
}

#else

void draw_sprite(int x,int y,
		 int index,SPRITE *spr,
		 int clip_x,int clip_y,int clip_w,int clip_h)
{
  int w, h;
  int tx,ty,sx,sy;
  int bx,by,bw,bh;
  int nhsprites,nvsprites;

  /* Store width and height */
  w = spr->w;
  h = spr->h;
  nhsprites = spr->nhsprites;
  nvsprites = spr->nvsprites;

  /* Cache and precalculate variables */
  tx=x;
  ty=window_height-h-y; /* invert y-axis */
  bx=0;
  by=0;
  bw=w;
  bh=h;

  /* Test if sprite is complete outside clipping box */
  if((tx+w<=clip_x)||(tx>=clip_w)||(ty+h<=clip_y)||(ty>=clip_h))
    return;

  /* Get statring position in pixels of indexed tile */
  sx=1+(index%nhsprites)*(w+1);
  sy=nvsprites*h-(index/nhsprites)*(h+1)-1;

  /* Test if no clipping is needed */
  if((tx>=clip_x&&tx+w<=clip_w)&&(ty>=clip_y&&ty+h<=clip_h))
  {
    /* Set starting position */
    glPixelStorei(GL_UNPACK_ROW_LENGTH,spr->img->Width);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS,sx);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,sy);

    /* Set raster postition to location where sprites should be drawn */
    glRasterPos2i(tx,ty);

    /* Draw tile */
    glDrawPixels(bw,bh,GL_RGBA,GL_UNSIGNED_BYTE,spr->img->Data);

    /* Done */
    return;
  }

  /* Clipping is needed */

  /* x left of clipping box */
  if(tx<clip_x)
  {
    bx=clip_x-tx;
    bw=w-bx;
    tx=clip_x;
  }

  /* x+w right of clipping box */
  else if(tx+w>clip_w)
  {
    bx=0;
    bw=clip_w-tx;
  }

  /* y over of clipping box */
  if(ty<clip_y)
  {
    by=clip_y-ty;
    bh=h-by;
    ty=clip_y;
  }

  /* y+h under clipping box */
  else if(ty+h>clip_h)
  {
    by=0;
    bh=clip_h-ty;
  }

  /* Set starting position */
  glPixelStorei(GL_UNPACK_ROW_LENGTH,spr->img->Width);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS,sx+bx);
  glPixelStorei(GL_UNPACK_SKIP_ROWS,sy+by);

  /* Set raster postition to location where sprites should be drawn */
  glRasterPos2i(tx,ty);

  /* Draw tile */
  glDrawPixels(bw,bh,GL_RGBA,GL_UNSIGNED_BYTE,spr->img->Data);
}

#endif

