#
# $Id: GNUmakefile,v 1.3 2008/08/26 21:20:33 brebel Exp $
#
# brebel@fnal.gov
#
include SoftRelTools/arch_spec_root.mk

PACKAGE_INCLUDE := .
SUBDIRS     := uboone_datatypes utils 
LIB_TYPE    := shared
LIB         := lib$(PACKAGE)
LIBCXXFILES := $(wildcard *.cxx)

LIBLINK    := -L$(SRT_PRIVATE_CONTEXT)/lib/$(SRT_SUBDIR) -L$(LARSOFT)/lib/$(SRT_SUBDIR) -l$(PACKAGE)

########################################################################
include SoftRelTools/standard.mk
include SoftRelTools/arch_spec_art.mk

override LIBLIBS += $(LOADLIBES)
