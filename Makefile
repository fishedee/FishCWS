.PHONY:build install clean
build:
	cd src && $(CXX) fishcws.cpp -o ../bin/fishcws -L../lib/ -lpthread -lNLPIR -levent -ljsoncpp -Wall -Wunused -O3 -DOS_LINUX -std=c++0x
install:build
	sudo cp lib/libNLPIR.so /usr/lib/
	sudo cp bin/fishcws /usr/bin/
	sudo rm /usr/local/fishcws -r
	sudo mkdir /usr/local/fishcws/
	sudo cp Data /usr/local/fishcws/ -r
run:
	bin/fishcws -x userdict.txt
clean:
	rm bin/fishcws -rf
