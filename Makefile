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

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
#LIBS += -lsocket -lnsl -lrt
LIBS+=-lpthread

ifdef DEBUG
	CPPFLAGS+=-DDEBUG
endif

ifdef TEST
	CPPFLAGS+=-DTEST
endif

ifdef MAIN2
	EXEC = $(BINDIR)/shell2
	SRCS = $(wildcard $(SRCDIR)/*[^13].c)
	OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
else ifdef MAIN3
	EXEC = $(BINDIR)/shell3
	SRCS = $(wildcard $(SRCDIR)/*[^12].c)
	OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
else
	EXEC = $(BINDIR)/shell1
	SRCS = $(wildcard $(SRCDIR)/*[^23].c)
	OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
endif

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $^ -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean :
	-@rm -r $(OBJDIR)/*.o $(SRCDIR)/*~ $(BINDIR)/* 2>/dev/null || true
	@echo All is removed
