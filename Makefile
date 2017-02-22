
.PHONY: all clean doc

all: test
clean:
	$(RM) *.o

CXXFLAGS := -pedantic -Wextra -Wall `root-config --cflags` $(CXXFLAGS)
LDFLAGS := `root-config --libs` $(LDFLAGS)

libcalofilter.a: calofilter.o calofilter.h
	$(AR) rcs libcalofilter.a calofilter.o

test: test.o libcalofilter.a
	$(CXX) $(CXXFLAGS) test.o libcalofilter.a -o test $(LDFLAGS)

doc: doc/latex/refman.pdf

doc/latex/refman.pdf: doc/latex/Makefile
	$(MAKE) -C doc/latex

doc/latex/Makefile: *.h *.cpp
	doxygen
