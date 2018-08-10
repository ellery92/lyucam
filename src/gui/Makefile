all: empty

empty: ../genicam.i ../genicam.h
	swig -c++ -python ../genicam.i
	cd ../ && python genicam-setup.py build_ext --inplace
	cp ../_genicam.cpython* .
	cp ../genicam.py .
	cp ../../rc_genicam_api/rc_genicam_api/librc_genicam_api* /usr/local/lib
