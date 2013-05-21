

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXX = clang++
endif

GH_PATH=/opt/oblong/greenhouse
DEPS_PATH=/opt/oblong/deps-64-8
GH_PKG_CONFIG=$(PKG_CONFIG_PATH):$(GH_PATH)/lib/pkgconfig $(DEPS_PATH)/bin/pkg-config

CXXFLAGS = -Wempty-body \
  -fdiagnostics-show-option \
  -Wall \
  -Wpointer-arith \
  -Wshadow \
  -Wwrite-strings \
  -Wno-overloaded-virtual \
  -Wno-strict-overflow \
  -Wno-error=unused-variable \
  -Werror \
  `PKG_CONFIG_PATH=$(GH_PKG_CONFIG) --cflags --static libGreenhouse` \
  -g -O3 -std=gnu++0x \
  -I$(DEPS_PATH)/include/ImageMagick

LDFLAGS=`PKG_CONFIG_PATH=$(GH_PKG_CONFIG) --libs --static libGreenhouse` \
  -lMagickWand6-Q16

all: magick
clean:
	-rm magick

magick: magick.C
	${CXX} $^ -lGreenhouse ${LDFLAGS} ${CXXFLAGS} -o $@
