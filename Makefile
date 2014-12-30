CC=g++
CFLAGS=-c -Wall -fopenmp
#CFLAGS=-c -Wall -Wextra -Wcast-qual -Wdisabled-optimization -Wdiv-by-zero -Wdouble-promotion -Wendif-labels -Wformat-extra-args -Wformat-nonliteral -Wformat-security -Wformat-y2k -Winit-self -Winline -Winvalid-pch -Wlogical-op -Werror=missing-braces -Wmain -Wmissing-declarations -Wno-missing-format-attribute -Wmissing-braces -Wmissing-include-dirs -Wmultichar -Wpacked -Wpointer-arith -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-default -Wunused-parameter -Werror=undef -Wno-unused -Wvariadic-macros -Wwrite-strings -Werror=declaration-after-statement -Werror=implicit-function-declaration -Werror=nested-externs -Werror=old-style-definition -Werror=strict-prototypes -fpermissive
LDFLAGS=-fopenmp
SOURCES=ttt.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=m_ttt

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)
