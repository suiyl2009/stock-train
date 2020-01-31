include ../makeserver.inc

all:            KLine.so

OBJS_Utils=     KLine.o  

KLine.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
