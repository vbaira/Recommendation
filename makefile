CXX=g++
CXXFLAGS=-g -std=c++11
BIN=recommendation
OBJDIR=obj


SRC= \
	main.cpp \
	recommendation.cpp \
	utility.cpp \
	./clustering/cluster_init.cpp \
	./clustering/cluster_assignment.cpp \
	./clustering/cluster_update.cpp \
	./clustering/clustering.cpp \
	./clustering/lsh_hypercube/cube.cpp \
	./clustering/lsh_hypercube/lsh.cpp \
	./clustering/lsh_hypercube/h_family.cpp


OBJ = $(patsubst %.cpp,%.o,$(SRC))

all: $(OBJ)
	$(CXX) $(OBJDIR)/*.o -o $(BIN)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $(OBJDIR)/$(@F)

clean:
	rm -f $(OBJDIR)/*.o
