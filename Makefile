
.PHONY: all clean doc

all: test
clean:
	$(RM) *.o
	$(RM) -r doc/html

CXXFLAGS := -pedantic -Wextra -Wall `root-config --cflags` $(CXXFLAGS)
LDFLAGS := `root-config --libs` $(LDFLAGS)

calofilter.o: calofilter.cpp calofilter.h
eb.o: eb.cpp calofilter.h eb.h

libcalofilter.a: calofilter.o calofilter.h eb.o
	$(AR) rcs libcalofilter.a calofilter.o eb.o

test: test.o libcalofilter.a
	$(CXX) $(CXXFLAGS) test.o libcalofilter.a -o test $(LDFLAGS)

doc: doc/html/index.html

doc/html/index.html: *.h *.cpp doc/stylesheet.css
	doxygen
