#!/usr/bin/python

import sys
import os
import string

# the basics of the command
basic_command = "sudo ./peer"

#semi-static values
helper_ip     = "server.example.org"
peer_ip       = "192.168.1.100"
buddy_ext_ip  = "buddy.example.com"
buddy_int_ip  = "192.168.1.99"
message       = '"message from peer"'
random        = ""

# "" means no device passed to peer, otherwise the named device is passed
device        = "" 

# dynamic values - set to defaults
peer_port       = 4000
helper_port     = 8000
buddy_int_port  = 3000

# loop over all arguments and create the extraports peer-args
for i in range(1,len(sys.argv)) :
	if (sys.argv[i][len(sys.argv[i])-1] == "p") :
		peer_port = sys.argv[i][:len(sys.argv[i])-1]
	if (sys.argv[i][len(sys.argv[i])-1] == "h") :
		helper_port = sys.argv[i][:len(sys.argv[i])-1]
	if (sys.argv[i][len(sys.argv[i])-1] == "b") :
		buddy_int_port = sys.argv[i][:len(sys.argv[i])-1]
	if (sys.argv[i][0] == "r") :
		random = " --random"

# create the command
command = ""
command += basic_command
command += " --helper_ip "      + helper_ip
command += " --helper_port "    + str(helper_port)
command += " --local_ip "       + peer_ip
command += " --local_port "     + str(peer_port)
command += " --buddy_ext_ip "   + buddy_ext_ip
command += " --buddy_int_ip "   + buddy_int_ip
command += " --buddy_int_port " + str(buddy_int_port)
command += " --message "        + message
command += random
if (device != "") :
	command += " --device " + device

# print the command being executed, FYI
print "run.py executing sudo ./peer with the follow parameters:"
print "   helper IP ................",helper_ip
print "   helper port ..............",helper_port
print "   peer IP ..................",peer_ip
print "   peer port ................",peer_port
print "   buddy external IP ........",buddy_ext_ip
print "   buddy internal IP ........",buddy_int_ip
print "   buddy internal port ......",buddy_int_port
if (device != "" ) :
	print "   device ...................",device
print "   message ..................",message
if (random != "" ) :
	print "   random ................... yes"
else :
	print "   random ................... no"

# execute the command
#os.system(command)
os.system(command + " >& run.py.tmp")
fid = open("run.py.tmp","r")
print fid.read()
fid.close()

