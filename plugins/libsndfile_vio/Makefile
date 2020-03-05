#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = libsndfile_vio
# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	libsndfile_vio.cpp \
	audio.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# Extra flags
BASE_FLAGS += $(shell $(PKG_CONFIG) --cflags sndfile  )
LINK_FLAGS += $(shell $(PKG_CONFIG) --libs sndfile  )

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(LINUX),true)
TARGETS += jack
endif

TARGETS += lv2_dsp
TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------
