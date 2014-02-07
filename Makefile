SSLPP=../sslpp

CDEFS=-D_POSIX_THREAD_SAFE_FUNCTIONS -D_REENTRANT
COPTS=-fexceptions -frtti -fPIC -Wall -Wunused -Wpointer-arith -Wreturn-type
CINCS=-I$(SSLPP)/
LIBS=-L$(SSLPP)/ -lssl -lssl++ -lz
LIB_NAME=libhessian
SHA_LIB=$(LIB_NAME).so
STAT_LIB=$(LIB_NAME).a
DEBUG=#-ggdb3

CXXFLAGS=$(CDEFS) $(COPTS) $(CINCS)


OBJ=zlibdec.o hessian_input.o hessian_output.o string_input_stream.o hessian_proxy.o wrappers.o
BIN=main

all:	$(SHA_LIB) $(STAT_LIB) $(BIN) 

$(BIN):	$(OBJ) $(BIN).o
	$(CXX) $(DEBUG) -o $(BIN) $(BIN).o libhessian.a $(LIBS)
	$(CXX) $(DEBUG) -o $(BIN)_dyn $(BIN).o -L. -lhessian $(LIBS)
	
$(SHA_LIB): $(OBJ)
	$(CXX) $(DEBUG) -shared -o $(SHA_LIB) $(LIBS) $(LDFLAGS) $(OBJ)

$(STAT_LIB): $(OBJ)
	ar rv $(STAT_LIB) $(OBJ)
	-ranlib $(STAT_LIB)
clean:
	rm -f *.o deps core *~ *.d $(SHA_LIB) $(STAT_LIB) $(BIN)

main.o:			main.cpp input_stream.h string_input_stream.h exceptions.h wrappers.h
zlibdec.o:		zlibdec.cpp zlibdec.h
hessian_input.o:	hessian_input.cpp hessian_input.h exceptions.h wrappers.h
hessian_output.o:	hessian_output.cpp hessian_output.h exceptions.h wrappers.h
hessian_proxy.o:	hessian_proxy.cpp exceptions.h hessian_input.h hessian_output.h wrappers.h string_input_stream.h
string_input_stream.o:	input_stream.h string_input_stream.cpp string_input_stream.h
wrappers.o:		wrappers.cpp wrappers.h

