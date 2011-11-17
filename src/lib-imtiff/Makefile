.PHONY: debug
debug:
ifeq ($(wildcard debug),)
	mkdir debug
endif
	(cd debug; cmake ../ -DCMAKE_BUILD_TYPE=Debug)
	(cd debug; make)

.PHONY: release
release:
ifeq ($(wildcard release),)
	mkdir release
endif
	(cd release; cmake ../ -DCMAKE_BUILD_TYPE=Release)
	(cd release; make)

.PHONY: all
all: debug release 

.PHONY: interactive
interactive: 
	(cd debug; ccmake ../ -DCMAKE_BUILD_TYPE=Debug)
	(cd debug; make)


.PHONY: clean
clean: 
	@rm -Rf debug
	@rm -Rf release
	@mkdir debug
	@mkdir release

.PHONY: cleanall
cleanall: clean 
	@rm -rf doc/html doc/latex doc/man
