CC = gcc
INCLUDES = -I./src/share -I./src/peer -I./src/helper
PEER_LIBS = -L/usr/local/lib -lpcap -lnet
SHARE_LIBS = -lpthread
CFLAGS = -Wall -Werror -O3 -fno-strict-aliasing 
LIBNET_FLAGS = -D_BSD_SOURCE -D__BSD_SOURCE -D__FAVOR_BSD -DHAVE_NET_ETHERNET_H

SHARE_OBJS = ./src/share/nethelp.o ./src/share/netio.o ./src/share/list.o ./src/share/util.o 

PEER_EXE = peer
PEER_MAIN = ./src/stubs/peer.c
PEER_OBJS = ./src/peer/directconn.o ./src/peer/sniff.o ./src/peer/peercon.o \
./src/peer/peerfsm.o ./src/peer/spoof.o ./src/peer/natblaster_peer.o
PEER_SO=libnatblaster_peer.so

HELPER_EXE = helper
HELPER_MAIN = ./src/stubs/helper.c
HELPER_OBJS = ./src/helper/helpercon.o ./src/helper/connlist.o \
./src/helper/helperfsm.o ./src/helper/natblaster_helper.o
HELPER_SO=libnatblaster_helper.so

DOC = doxygen
DOC_DIR = doc

PRINT = enscript
PRINT_FILE = ps/natblasterv2_src.ps

FILES=./src/helper/*.[ch] ./src/peer/*.[ch] ./src/share/*.[ch] \
./src/stubs/*.[ch]

.PHONY: all both html print clean help

all: both

%.o: %.c 
	$(CC) -c -o $(@) $(CFLAGS) $(INCLUDES) $(LIBNET_FLAGS) $(@:.o=.c)

both: $(PEER_EXE) $(HELPER_EXE)

$(PEER_EXE): $(PEER_SO)
	$(CC) $(PEER_MAIN) -o $@ -L. -lnatblaster_peer -Wl,-rpath,$(shell pwd) $(INCLUDES) $(PEER_LIBS) $(SHARE_LIBS)

$(HELPER_EXE): $(HELPER_SO)
	$(CC) $(HELPER_MAIN) -o $@ -L. -lnatblaster_helper -Wl,-rpath,$(shell pwd) $(INCLUDES) $(SHARE_LIBS)

$(PEER_SO): $(PEER_OBJS) $(SHARE_OBJS)
	$(CC) -shared -fPIC -o $@ $^ 
	
$(HELPER_SO): $(HELPER_OBJS) $(SHARE_OBJS) 
	$(CC) -shared -fPIC -o $@ $^
	
html:
	$(DOC) $(DOC_DIR)/doxygen.cfg

print:
	$(PRINT) -2r -p $(PRINT_FILE) $(FILES)
	@echo "The errorcodes.h file was included in the output,\
	this makes the output longer than really necessary.  Please\
	manually create the postscript file if you do not want the\
	errorcodes file included."
#	'********************************************************'
#	'* This will create a postscript file with all the code *'
#	'* if you have enscript installed on your computer.     *'
#	'********************************************************'

clean:
	rm -f $(PEER_OBJS) $(HELPER_OBJS) $(SHARE_OBJS)
	rm -f $(PEER_EXE) $(HELPER_EXE) 
	rm -f $(PEER_SO) $(HELPER_SO)
	rm -f $(PRINT_FILE)
	rm -rf $(DOC_DIR)/html $(DOC_DIR)/latex $(DOC_DIR)/rtf 
	rm -rf $(DOC_DIR)/man $(DOC_DIR)/xml

help:
	@echo "make all:     compile peer and helper"
	@echo "make both:    same as make all"
	@echo "make peer:    compile the peer (requires libnet/libpcap)"
	@echo "make helper:  compile the helper (no libnet/libpcap required)"
	@echo "make html:    make the doxygen documentation (doxygen required)"
	@echo "make print:   make a postsript file with all the code (enscript required)"
	@echo "make clean:   clean up everything"
