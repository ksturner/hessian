CDEFS=-D_POSIX_THREAD_SAFE_FUNCTIONS -D_REENTRANT
CFLAGS=-I/opt/local/include -O2 -fPIC -frtti -fexceptions -Wunused -Wpointer-arith -Wreturn-type -Wall
CXXFLAGS=$(CDEFS) $(CFLAGS)
LIBS=-lssl -lcrypto

OBJ=http_connection.o http_exception.o ssl_connection.o socket_connection.o
LIB_NAME=libssl++
SHA_LIB=$(LIB_NAME).so
OUT_HDR=*.h
PREFIX=/usr/local

all: $(SHA_LIB)

%.o: %.cpp
	g++ $(CXXFLAGS) -c $<

$(SHA_LIB): $(OBJ)
	g++ -g -shared -o $(SHA_LIB) $(LIBS) $(LDFLAGS) $(OBJ)

client: install
	g++ -g -L$(PREFIX)/libsslpp/lib -I$(PREFIX)/libsslpp/include client.cpp -o client -lssl++

install: $(SHA_LIB)
	@echo installing ssl++ to $(PREFIX)/libsslpp lib/ and include/
	@mkdir -p $(PREFIX)/libsslpp/lib
	@mkdir -p $(PREFIX)/libsslpp/include
	@cp -f $(SHA_LIB) $(PREFIX)/libsslpp/lib
	@cp -f $(OUT_HDR) $(PREFIX)/libsslpp/include

clean:
	rm -f *.o deps core *~ *.d $(SHA_LIB) client
