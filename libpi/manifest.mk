#BUILD_DIR := ./objs
#LIB := libpi.a
#START := ./staff-start.o

ifeq ($(USE_FP),1)
    BUILD_DIR := ./fp-objs
    LIB := libpi-fp.a
    LIBM := libm
    START := ./staff-start-fp.o
    STAFF_OBJS := $(foreach o, $(STAFF_OBJS), $(dir $o)/fp/$(notdir $o))
else
    BUILD_DIR := ./objs
    LIB := libpi.a
    START := ./staff-start.o
endif



# set this to the path of your ttyusb device if it can't find it
# automatically
TTYUSB = 

# the string to extract for checking
GREP_STR := 'HASH:\|ERROR:\|PANIC:\|PASS:\|TEST:'

# set if you want the code to automatically run after building.
RUN = 1
# set if you want the code to automatically check after building.
#CHECK = 0

ifdef CS240LX_STAFF
ifndef CS240LX_ACT_AS_STUDENT
STAFF_OBJS += staff-objs/kmalloc.o
STAFF_OBJS += staff-objs/sw-uart.o
STAFF_OBJS += staff-objs/interrupts-asm.o      
STAFF_OBJS += staff-objs/interrupts-vec-asm.o 
STAFF_OBJS += staff-objs/rpi-thread.o 
STAFF_OBJS += staff-objs/rpi-thread-asm.o 
STAFF_OBJS += staff-objs/sw-spi.o
STAFF_OBJS += staff-objs/interrupts-vec-init.o
SRC += $(wildcard ./staff-dev/*.[Sc])

endif
endif

DEPS += ./src


include $(CS240LX_2022_PATH)/libpi/mk/Makefile.lib.template

test:
	make -C  tests check

all:: $(START)

staff-start.o: ./objs/staff-start.o
	cp ./objs/staff-start.o staff-start.o

staff-start-fp.o: ./fp-objs/staff-start.o
	cp ./fp-objs/staff-start.o staff-start-fp.o

clean::
	rm -f staff-start.o
	rm -f staff-start-fp.o
	make -C  tests clean
ifneq ($(USE_FP),1)
	make clean USE_FP=1
	make -C libm clean
endif

.PHONY : libm test libpi-fp
