make all
port=5678
clients=3
echo -e "starting gateway"
./sensor_gateway $port $clients &
sleep 2s
echo -e 'starting 3 sensor nodes'
./sensor_node 10 10000 127.0.0.1 $port &
sleep 0.1s
./sensor_node 21 10000 127.0.0.1 $port &
sleep 0.1s
./sensor_node 37 10000 127.0.0.1 $port &
sleep 5s
killall sensor_node
sleep 0.1s
killall sensor_gateway