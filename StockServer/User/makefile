include ../makeserver.inc

all:            User.so

OBJS_Utils=     Mail.o User.o  

User.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o

debug:
	make DEBUG='-D_DEBUG -g'
