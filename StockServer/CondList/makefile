include ../makeserver.inc

all:            CondList.so

OBJS_Utils=     CondList.o  

CondList.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
