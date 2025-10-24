#!/bin/bash

BIN="learn"

CC="gcc"
SRC="main.c glad.c"
INC="-I."
LFLAGS="-lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl"
CFLAGS="-Wall -Wextra -Werror"

$CC $INC $SRC $LFLAGS $CFLAGS -o $BIN
