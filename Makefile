
.PHONY: all clean doc

all: test
clean:
	$(RM) *.o
	$(RM) -r doc/html

CXXFLAGS := -pedantic -Wextra -Wall `root-config --cflags` $(CXXFLAGS)
LDFLAGS := `root-config --libs` $(LDFLAGS)

calofilter.o: calofilter.cpp calofilter.h

libcalofilter.a: calofilter.o calofilter.h
	$(AR) rcs libcalofilter.a calofilter.o

test: test.o libcalofilter.a
	$(CXX) $(CXXFLAGS) test.o libcalofilter.a -o test $(LDFLAGS)

doc: doc/html/index.html

doc/html/index.html: *.h *.cpp
	doxygen
