TITLE_COLOR = \033[33m
NO_COLOR = \033[0m

# ************************************** START OF ORIGINAL TESTS ***********************************************

# Note that the only thing that was changed in the build target is the DSET_MAX_TEMP and DSET_MIN_TEMP (so that both "too hot" and "too cold" messages can be displayed in gateway.log)
all: sensor_gateway sensor_node file_creator

run_test3:
	./test3.sh

run_test5:
	./test5.sh

# ************************************** END OF ORIGINAL TESTS *************************************************

# ************************************** START OF MARTIN'S TESTS ***********************************************


# ******* PERFORMANCE AND SAFETY TESTING OF GATEWAY *******

# The following tests have been designed, performed and completed after ensuring correct functionality of the gateway under the conditions of test3.sh and test5.sh
# All the following tests, except Miscellaneous 1, give the error "sensor_gateway: process not found" at the end. This is expected, because the gateway process should indeed end once the sensor_nodes disconnect.

# -> Note that the " sleep() " in the sensor_node line 106 has been changed to " usleep() ", hence why the large numbers displayed in argv[2] of the sensor nodes execution below.

# 1 )

# ULTRA-SHORT EXECUTION: All sleep times are reduced to 1ms.

# For this test, I do not focus on the accuracy of data insertions at all.
# Rather, I evaluate whether the appropriate set-up for data recording is established effectively and efficiently.
# To verify this, I check the following logs: data.csv opened and closed, sensor node connections opened and closed. I also check whether the runtime error "sensor_gateway no process found"
run_fast_connect: sensor_gateway sensor_node
	./sensor_gateway 5678 6 & \
	sleep 0.001s; \
	./sensor_node 15 100000 127.0.0.1 5678 & \
	sleep 0.001s; \
	./sensor_node 21 100000 127.0.0.1 5678 & \
	sleep 0.001s; \
	./sensor_node 37 100000 127.0.0.1 5678 & \
	sleep 0.001s; \
	./sensor_node 112 100000 127.0.0.1 5678 & \
	sleep 0.001s; \
	./sensor_node 129 100000 127.0.0.1 5678 & \
	sleep 0.001s; \
	./sensor_node 132 100000 127.0.0.1 5678 & \
	sleep 0.001s; \
	killall sensor_node; \
	sleep 0.001s; \
	killall sensor_gateway

# 2 )

# LONG-RUN EXECUTION : I let my sensor_gateway run extensively by setting a long sleep time between the last sensor connection and the killall sensor_node.

# I also run valgrind on this (see below). Finally, I make the sensors record at a moderate frequency (500ms).
# Parameters for success: No significant loss of data, no memory leaks and no errors. The average data loss is around 1%.

run_long: sensor_gateway sensor_node
	./sensor_gateway 5678 5 & \
	sleep 1; \
	./sensor_node 15 500000 127.0.0.1 5678 & \
	sleep 0.01s; \
	./sensor_node 21 500000 127.0.0.1 5678 & \
	sleep 0.01s; \
	./sensor_node 37 500000 127.0.0.1 5678 & \
	sleep 0.01s; \
	./sensor_node 49 500000 127.0.0.1 5678 & \
	sleep 0.01s; \
	./sensor_node 112 500000 127.0.0.1 5678 & \
	sleep 60; \
	killall sensor_node; \
	sleep 0.001s; \
	killall sensor_gateway


# 3 )

# HIGH-FREQUENCY MEASUREMENTS AND CONNECTIONS : All 5 sensor nodes connect to the gateway within 50ms, and each one performs a temperature recording every 10ms.

# Not all readings are recorded in the CSV. To quantify the error, I increased the sleep time between the last node connection and the killall "sensor_nodes" by steps of 0.5s, from 1s till 3s. Thus letting this high-frequency measurement run for longer.
# At each iteration, I divided the amount of recordings in the csv by the amount of readings the sensor performed. Then, I went back to 1s and completed the whole experiment 2 more times.
# I get a data loss of about 6% per execution for this frequency. Note that the error reduces significantly with lower frequencies, as is shown by the "long run" test above.

run_high_freq: all
	 ./sensor_gateway 5678 5 & \
	sleep 1s; \
	./sensor_node 15 10000 127.0.0.1 5678 & \
	sleep 0.01s; \
	./sensor_node 21 10000 127.0.0.1 5678 & \
	sleep 0.01s; \
	./sensor_node 37 10000 127.0.0.1 5678 & \
	sleep 0.01s; \
	./sensor_node 112 10000 127.0.0.1 5678 & \
	sleep 0.01s; \
    ./sensor_node 129 10000 127.0.0.1 5678 & \
	sleep 1s; \
	killall sensor_node; \
	sleep 0.001s; \
	killall sensor_gateway

# 4 )

# MISCELLANEOUS 1

# Parameters for success: The code works under the following ambiguous conditions.
# -> No sleep between connections
# -> Less connections established than the max connections that can be handled by the sensor gateway (this means the connmgr server thread keeps listening for an incoming connection, and thus that the gateway is force killed by the makefile target. "sensor gateway: no process found" error will not occur)
# -> Sensor with invalid Sensor ID 10 connected
# -> ID 132 does not communicate to the right port number, and therefore its packets are not recorded, even though the server gateway is still listening for one last client.

run_miscellaneous: all
	./sensor_gateway 5678 4 & \
	sleep 1s; \
	./sensor_node 10 100000 127.0.0.1 5678 & \
	./sensor_node 21 100000 127.0.0.1 5678 & \
	./sensor_node 37 100000 127.0.0.1 5678 & \
	./sensor_node 132 500000 127.0.0.1 1111 & \
	sleep 5s; \
	killall sensor_node; \
	sleep 0.02s; \
	killall sensor_gateway

# 5 )

# MISCELLANEOUS 2

# The code works under ambiguous conditions:
# 6 sensors are created.
# -> ID 112 has a frequency of recording higher than TIMEOUT, which means it will record once and then disconnect.
# -> ID 142 should not be recorded because the max nr of connections for the sensor_gateway is 5.
# So, within 6 seconds from the execution's start, only 4 sensors should be recording continuously.

run_miscellaneous2: all
	./sensor_gateway 5678 5 & \
	sleep 1s; \
	./sensor_node 15 500000 127.0.0.1 5678 & \
	sleep 1s;
	./sensor_node 21 500000 127.0.0.1 5678 & \
	sleep 1s;
	./sensor_node 37 500000 127.0.0.1 5678 & \
	sleep 1s; \
	./sensor_node 49 500000 127.0.0.1 5678 & \
	sleep 1s;
	./sensor_node 112 6000000 127.0.0.1 5678 & \
	sleep 1s;
	./sensor_node 142 500000 127.0.0.1 5678 & \
	sleep 10s; \
	killall sensor_node; \
	sleep 0.02s; \
	killall sensor_gateway

# ******* VALGRIND TESTING *******

# I ensure all memory is freed by running first the server ( "valgrind" target), and then multiple sensor clients ( "run_sensor_clients" targets).
# Two terminals will open, one for the server and one for the sensor nodes. I wait a bit, and then stop each client (kill the "run_sensor_clients" terminal) manually.
# The server terminal will then show the valgrind compilation, thus enabling me to see the memory allocs, frees, and overall errors.

valgrind: sensor_gateway
	valgrind --leak-check=full ./sensor_gateway 5678 3

run_sensor_client1: sensor_node
	./sensor_node 15 100000 127.0.0.1 5678

run_sensor_client2: sensor_node
	./sensor_node 21 200000 127.0.0.1 5678

run_sensor_client3: sensor_node
	./sensor_node 37 500000 127.0.0.1 5678

# ************************************** END OF MARTIN'S TESTS ***********************************************

# ****** ORIGINAL BUILD TARGETS (UNCHANGED) ******

# When trying to compile one of the executables, first look for its .c files
# Then check if the libraries are in the lib folder
sensor_gateway : main.c connmgr.c datamgr.c sensor_db.c sbuffer.c lib/libdplist.so lib/libtcpsock.so
	@echo "$(TITLE_COLOR)\n***** COMPILING sensor_gateway *****$(NO_COLOR)"
	gcc -c main.c      -Wall -std=c11 -Werror -DSET_MIN_TEMP=20 -DSET_MAX_TEMP=21 -DTIMEOUT=5 -o main.o      -fdiagnostics-color=auto
	gcc -c connmgr.c   -Wall -std=c11 -Werror -DSET_MIN_TEMP=20 -DSET_MAX_TEMP=21 -DTIMEOUT=5 -o connmgr.o   -fdiagnostics-color=auto
	gcc -c datamgr.c   -Wall -std=c11 -Werror -DSET_MIN_TEMP=20 -DSET_MAX_TEMP=21 -DTIMEOUT=5 -o datamgr.o   -fdiagnostics-color=auto
	gcc -c sensor_db.c -Wall -std=c11 -Werror -DSET_MIN_TEMP=20 -DSET_MAX_TEMP=21 -DTIMEOUT=5 -o sensor_db.o -fdiagnostics-color=auto
	gcc -c sbuffer.c   -Wall -std=c11 -Werror -DSET_MIN_TEMP=20 -DSET_MAX_TEMP=21 -DTIMEOUT=5 -o sbuffer.o   -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING sensor_gateway *****$(NO_COLOR)"
	gcc main.o connmgr.o datamgr.o sensor_db.o sbuffer.o -ldplist -ltcpsock -lpthread -o sensor_gateway -Wall -L./lib -Wl,-rpath=./lib -fdiagnostics-color=auto

#target for a quick build of your source code.
sensor_gateway_quick :
	gcc -w -o sensor_gateway main.c connmgr.c datamgr.c sensor_db.c sbuffer.c lib/dplist.c lib/tcpsock.c -DSET_MIN_TEMP=10 -DSET_MAX_TEMP=20 -DTIMEOUT=5 -lpthread 
		
sensor_gateway_debug :
	gcc -g -w -o sensor_gateway main.c connmgr.c datamgr.c sensor_db.c sbuffer.c lib/dplist.c lib/tcpsock.c -DSET_MIN_TEMP=10 -DSET_MAX_TEMP=20 -DTIMEOUT=5 -lpthread 

#file_creator program to generate a room map	
file_creator : file_creator.c
	@echo "$(TITLE_COLOR)\n***** COMPILE & LINKING file_creator *****$(NO_COLOR)"
	gcc file_creator.c -o file_creator -Wall -fdiagnostics-color=auto

#test client
sensor_node : sensor_node.c lib/libtcpsock.so
	@echo "$(TITLE_COLOR)\n***** COMPILING sensor_node *****$(NO_COLOR)"
	gcc -c sensor_node.c -Wall -std=c11 -Werror -o sensor_node.o -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING sensor_node *****$(NO_COLOR)"
	gcc sensor_node.o -ltcpsock -o sensor_node -Wall -L./lib -Wl,-rpath=./lib -fdiagnostics-color=auto

# If you only want to compile one of the libs, this target will match (e.g. make liblist)
libdplist : lib/libdplist.so
libtcpsock : lib/libtcpsock.so

lib/libdplist.so : lib/dplist.c
	@echo "$(TITLE_COLOR)\n***** COMPILING LIB dplist *****$(NO_COLOR)"
	gcc -c lib/dplist.c -Wall -std=c11 -Werror -fPIC -o lib/dplist.o -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING LIB dplist< *****$(NO_COLOR)"
	gcc lib/dplist.o -o lib/libdplist.so -Wall -shared -lm -fdiagnostics-color=auto

lib/libtcpsock.so : lib/tcpsock.c
	@echo "$(TITLE_COLOR)\n***** COMPILING LIB tcpsock *****$(NO_COLOR)"
	gcc -c lib/tcpsock.c -Wall -std=c11 -Werror -fPIC -o lib/tcpsock.o -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING LIB tcpsock *****$(NO_COLOR)"
	gcc lib/tcpsock.o -o lib/libtcpsock.so -Wall -shared -lm -fdiagnostics-color=auto

# do not look for files called clean, clean-all or this will be always a target
.PHONY : clean clean-all run zip

clean:
	rm -rf *.o sensor_gateway sensor_node file_creator *~

clean-all: clean
	rm -rf lib/*.so

run : sensor_gateway sensor_node
	@echo "Add your own implementation here..."

zip:
	zip lab_final.zip main.c connmgr.c connmgr.h datamgr.c datamgr.h sbuffer.c sbuffer.h sensor_db.c sensor_db.h config.h lib/dplist.c lib/dplist.h lib/tcpsock.c lib/tcpsock.h Makefile

clean-thoroughly:

