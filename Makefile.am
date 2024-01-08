ACLOCAL_AMFLAGS = --install -I m4

SUBDIRS = src

# Do not install static libraries.
install installdirs: SUBDIRS = src

doc_DATA = COPYING
docdir = $(prefix)/usr/$(PACKAGE)

EXTRA_DIST = LICENSE res
