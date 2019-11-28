CIRCLEHOME = ../circle

OBJS = demo.o gl.o

LIBS = \
       $(CIRCLEHOME)/lib/libcircle.a \
       $(CIRCLEHOME)/lib/net/libnet.a \
       $(CIRCLEHOME)/lib/usb/libusb.a \
       $(CIRCLEHOME)/lib/fs/libfs.a \
       $(CIRCLEHOME)/lib/input/libinput.a \
       $(CIRCLEHOME)/lib/sched/libsched.a

include $(CIRCLEHOME)/Rules.mk
