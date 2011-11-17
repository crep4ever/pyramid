.PHONY: debug
debug:
ifeq ($(wildcard debug),)
	mkdir debug
endif
	(cd debug; cmake -G KDevelop3 ../src -DCMAKE_BUILD_TYPE=Debug)
	(cd debug; make -j 3)

.PHONY: release
release:
ifeq ($(wildcard release),)
	mkdir release
endif
	(cd release; cmake -G KDevelop3 ../src -DCMAKE_BUILD_TYPE=Release)
	(cd release; make)

.PHONY: doc
doc:
ifeq ($(wildcard doc),)
	mkdir doc
endif
	(cd doc; doxygen Doxyfile;)


.PHONY: profile
profile:
ifeq ($(wildcard profile),)
	mkdir profile
endif
	(cd profile; cmake -G KDevelop3 ../src -DCMAKE_BUILD_TYPE=Profile)
	(cd profile; make -j 3)

.PHONY: interactive
interactive: 
	(cd interactive; ccmake ../ -DCMAKE_BUILD_TYPE=Interactive)
	(cd interactive; make)

.PHONY: all
all: debug release 

.PHONY: clean
clean: 
	@rm -f Memory_leaks.log
	@rm -Rf debug
	@rm -Rf release
	@rm -Rf profile
	@rm -Rf interactive
	@mkdir debug
	@mkdir release
	@mkdir profile
	@mkdir interactive

.PHONY: cleanall
cleanall: clean 
	@rm -rf doc/html doc/latex doc/man
	@rm -rf output/ result/ *.tif
	@rm -f callgrind.out.*
