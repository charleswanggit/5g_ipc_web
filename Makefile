#############################
# 全局变量
#############################

#CROSS_COMPILE := /opt/anyka/arm-anycloud-linux-uclibcgnueabi/bin/arm-anycloud-linux-uclibcgnueabi-
#CROSS_COMPILE := arm-anycloud-linux-uclibcgnueabi-
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)gcc
STRIP := $(CROSS_COMPILE)strip

DEBUG=n
ROOT_DIR = $(shell pwd)

OUT_DIR=$(ROOT_DIR)/out
#OUT_DIR=$(ROOT_DIR)/../output

#############################
# 模块
#############################

NAME := web_demo
BUILD_TARGET_TYPE := exe

SRC += ./main.cpp
SRC += ./config.cpp
SRC += ./mongoose.c

STATIC_LDS_WITH += ./3rdparty/lib/libjsoncpp.a

GNUFLAGS += -Wall -D_GNU_SOURCE  -fno-builtin \
	 -ggdb3 #-Werror
#GNUFLAGS += -mlittle-endian -mlong-calls

CFLAGS += $(GNUFLAGS) -std=c99

CXXFLAGS += $(GNUFLAGS) -std=c++11

DYN_LDS_WITH += -lpthread -lm -lstdc++

INCDIR += -I./3rdparty/include/

# 编译结束后执行
#DONE += cp -a $(OUT_DIR)/$(NAME) /home/zsp/nfs;

#PRINT_COMPILER=y

include $(ROOT_DIR)/env/core.Makefile

