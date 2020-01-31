include ../makeserver.inc

all:            load.so

OBJS=     DBData.o Data.o OutInfo.o IndexData.o   

load.so: $(OBJS)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
