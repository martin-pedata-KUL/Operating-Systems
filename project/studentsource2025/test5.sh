make all
port=5678
clients=5
echo -e "starting gateway "
./sensor_gateway $port $clients &
sleep 3
echo -e 'starting 5 sensor nodes'
./sensor_node 15 100000 127.0.0.1 $port &
sleep 0.1s
./sensor_node 10 100000 127.0.0.1 $port &
sleep 0.1s
./sensor_node 37 100000 127.0.0.1 $port &
sleep 0.1s
./sensor_node 132 100000 127.0.0.1 $port &
sleep 0.1s
./sensor_node 142 100000 127.0.0.1 $port &
sleep 7s
killall sensor_node
sleep 0.1s
killall sensor_gateway
