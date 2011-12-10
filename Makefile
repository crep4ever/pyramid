builddir=.build-$(shell hostname)-$(shell gcc -dumpmachine)-$(shell gcc -dumpversion)

ifeq (,$(VERBOSE))
    MAKEFLAGS:=$(MAKEFLAGS)s
    ECHO=echo
else
    ECHO=@:
endif

TARGETS=pyramid noqt
BUILDLN=build

all: $(TARGETS) $(BUILDLN) ;

$(TARGETS): cmake-build
	ln -s -f ${builddir}/$@ $@

$(BUILDLN):
	test -e $(BUILDLN) || ln -s -f ${builddir} $(BUILDLN)

cmake ${builddir}/CMakeCache.txt:
	mkdir -p ${builddir}
	$(ECHO) "Running cmake "
	cd ${builddir} && cmake $(CMAKE_ARGS) "$(@D)" ..

cmake-build: ${builddir}/CMakeCache.txt
	$(ECHO) "Building "
	$(MAKE) -C ${builddir}

install:
	$(ECHO) "Installing "
	$(MAKE) -C ${builddir} install

distclean:
	$(ECHO) -n "Cleaning up build directory "
	$(RM) -r ${builddir} $(BUILDLN) $(TARGETS)
	$(ECHO) " done"

%: cmake
	$(ECHO) "Running make $@ "
	$(MAKE) -C ${builddir} $@
	$(and $(filter clean,$@),$(RM) $(BUILDLN) $(TARGETS))

.PHONY: cmake-build cmake install $(BUILDLN)
