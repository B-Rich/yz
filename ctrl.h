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

#ifndef _ctrl_h
#define _ctrl_h

#ifdef SDL_CTRL
#include <SDL/SDL.h>
#else
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

#define PRESS_RIGHT 1
#define PRESS_LEFT 2
#define PRESS_UP 4
#define PRESS_DOWN 8
#define PRESS_ENTER 16
#define PRESS_FIRE 32
#define PRESS_ESC 64
#define SET_BITS(x,bits) (x|bits)
#define RESET_BITS(x,bits) (x&~bits)

extern int input;

extern int get_input(void);
extern int get_input_keydown(int ks);
extern int get_input_keyup(int ks);

#endif

