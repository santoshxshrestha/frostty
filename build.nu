#!/usr/bin/env nu
 gcc main.c (pkg-config --cflags --libs sdl3)
