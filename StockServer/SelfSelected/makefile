include ../makeserver.inc

all:            SelfSelected.so

OBJS_Utils=     SelfSelected.o  

SelfSelected.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
