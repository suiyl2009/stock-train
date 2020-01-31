include ../makeserver.inc

all:            BatchLine.so

OBJS_Utils=     BatchLine.o  

BatchLine.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
