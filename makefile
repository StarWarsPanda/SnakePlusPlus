# ----------------------------
# Makefile Options
# ----------------------------

# Name of the application
NAME ?= SNAKEPP

#For ICE shell
#ICON ?= icon.png
DESCRIPTION ?= "Snake++"

#Memory allocation on calc
COMPRESSED ?= NO
ARCHIVED ?= NO

CFLAGS ?= -Wall -Wextra -Oz
CXXFLAGS ?= -Wall -Wextra -Oz

# ----------------------------

ifndef CEDEV
$(error CEDEV environment path variable is not set)
endif

include $(CEDEV)/meta/makefile.mk
