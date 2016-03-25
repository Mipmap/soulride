# Product makefile for Soul Ride.

CPPSOURCES=$(wildcard src/*.cpp src/*.hpp src/gamegui/*.cpp src/gamegui/*.hpp)

ifeq "$(debug)" "1"
OBJ_DIR = Debug
else
OBJ_DIR = Release
endif


## all         : (default) compiles and copies the executable to project dir
all:
	$(MAKE) -C src $@

## clean       : remove build artifacts
clean:
	$(MAKE) -C src $@

PRODUCTS = \
	soulride_setup \
	virtual_breckenridge \
	virtual_breckenridge_linux \
	virtual_stratton \
	virtual_stratton_linux \
	virtual_jay_peak \
	virtual_jay_peak_linux

$(PRODUCTS): all
	$(MAKE) -C src/installer $@

## help        : print this help message and exit
help: makefile
	@sed -n 's/^##//p' $<

## installdeps : install dependencies on Debian-based system
installdeps:
	apt-get install build-essential libsdl1.2-dev libsdl-mixer1.2-dev

## doxygen     : generate documentation of the C++ code
doxygen: docs/doxygen/html/index.html

docs/doxygen/html/index.html: $(CPPSOURCES)
	doxygen

## cppcheck    : run static analysis on C++ source code
cppcheck: $(CPPSOURCES)
	cppcheck $(CPPSOURCES) --enable=all --platform=unix64 \
	--std=c++11 --inline-suppr --quiet --force \
	$(addprefix -I,$(INCLUDE_DIRS)) \
	-I/usr/include -I/usr/include/linux
