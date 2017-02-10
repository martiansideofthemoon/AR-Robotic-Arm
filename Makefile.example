

OBJ1=ik.o
OBJ2=ar_context.o ar_gl_interface.o
OBJ3=gl_interface.o

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CC=g++
	ARTOOLKIT_ROOT=/home/kalpesh/Projects/artoolkit5
	ARTK_INC_DIR=$(ARTOOLKIT_ROOT)/include
	ARTK_LIB_DIR=$(ARTOOLKIT_ROOT)/lib
	EIGEN_INC_DIR=/usr/include/eigen3

	ARTK_LDFLAGS=-L/usr/lib/x86_64-linux-gnu -L/usr/lib64 -L$(ARTK_LIB_DIR)/linux-x86_64 -L$(ARTK_LIB_DIR)
	ARTK_LIBS=-lARgsub_lite -lARvideo -lAR -lARICP -lAR  -lX11 -lpthread -ljpeg
	GL_LIBS=-lglut -lGLU -lGL -lm
	ARTK_CFLAGS=-O3 -fPIC -DHAVE_NFT=1 -Wno-write-strings -I$(ARTK_INC_DIR) -I$(EIGEN_INC_DIR)
	IK_CFLAGS= -O3 -fPIC -I$(EIGEN_INC_DIR)
endif
ifeq ($(UNAME_S),Darwin)
	CC=c++
	ARTOOLKIT_ROOT=/home/kalpesh/Projects/artoolkit5
	ARTK_INC_DIR=$(ARTOOLKIT_ROOT)/include
	ARTK_LIB_DIR=$(ARTOOLKIT_ROOT)/lib
	EIGEN_INC_DIR=/usr/local/include/eigen3

	ARTK_LDFLAGS=-L$(ARTK_LIB_DIR)
	ARTK_LIBS=-lARgsub_lite -lARvideo -lAR -lARICP -lAR2
	GL_LIBS=-framework Accelerate -framework QTKit -framework CoreVideo -framework Carbon -framework GLUT -framework OpenGL -framework Cocoa -framework QuickTime -L/usr/local/lib -ljpeg
	ARTK_CFLAGS= -stdlib=libc++ -O -DHAVE_NFT=1 -Wno-write-strings -Wno-deprecated-declarations -I $(ARTK_INC_DIR) -I $(EIGEN_INC_DIR)
	IK_CFLAGS= -O -Wno-deprecated-declarations -I$(EIGEN_INC_DIR)
endif



.SUFFIXES:.cpp .o .hpp .c

all: ARApp IKApp

.cpp.o:
	$(CC) $(ARTK_CFLAGS) -c -o $@ $*.cpp

ARApp: ARApp.cpp $(OBJ1) $(OBJ2)
	$(CC) ARApp.cpp $(OBJ2) $(OBJ1) $(ARTK_CFLAGS) $(ARTK_LDFLAGS) $(ARTK_LIBS) $(GL_LIBS) -o ARApp

IKApp: IKApp.cpp $(OBJ3) $(OBJ1)
	$(CC) IKApp.cpp $(OBJ3) $(OBJ1) $(IK_CFLAGS) $(GL_LIBS) -o IKApp

clean:
	rm -f *.o core *~

distclean: clean
	rm -f IKApp ARApp
