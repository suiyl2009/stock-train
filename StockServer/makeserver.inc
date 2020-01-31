project_home = /mnt/hgfs/project/StockServer

RM		=rm -f
RMDIR		=rm -rf

####
####	Compile flags:
CXX		=g++ 
CC              =gcc

PROC_INCLUDE = -I $(project_home)/MainFrame -I $(project_home)/Utils -I $(project_home)/Load -I $(project_home)/CurrList \
		-I $(project_home)/Single -I $(project_home)/Batch -I $(project_home)/CondList -I $(project_home)/OptionList \
		-I $(project_home)/KLine -I $(project_home)/KTrain -I $(project_home)/DBPool -I $(project_home)/TradePoint \
		-I $(project_home)/User -I $(project_home)/SelfSelected -I $(project_home)/Index 

CXX_FLAG = $(PROC_INCLUDE)
 
##
.SUFFIXES:		.cxx .cpp .c

.cxx.o:
	$(RM) $@
	$(CXX) -shared -Wl, -fPIC $(DEBUG) -c $(CXX_FLAG) -o $@ $<
	
.cpp.o:
	$(RM) $@
	$(CXX) -g -shared -Wl, -fPIC $(DEBUG) -c $(CXX_FLAG) -o $@ $<

.c.o:
	$(RM) $@
	$(CC) -shared -Wl, -fPIC $(DEBUG) -c -o $@ $<

