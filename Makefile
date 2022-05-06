INCLUDE	:= ./include
OUTDIR	:= ./src/out

#---------------------------------------
# Target
#---------------------------------------
TARGET = ./lib/liblightipc.a

include Makefile.inc

OBJS := $(SRCS:%.cpp=$(OUTDIR)/%.o)
DEPS := $(SRCS:%.cpp=$(OUTDIR)/%.d)
	
#---------------------------------------
# Include +
#---------------------------------------
INCLUDE += \


#---------------------------------------
# Libraly
#---------------------------------------
LIB_DIR =
LIBS	=


#---------------------------------------
# Common
#---------------------------------------
CC			= @echo compiling $< && ccache g++
LINK		= @echo linking $@ && ccache g++
AR			= @echo archiving $@ && ar

DEFINES		+= 
CFLAGS		= -std=c++0x -pipe -g -O2 -Wall -Wno-unused-function -Wno-deprecated-declarations -fPIC $(DEFINES)
ARFLAGS		= cqs
LFLAGS		= -O1 -Wl

#---------------------------------------
# Compile
#---------------------------------------
.SUFFIXES : .cpp .o .h

all:	$(TARGET)

-include $(DEPS)

$(OUTDIR)/%.o:%.cpp
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ -MMD -MP -MF $(@:%.o=%.d) $<
		
$(TARGET): $(OBJS)  
	$(AR) $(ARFLAGS) $(TARGET) $(OBJS) $(LIB_DIR) $(LIBS)

clean:
	rm -f $(TARGET) *~ core *.core
	rm -rf $(OUTDIR)
					
depend:
	makedepend -Y $(DEFS) $(INCLUDE) -- $(SRCS)

