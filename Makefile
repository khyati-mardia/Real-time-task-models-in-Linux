CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -D _GNU_SOURCE -std=c++11

OBJS =		NewProject.o

LIBS = -pthread  

TARGET =	NewProject

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
