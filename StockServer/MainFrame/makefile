include ../makeserver.inc

SO_STOCK = load.so DBPool.so CurrList.so SingleLine.so BatchLine.so CondList.so OptionList.so KLine.so KTrain.so TradePoint.so utils.so \
	   User.so SelfSelected.so Index.so  

all: StockServer

StockServer: StockServer.o Req.o $(SO_STOCK) 
	$(CXX) -g -o StockServer StockServer.o Req.o $(SO_STOCK) -fPIC -lfcgi -lpthread -lmysqlclient -lmysqlcppconn -ljansson -ldl 

utils.so:
	cd ../Utils;make clean;make 
	cp ../Utils/utils.so .

load.so:
	cd ../Load;make clean;make
	cp ../Load/load.so .

CurrList.so:
	cd ../CurrList;make clean;make
	cp ../CurrList/CurrList.so .

SingleLine.so:
	cd ../Single;make clean;make
	cp ../Single/SingleLine.so .

BatchLine.so:
	cd ../Batch;make clean;make
	cp ../Batch/BatchLine.so .

CondList.so:
	cd ../CondList;make clean;make
	cp ../CondList/CondList.so .

OptionList.so:
	cd ../OptionList;make clean;make
	cp ../OptionList/OptionList.so .

KLine.so:
	cd ../KLine;make clean;make
	cp ../KLine/KLine.so .

KTrain.so:
	cd ../KTrain;make clean;make
	cp ../KTrain/KTrain.so .

DBPool.so:
	cd ../DBPool;make clean;make
	cp ../DBPool/DBPool.so .

TradePoint.so:
	cd ../TradePoint;make clean;make 
	cp ../TradePoint/TradePoint.so .  

User.so:
	cd ../User;make clean;make
	cp ../User/User.so .

SelfSelected.so:
	cd ../SelfSelected;make clean;make
	cp ../SelfSelected/SelfSelected.so .
Index.so:
	cd ../Index;make clean;make
	cp ../Index/Index.so .
 
debug: 
	make "CXX += -dDebug"
 
clean:
	$(RM) *.o StockServer *.so 
	cd ../Utils;make clean
	cd ../Load;make clean
	cd ../CurrList;make clean
	cd ../Single;make clean
	cd ../Batch;make clean
	cd ../CondList;make clean
	cd ../OptionList;make clean
	cd ../KLine;make clean
	cd ../KTrain;make clean
	cd ../DBPool;make clean
	cd ../TradePoint;make clean
	cd ../SelfSelected;make clean
	cd ../Index;make clean 
