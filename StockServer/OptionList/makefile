include ../makeserver.inc

all:            OptionList.so

OBJS_Utils=     OptionList.o  

OptionList.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
