
.PHONY: all clean doc

all: test
clean:
	$(RM) *.o

CXXFLAGS = -pedantic -Wextra -Wall `root-config --cflags`
LDFLAGS = `root-config --libs`

test: test.o calofilter.o
	$(CXX) $(CXXFLAGS) test.o calofilter.o -o test $(LDFLAGS)

doc: doc/latex/refman.pdf

doc/latex/refman.pdf: doc/latex/Makefile
	$(MAKE) -C doc/latex

doc/latex/Makefile: *.h *.cpp
	doxygen
