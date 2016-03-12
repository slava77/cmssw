all: generate_misalignment

generate_misalignment: generate_misalignment.cc
	g++ `root-config --libs` -lMinuit `root-config --cflags` generate_misalignment.cc -o generate_misalignment
