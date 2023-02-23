.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

SRCDIR=Srcs
OBJDIR=Objs
BINDIR=Bin
HEADDIR=Headers

CC=gcc
CFLAGS=-Wall -Werror -g
CPPFLAGS=-I$(HEADDIR)
#VPATH=src/

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
#LIBS += -lsocket -lnsl -lrt
LIBS+=-lpthread

ifdef DEBUG
	CPPFLAGS+=-DDEBUG
endif

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
EXEC = $(BINDIR)/shell

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $^ -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean :
	-@rm -r $(OBJS) $(SRCDIR)/*~ $(EXEC) 2>/dev/null || true
	@echo All is removed