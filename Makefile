###############################################################################
#   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
#
#   This file is part of yz.
#   Copyright (C) 2013 Surplus Users Ham Society
#
#   Yz is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 2 of the License, or
#   (at your option) any later version.
#
#   Yz is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with Yz.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################

CC = gcc
CFLAGS = -Wall -g -DSDL_CTRL -DSDL_GFX -DDEBUG
INCFLAGS =
LIBS = -lSDLmain -lSDL -lSDL_image -lexpat
LDFLAGS = -L/opt/local/lib -lSDLmain
OUTPUT = yz

.SUFFIXES: .o .c

.c.o:
	$(CC) $(CFLAGS) $(INCFLAGS) -c $*.c

BASE_OBJS = list.o pixel.o sprite.o biot.o map.o draw_map.o draw_text.o \
	    battle.o enemies.o strike.o magic.o ctrl.o fps_timer.o
OBJS = $(BASE_OBJS) world.o scanner.o actor.o player.o menu.o game.o main.o

all: yz

yz: $(OBJS)
	$(CC) -o $(OUTPUT) $(OBJS) $(LIBS) $(LDFLAGS)

clean:
	rm *.o; rm $(OUTPUT)

