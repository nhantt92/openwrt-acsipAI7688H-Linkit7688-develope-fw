# Build target executable when user executes "make"
TARGET=mraa_blink

export STAGING_DIR=home/nhantt/Desktop/OpenWrt/Project
CC=/home/nhantt/Desktop/OpenWrt/OpenWrt-SDK/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-uclibc-gcc
CFLAGS =-I.
CFLAGS+=-I /home/nhantt/Desktop/OpenWrt/OpenWrt-SDK/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/include
CFLAGS+=-I /home/nhantt/Desktop/OpenWrt/OpenWrt-SDK/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/include
LDFLAGS =-L.
LDFLAGS+=-L /home/nhantt/Desktop/OpenWrt/OpenWrt-SDK/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/lib
LDFLAGS+=-L /home/nhantt/Desktop/OpenWrt/OpenWrt-SDK/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/lib
LDFLAGS+=-lmraa -pthread -ldl

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET).run $(TARGET).c $(LDFLAGS)

# remove object files and executable when user executes "make clean"
clean:
	rm $(TARGET).run

