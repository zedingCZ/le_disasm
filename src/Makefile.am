bin_PROGRAMS = swdisasm
bindir = $(prefix)/usr/$(PACKAGE)

swdisasm_SOURCES = \
	disassembler.h \
	disassembler.cpp \
	instruction.h \
	instruction.cpp \
	image.h \
	image.cpp \
	le.h \
	le.cpp \
	le_image.h \
	le_image.cpp \
	swdisasm.cpp \
	util.h \
	util.cpp

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

# Include dynamic libraries in the package
if TARGET_WINDOWS
install-exec-hook:
	mkdir -p "$(DESTDIR)${prefix}/usr/$(PACKAGE)"
	## Find main executable dependencies
	$(eval lib_SHARED_INSTALL := $(shell objdump -p swdisasm$(EXEEXT) | \
		sed -n 's/\s*\(DLL Name:\|NEEDED\)\s*\(.*\)$$/\2/p' | \
		xargs -I {} find $(shell dirname $(shell which ${CXX})) -name '{}'))
	## Find sub-dependencies for the previous dependencies
	$(eval lib_SHARED_INSTALL += $(shell echo -n '$(lib_SHARED_INSTALL)' | \
        xargs -d ' ' -I {} objdump -p '{}' | \
		sed -n 's/\s*\(DLL Name:\|NEEDED\)\s*\(.*\)$$/\2/p' | \
		xargs -I {} find $(shell dirname $(shell which ${CXX})) -name '{}'))
	## Remove duplicates
	$(eval lib_SHARED_INSTALL := $(sort $(lib_SHARED_INSTALL)))
	cp $(lib_SHARED_INSTALL) $(DESTDIR)${prefix}/usr/$(PACKAGE)
endif


DISTCLEANFILES = *~
