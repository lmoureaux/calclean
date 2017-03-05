
.PHONY: all clean doc tools

all: test tools
clean:
	$(RM) *.o
	$(RM) -r doc/html

CXXFLAGS := -I. $(CXXFLAGS) -O2
CXXFLAGS := -pedantic -Wextra -Wall `root-config --cflags` $(CXXFLAGS)
LDFLAGS := `root-config --libs` $(LDFLAGS)

calofilter.o: calofilter.cpp calofilter.h
eb.o: eb.cpp calofilter.h eb.h
hb.o: hb.cpp calofilter.h hb.h

libcalofilter.a: calofilter.o calofilter.h logic.h eb.o hb.o
	$(AR) rcs libcalofilter.a calofilter.o eb.o hb.o

test: test.o libcalofilter.a
	$(CXX) $(CXXFLAGS) test.o libcalofilter.a -o test $(LDFLAGS)

tools: tools/hbanalyzer

tools/hbanalyzer.o: tools/hbanalyzer.cpp calofilter.h hb.h

tools/hbanalyzer: tools/hbanalyzer.o libcalofilter.a
	$(CXX) $(CXXFLAGS) tools/hbanalyzer.o libcalofilter.a -o tools/hbanalyzer $(LDFLAGS)

doc: doc/html/index.html

doc/html/index.html: *.h *.cpp tools/*.cpp tools/*.md doc/stylesheet.css
	doxygen
