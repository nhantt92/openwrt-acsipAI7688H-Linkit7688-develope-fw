BUILD_BASE  = build
TARGER_BASE = bin

TARGET ?= hellomake

SOURCE = src lib

INCLUDE = "src\include" "lib\include"

export STAGING_DIR=
TOOLCHAIN_PATH ?= "/home/nhantt/Desktop/OpenWrt/OpenWrt-SDK/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/"
LIB_PATH = "/home/nhantt/Desktop/OpenWrt/OpenWrt-SDK/staging_dir/"

CC := $(TOOLCHAIN_PATH)/mipsel-openwrt-linux-uclibc-gcc
LD := $(TOOLCHAIN_PATH)/mipsel-openwrt-linux-uclibc-gcc

CC_FLAGS = -I.
CC_FLAGS += -I $(LIB_PATH)/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/include
CC_FLAGS += -I $(LIB_PATH)/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/include

LD_FLAGS = -L.
LD_FLAGS += -L $(LIB_PATH)/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/lib
LD_FLAGS += -L $(LIB_PATH)/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/lib
LD_FLAGS += -lmraa -pthread -ldl


BUILD_DIR := $(addprefix $(BUILD_BASE)/,$(SOURCE))

SRC   := $(foreach sdir,$(SOURCE),$(wildcard $(sdir)/*.c))
OBJ   := $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC))
LIBS  := $(addprefix -l,$(LIBS))

INCDIR  := $(addprefix -I,$(SOURCE))
INCDIR  := $(addsuffix /include,$(INCDIR))

EXTRA_INCDIR  := $(addprefix -I,$(INCLUDE))
BIN_OUT       := $(addprefix $(TARGER_BASE)/,$(TARGET).run)

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif


vpath %.c $(SOURCE)

define compile-objects
$1/%.o: %.c
		$(vecho)  "CC $$<"
		$(Q) $(CC) $(INCDIR) $(EXTRA_INCDIR) $(CC_FLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs $(BIN_OUT)

$(BIN_OUT): $(OBJ)
	$(vecho) "LD $@"
	$(Q) $(LD) $(OBJ) $(LD_FLAGS) -o $@

checkdirs: $(BUILD_DIR) $(TARGER_BASE)

$(BUILD_DIR):
	$(Q) mkdir -p $@
$(TARGER_BASE):
	$(Q) mkdir -p $@

clean:
	rm -rf $(TARGER_BASE) $(BUILD_DIR)
flash:
	#scp ./bin/*.run root@mylinkit.local: #enter password
	sshpass -p nhantt1904 scp ./bin/*.run root@mylinkit.local:

$(foreach bdir,$(BUILD_DIR),$(eval $(call compile-objects,$(bdir))))