include ../makeserver.inc

all:            CurrList.so

OBJS_Utils=     CurrList.o  

CurrList.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
