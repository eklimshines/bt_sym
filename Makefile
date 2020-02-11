CROSS_COMPILE = gcc

CURDIR = $(shell pwd)
LIBDIR = $(CURDIR)/lib

CFLAGS = -I$(CURDIR)
#CFLAGS += -I/usr/local/include/pbc
CFLAGS += -I/usr/local/ssl/include
#CFLAGS += -I$(CURDIR)/asn/rtbersrc
#CFLAGS += -w -Wunused
#CFLAGS += -g

LIBS = -L/usr/local/lib
LIBS += -L/usr/local/ssl/lib -Wl,-rpath /usr/local/lib
#LIBS += -lpbc -lgmp
LIBS += -lssl -lcrypto
LIBS += -lbluetooth
LIBS += -lpthread
#LIBS += -lsqlite3 -ldl

SRC	=	main.c\
		util.c\
		aes.c\
		network.c\
		network_server.c\
		network_client.c\
		network_packet.c\
		network_parse.c\
		bluetooth.c\
		bluetooth_l2cap.c\
		sdp.c\
		sym.c\
		save.c\

TARGET = blue_test

$(TARGET) :
	$(CROSS_COMPILE) -o $@ $(SRC) $(CFLAGS) $(DFLAGS)  $(LIBS)
	
clean:
	rm -rf $(TARGET)
	rm -rf *.o


