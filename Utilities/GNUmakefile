# Compile the utility programs in this package.

include SoftRelTools/arch_spec_root.mk
include SoftRelTools/arch_spec_pgsql.mk

LIB_TYPE        := shared
LIB             := lib$(PACKAGE)
LIBCXXFILES     := $(wildcard *.cxx)
JOBFILES        := $(wildcard *.fcl)
SUBDIRS         := test

LIBLINK    := -L$(SRT_PRIVATE_CONTEXT)/lib/$(SRT_SUBDIR) -L$(LARSOFT)/lib/$(SRT_SUBDIR) -l$(PACKAGE)  -L${PGSQL_DIR}/lib -lpq

########################################################################
include SoftRelTools/standard.mk
include SoftRelTools/arch_spec_art.mk
########################################################################

override LIBLIBS += $(LOADLIBES) -L$(SRT_PRIVATE_CONTEXT)/lib/$(SRT_SUBDIR) -L$(LARSOFT)/lib/$(SRT_SUBDIR) -lGeometry
