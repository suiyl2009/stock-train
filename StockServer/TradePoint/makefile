include ../makeserver.inc

all:            TradePoint.so

OBJS_Utils=     TradePoint.o  

TradePoint.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
