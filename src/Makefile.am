bin_PROGRAMS = swdisasm
bindir = $(prefix)/usr/$(PACKAGE)

swdisasm_SOURCES = \
	disassembler.h \
	disassembler.cc \
	image.h \
	image.cc \
	le.h \
	le.cc \
	le_image.h \
	le_image.cc \
	swdisasm.cc \
	util.h \
	util.cc

swdisasm_CPPFLAGS = 

swdisasm_RCFLAGS = \
  -I"$(top_srcdir)/src" -I"$(builddir)" \
  $(RCFLAGS)

if HAS_WINDRES
swdisasm_RESRCS = swdisasm_stdres.res
else
swdisasm_RESRCS =
endif

swdisasm_LDADD = \
  $(swdisasm_RESRCS)

if HAS_WINDRES
%.res: $(top_srcdir)/res/%.rc
	$(WINDRES) $(swdisasm_RCFLAGS) -i $< -J rc -o $@ -O coff
endif

DISTCLEANFILES = *~
