include ../makeserver.inc

all:            KTrain.so

OBJS_Utils=     KTrain.o  

KTrain.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
