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
 * draw_map.c -- draw tiled map
 */

#include <stdio.h>
#include <stdlib.h>

#include "sprite.h"
#include "map.h"
#include "draw_map.h"

void draw_map(int x,int y, int w, int h, int opaque,
	      Map *m, int sx, int sy, SPRITE *spr)
{
  int u,v,i,j;
  int mw,tw,th;
  int index;
  int *dat;
  Tile *tls;

  /* Cache all variables */
  mw=(int)m->w;
  tw=(int)m->tw;
  th=(int)m->th;
  dat=m->dat;
  tls=m->tiles;

  /* Do drawing */
  for(v=sy/th,j=y-(sy&(th-1));j<h;v++,j+=th)
  {
    for(u=sx/tw,i=x-(sx&(tw-1));i<w;u++,i+=tw)
    {
      index=tls[(int)(dat[v*mw+u])].index;
      if(index||opaque)
        draw_sprite(i,j,index,spr,x,y,w,h);
    }
  }
}
