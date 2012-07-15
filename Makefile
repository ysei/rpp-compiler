all: rpp

clean:
	cd src/ && make clean

rpp: FORCE
	cd src && make all

FORCE: