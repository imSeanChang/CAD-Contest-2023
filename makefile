all:
CXX = g++
LINKFLAGS = -O3 -std=c++11

cadd0020:
	$(CXX) $(LINKFLAGS) main.cpp legalize.cpp -o cadd0020_alpha


clean:
	rm cadd0020_alpha