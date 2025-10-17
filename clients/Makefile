PRODUCT = news
PLATFORMS += adam
PLATFORMS += apple2
PLATFORMS += atari-pas
PLATFORMS += c16
PLATFORMS += c64
PLATFORMS += coco
PLATFORMS += msdos
PLATFORMS += vic20

# You can run 'make <platform>' to build for a specific platform,
# or 'make <platform>/<target>' for a platform-specific target.
# Example shortcuts:
#   make coco        → build for coco
#   make apple2/disk → build the 'disk' target for apple2

# SRC_DIRS may use the literal %PLATFORM% token.
# It expands to the chosen PLATFORM plus any of its combos.
SRC_DIRS = src src/%PLATFORM%

# FUJINET_LIB can be
# - a version number such as 4.7.6
# - a directory which contains the libs for each platform
# - a zip file with an archived fujinet-lib
# - a URL to a git repo
# - empty which will use whatever is the latest
# - undefined, no fujinet-lib will be used
FUJINET_LIB = 

# Some platforms don’t use FUJINET_LIB; set this to allow builds to continue
# even if the library isn’t present.
FUJINET_LIB_OPTIONAL = 1

# Define extra dirs ("combos") that expand with a platform.
# Format: platform+=combo1,combo2
PLATFORM_COMBOS = \
  c64+=commodore \
  atari-pas+=atari \
  atarixe+=atari \
  msxrom+=msx \
  msxdos+=msx

include makefiles/toplevel-rules.mk

########################################
# Common things

CFLAGS = -DBUILD_$(PLATFORM_UC)
CFLAGS_EXTRA_CC65 = -Os
CFLAGS_EXTRA_Z88DK = -Os

########################################
# VIC-20 customization

LDFLAGS_EXTRA_VIC20 = -C src/vic20/cart.cfg

########################################
# C16 customization

LDFLAGS_EXTRA_C16 = -C src/c16/c16.cfg

########################################
# C64 customization

LDFLAGS_EXTRA_C16 = -C src/c64/c64.cfg
