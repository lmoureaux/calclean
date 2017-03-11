
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
ee.o: ee.cpp calofilter.h ee.h
hb.o: hb.cpp calofilter.h hb.h

libcalofilter.a: calofilter.o calofilter.h logic.h eb.o ee.o hb.o
	$(AR) rcs libcalofilter.a calofilter.o eb.o hb.o

test: test.o libcalofilter.a
	$(CXX) $(CXXFLAGS) test.o libcalofilter.a -o test $(LDFLAGS)

tools: tools/eeanalyzer tools/hbanalyzer tools/heanalyzer

tools/eeanalyzer.o: tools/eeanalyzer.cpp calofilter.h ee.h
tools/hbanalyzer.o: tools/hbanalyzer.cpp calofilter.h hb.h
tools/heanalyzer.o: tools/heanalyzer.cpp calofilter.h he.h

tools/eeanalyzer: tools/eeanalyzer.o libcalofilter.a
	$(CXX) $(CXXFLAGS) tools/eeanalyzer.o libcalofilter.a -o tools/eeanalyzer $(LDFLAGS)
tools/hbanalyzer: tools/hbanalyzer.o libcalofilter.a
	$(CXX) $(CXXFLAGS) tools/hbanalyzer.o libcalofilter.a -o tools/hbanalyzer $(LDFLAGS)
tools/heanalyzer: tools/heanalyzer.o libcalofilter.a
	$(CXX) $(CXXFLAGS) tools/heanalyzer.o libcalofilter.a -o tools/heanalyzer $(LDFLAGS)

doc: doc/html/index.html

doc/html/index.html: *.h *.cpp tools/*.cpp tools/*.md doc/stylesheet.css
	doxygen
