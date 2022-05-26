DEBUG = 1

3DO_SDK    		= ~/Documents/Development/3do-devkit
EXTRA_TOOLS 	= ~/Documents/Development/3do-extra-tools

FILESYSTEM = takeme
EXENAME	   = $(FILESYSTEM)/LaunchMe
ISONAME    = Hello3DO.iso
STACKSIZE  = 4096
BANNER	   = $(GRAPHICS_SRC)/banner/banner.bmp

COMPILER_PATH = $(3DO_SDK)/bin/compiler/linux
TOOLS_PATH = $(3DO_SDK)/bin/tools/linux

CC	   = $(COMPILER_PATH)/armcc
CXX	   = $(COMPILER_PATH)/armcpp
AS 	   = $(COMPILER_PATH)/armasm
LD	   = $(COMPILER_PATH)/armlink
RM	   = rm
CP	   = cp
WINE   = wine
RETRO_ARCH = retroarch
MODBIN     		= $(TOOLS_PATH)/modbin
MAKEBANNER 		= $(TOOLS_PATH)/MakeBanner
3DOISO     		= $(TOOLS_PATH)/3doiso
3DOENCRYPT 		= $(TOOLS_PATH)/3DOEncrypt
BMPTO3DOIMAGE 	= $(EXTRA_TOOLS)/BMPTo3DOImage.exe
BMPTO3DOCEL 	= $(EXTRA_TOOLS)/BMPTo3DOCel.exe


## Flag definitions ##
# -bigend   : Compiles code for an ARM operating with big-endian memory. The most
#             significant byte has lowest address.
# -za0      : LDR is not restricted to accessing word-aligned addresses.
# -zps0     :
# -zpv1     :
# -zi4      : The compiler selects a value for the maximum number of instructions
#             allowed to generate an integer literal inline before using LDR rx,=value
# -fa       : Checks for certain types of data flow anomalies.
# -fh       : Checks "all external objects are declared before use",
#             "all file-scoped static objects are used",
#             "all predeclarations of static functions are used between
#              their declaration and their definition".
# -fv       : Reports on all unused declarations, including those from standard headers.
# -fx       : Enables all warnings that are suppressed by default.
# -fpu none : No FPU. Use software floating point library.
# -arch 3   : Compile using ARM3 architecture.
# -apcs     : nofp - Do not use a frame-pointer register. This is the default.
#             softfp - Use software floating-point library functions for floating-point code.
#             noswstackcheck - No software stack-checking PCS variant. This is the default.
OPT      = -O0
CFLAGS   = $(OPT) -bigend -za1 -zps0 -zi4 -fa -fh -fx -fpu none -arch 3 -apcs '/softfp/nofp/swstackcheck'
CXXFLAGS = $(CFLAGS)
ASFLAGS	 = -BI -i $(3DO_SDK)/include/3do
INCPATH	 = -I $(3DO_SDK)/include/3do -I $(3DO_SDK)/include/ttl
LIBPATH  = $(3DO_SDK)/lib/3do
LDFLAGS	 = -match 0x1 -nodebug -noscanlib -verbose -remove -aif -reloc -dupok -ro-base 0 -sym $(EXENAME).sym -libpath $(LIBPATH)
STARTUP	 = $(LIBPATH)/cstartup.o

LIBS =					\
    $(LIBPATH)/clib.lib		\
    $(LIBPATH)/cpluslib.lib		\
    $(LIBPATH)/graphics.lib         \
    $(LIBPATH)/lib3do.lib           \
    $(LIBPATH)/filesystem.lib       \
    $(LIBPATH)/operamath.lib		\
	$(LIBPATH)/audio.lib 			\
	$(LIBPATH)/music.lib 			\
	$(LIBPATH)/exampleslib.lib 		\
	$(LIBPATH)/input.lib			\
	$(LIBPATH)/swi.lib 				

SRC_S   = $(wildcard src/*.s)
SRC_C   = $(wildcard src/*.c)
SRC_CXX = $(wildcard src/*.cpp)

OBJ += $(SRC_S:src/%.s=build/%.s.o)
OBJ += $(SRC_C:src/%.c=build/%.c.o)
OBJ += $(SRC_CXX:src/%.cpp=build/%.cpp.o)

SYSTEM = $(FILESYSTEM)/System
SIGNATURES = $(FILESYSTEM)/signatures
ROM_TAGS = $(FILESYSTEM)/rom_tags
BANNER_SCREEN = $(FILESYSTEM)/BannerScreen

GAME_GRAPHICS = graphics/game
CD_GRAPHICS = $(FILESYSTEM)/Graphics

BANNER = graphics/banner/banner.bmp

BUILD = build

all: copy convert_bmp banner launchme modbin iso copy_iso

convert_bmp: buildgraphicsdir
	$(WINE) $(BMPTO3DOIMAGE) $(GAME_GRAPHICS)/bg.bmp $(CD_GRAPHICS)/bg
	
	$(WINE) $(BMPTO3DOCEL) $(GAME_GRAPHICS)/sun.bmp $(CD_GRAPHICS)/sun
	$(WINE) $(BMPTO3DOCEL) $(GAME_GRAPHICS)/bullet.bmp $(CD_GRAPHICS)/bullet
		
copy:
	$(CP) -r $(3DO_SDK)/takeme/System $(FILESYSTEM)

launchme: builddir $(OBJ)
	$(LD) -o $(EXENAME) $(LDFLAGS) $(STARTUP) $(LIBS) $(OBJ)

modbin:
	$(MODBIN) --stack=$(STACKSIZE) $(EXENAME) $(EXENAME)

banner:
	$(MAKEBANNER) $(BANNER) $(FILESYSTEM)/BannerScreen

iso:
	$(3DOISO) -in $(FILESYSTEM) -out $(ISONAME)
	$(3DOENCRYPT) genromtags $(ISONAME)

builddir:
	mkdir -p build/

buildgraphicsdir:
	mkdir -p $(FILESYSTEM)/Graphics

build/%.s.o: src/%.s
	$(AS) $(INCPATH) $(ASFLAGS) $< -o $@

build/%.c.o: src/%.c
	$(CC) $(INCPATH) $(CFLAGS) -c $< -o $@

build/%.cpp.o: src/%.cpp
	$(CXX) $(INCPATH) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -vrf $(OBJ) $(EXENAME) $(EXENAME).sym $(ISONAME) $(SYSTEM) $(BANNER_SCREEN) $(BUILD) $(FILESYSTEM)/Graphics

launch: all
	$(RETRO_ARCH) -L  ~/.config/retroarch/cores/opera_libretro.so $(ISONAME) &

copy_iso: all
	$(CP) $(ISONAME) /mnt/d/

.PHONY: clean modbin banner iso
