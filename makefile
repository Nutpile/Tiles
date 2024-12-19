# ----------------------------
# Makefile Options
# ----------------------------

NAME = TILES
ICON = icon.png
DESCRIPTION = "Tiles! a game by Nutpile"
COMPRESSED = YES
COMPRESSED_MODE = zx0

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
