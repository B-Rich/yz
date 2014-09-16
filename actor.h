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
 * actor.h -- Game actor
 */

#ifndef _actor_h
#define _actor_h

typedef struct {
  char img_fn[80];
  int w, h, treshold;
  int start_frame_down;
  int stand_frame_down;
  int end_frame_down;
  int start_frame_left;
  int stand_frame_left;
  int end_frame_left;
  int start_frame_right;
  int stand_frame_right;
  int end_frame_right;
  int start_frame_up;
  int stand_frame_up;
  int end_frame_up;
  SPRITE *img;
  STATUS status;
  int counter;
  int hit;
  int index;
} ACTOR;

extern ACTOR *load_actor_XML(const char *fn);
extern void free_actor(ACTOR *hero);
extern int actor_alive(ACTOR *hero);
extern void draw_actor_status(int x, int y, ACTOR *hero);

#endif

