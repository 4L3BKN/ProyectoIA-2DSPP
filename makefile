EXEC = 2dspp

CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall

SRC = main.cpp

all: $(EXEC)

$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRC)

clean:
	rm -f $(EXEC) *.o
