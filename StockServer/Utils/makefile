include ../makeserver.inc

all:            utils.so

OBJS_Utils=     Config.o StrSplit.o convert.o TravelDir.o IniFile.o Calendar.o 

utils.so: $(OBJS_Utils)
	$(RM) $@
	$(CXX) -shared -fPIC -o $@ $(OBJS_Utils)

clean:
	$(RM) core.* *.so  *.o testCal 

debug:
	make DEBUG='-D_DEBUG -g'

testCal: testCal.o utils.so
	$(CXX) -g -o testCal testCal.o utils.so


