# ----------------------------
# Makefile Options
# ----------------------------

NAME = PROGRAM
ICON = icon.png
DESCRIPTION = "C Program by Nutpile"
COMPRESSED = YES
COMPRESSED_MODE = zx0

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
