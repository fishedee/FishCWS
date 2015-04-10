.PHONY:build install clean
build:
	cd src && $(CXX) fishcws.cpp -ggdb -o ../bin/fishcws -L../lib/ -lpthread -lNLPIR -levent -ljsoncpp -Wall -Wunused -DOS_LINUX -std=c++0x
install:build
	-sudo pkill fishcws
	sudo cp lib/libNLPIR.so /usr/lib/libNLPIR.so
	sudo cp bin/fishcws /usr/bin/fishcws
	-sudo rm /usr/local/fishcws -r
	sudo mkdir /usr/local/fishcws/
	sudo cp data/* /usr/local/fishcws/ -r
run:
	-sudo pkill fishcws
	sudo fishcws -d -x userdict.txt
clean:
	rm bin/fishcws -rf
