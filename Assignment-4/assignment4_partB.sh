#!/bin/bash


if [[ $EUID -ne 0 ]]; then					 					
	echo "Execute the code in 'sudo su' "		 #this portion checks if the
	exit 1										 #user is in super user mode or not				
fi

ip netns add H1                              #adds the namespace H1
ip netns add H2								 #adds the namespace H2                              
ip netns add H3								 #adds the namespace H3
ip netns add H4								 #adds the namespace H4
ip netns add R1								 #adds the namespace R1
ip netns add R2								 #adds the namespace R2
ip netns add R3								 #adds the namespace R3

ip link add v1 type veth peer name v2        #add a virtual ethernet peer betwenn v1 ane v2
ip link add v3 type veth peer name v4        #add a virtual ethernet peer betwenn v3 ane v4
ip link add v5 type veth peer name v6        #add a virtual ethernet peer betwenn v5 ane v6
ip link add v7 type veth peer name v8        #add a virtual ethernet peer betwenn v7 ane v8
ip link add v9 type veth peer name v10       #add a virtual ethernet peer betwenn v9 ane v10
ip link add v11 type veth peer name v12      #add a virtual ethernet peer betwenn v11 ane v12

ip link set v1 netns H1                      #connects the veth interface v1 to the namsespace H1 
ip link set v3 netns H2                      #connects the veth interface v3 to the namsespace H2
ip link set v10 netns H3                     #connects the veth interface v10 to the namsespace H3
ip link set v12 netns H4                     #connects the veth interface v12 to the namsespace H4
ip link set v2 netns R1                      #connects the veth interface v2 to the namsespace R1
ip link set v5 netns R1                      #connects the veth interface v5 to the namsespace R1
ip link set v4 netns R1                      #connects the veth interface v4 to the namsespace R1
ip link set v6 netns R2                      #connects the veth interface v6 to the namsespace R2
ip link set v7 netns R2                      #connects the veth interface v7 to the namsespace R2
ip link set v8 netns R3                      #connects the veth interface v8 to the namsespace R3
ip link set v9 netns R3                      #connects the veth interface v9 to the namsespace R3
ip link set v11 netns R3                     #connects the veth interface v11 to the namsespace R3

ip -n H1 addr add 10.0.10.34/24 dev v1       #assigning the ip address of v1 to 10.0.10.34/24 in the namespace H1
ip -n H2 addr add 10.0.20.34/24 dev v3       #assigning the ip address of v1 to 10.0.20.34/24 in the namespace H2
ip -n H3 addr add 10.0.50.35/24 dev v10      #assigning the ip address of v1 to 10.0.50.35/24 in the namespace H3
ip -n H4 addr add 10.0.60.35/24 dev v12      #assigning the ip address of v1 to 10.0.60.35/24 in the namespace H4
ip -n R1 addr add 10.0.10.35/24 dev v2       #assigning the ip address of v1 to 10.0.10.35/24 in the namespace R1
ip -n R1 addr add 10.0.20.35/24 dev v4       #assigning the ip address of v1 to 10.0.20.35/24 in the namespace R1
ip -n R1 addr add 10.0.30.34/24 dev v5       #assigning the ip address of v1 to 10.0.30.34/24 in the namespace R1
ip -n R2 addr add 10.0.30.35/24 dev v6       #assigning the ip address of v1 to 10.0.30.35/24 in the namespace R2
ip -n R2 addr add 10.0.40.34/24 dev v7       #assigning the ip address of v1 to 10.0.40.34/24 in the namespace R2
ip -n R3 addr add 10.0.40.35/24 dev v8       #assigning the ip address of v1 to 10.0.40.35/24 in the namespace R3
ip -n R3 addr add 10.0.50.34/24 dev v9       #assigning the ip address of v1 to 10.0.50.34/24 in the namespace R3
ip -n R3 addr add 10.0.60.34/24 dev v11      #assigning the ip address of v1 to 10.0.60.34/24 in the namespace R3

ip -n H1 link set v1 up                      #in H1 setting up v1        
ip -n H2 link set v3 up                      #in H2 setting up v3 
ip -n H3 link set v10 up                     #in H3 setting up v10
ip -n H4 link set v12 up                     #in H4 setting up v12
ip -n R1 link set v2 up                      #in R1 setting up v2
ip -n R1 link set v5 up                      #in R1 setting up v5
ip -n R1 link set v4 up                      #in R1 setting up v4
ip -n R2 link set v6 up                      #in R2 setting up v6
ip -n R2 link set v7 up                      #in R2 setting up v7
ip -n R3 link set v8 up                      #in R3 setting up v8
ip -n R3 link set v9 up                      #in R3 setting up v9
ip -n R3 link set v11 up                     #in R3 setting up v11

ip -n R1 link set lo up                      #in R1 setting up lo
ip -n R2 link set lo up                      #in R2 setting up lo 
ip -n R3 link set lo up                      #in R3 setting up lo
ip -n H1 link set lo up                      #in H1 setting up lo
ip -n H2 link set lo up                      #in H2 setting up lo
ip -n H3 link set lo up                      #in H3 setting up lo
ip -n H4 link set lo up                      #in H4 setting up lo

ip -n H1 route add 10.0.30.0/24 via 10.0.10.35 dev v1  #route to the network 10.0.30.0/24 via gateway 10.0.10.35 through device v1  
ip -n H1 route add 10.0.20.0/24 via 10.0.10.35 dev v1  #route to the network 10.0.20.0/24 via gateway 10.0.10.35 through device v1 
ip -n H2 route add 10.0.10.0/24 via 10.0.20.35 dev v3  #route to the network 10.0.10.0/24 via gateway 10.0.20.35 through device v3 
ip -n H2 route add 10.0.30.0/24 via 10.0.20.35 dev v3  #route to the network 10.0.30.0/24 via gateway 10.0.20.35 through device v3 
ip -n R1 route add 10.0.40.0/24 via 10.0.30.35 dev v5  #route to the network 10.0.40.0/24 via gateway 10.0.30.35 through device v5 
ip -n R2 route add 10.0.50.0/24 via 10.0.40.35 dev v7  #route to the network 10.0.50.0/24 via gateway 10.0.40.35 through device v7 
ip -n R1 route add 10.0.50.0/24 via 10.0.30.35 dev v5  #route to the network 10.0.50.0/24 via gateway 10.0.30.35 through device v5 
ip -n R2 route add 10.0.60.0/24 via 10.0.40.35 dev v7  #route to the network 10.0.60.0/24 via gateway 10.0.40.35 through device v7 
ip -n R1 route add 10.0.60.0/24 via 10.0.30.35 dev v5  #route to the network 10.0.60.0/24 via gateway 10.0.30.35 through device v5 
ip -n H1 route add 10.0.50.0/24 via 10.0.10.35 dev v1  #route to the network 10.0.50.0/24 via gateway 10.0.10.35 through device v1 
ip -n H1 route add 10.0.60.0/24 via 10.0.10.35 dev v1  #route to the network 10.0.60.0/24 via gateway 10.0.10.35 through device v1  
ip -n H2 route add 10.0.50.0/24 via 10.0.20.35 dev v3  #route to the network 10.0.50.0/24 via gateway 10.0.20.35 through device v3 
ip -n H2 route add 10.0.60.0/24 via 10.0.20.35 dev v3  #route to the network 10.0.60.0/24 via gateway 10.0.20.35 through device v3 
ip -n H1 route add 10.0.40.0/24 via 10.0.10.35 dev v1  #route to the network 10.0.40.0/24 via gateway 10.0.10.35 through device v1 
ip -n H2 route add 10.0.40.0/24 via 10.0.20.35 dev v3  #route to the network 10.0.40.0/24 via gateway 10.0.20.35 through device v3 

ip -n H3 route add 10.0.40.0/24 via 10.0.50.34 dev v10  #route to the network 10.0.40.0/24 via gateway 10.0.50.35 through device v10  
ip -n H3 route add 10.0.60.0/24 via 10.0.50.34 dev v10  #route to the network 10.0.60.0/24 via gateway 10.0.50.35 through device v10  
ip -n H4 route add 10.0.40.0/24 via 10.0.60.34 dev v12  #route to the network 10.0.40.0/24 via gateway 10.0.60.35 through device v12   
ip -n H4 route add 10.0.50.0/24 via 10.0.60.34 dev v12  #route to the network 10.0.50.0/24 via gateway 10.0.60.35 through device v12   
ip -n R3 route add 10.0.30.0/24 via 10.0.40.34 dev v8   #route to the network 10.0.30.0/24 via gateway 10.0.40.35 through device v8   
ip -n R2 route add 10.0.10.0/24 via 10.0.30.34 dev v6   #route to the network 10.0.10.0/24 via gateway 10.0.30.35 through device v6   
ip -n R3 route add 10.0.10.0/24 via 10.0.40.34 dev v8   #route to the network 10.0.10.0/24 via gateway 10.0.40.35 through device v8   
ip -n R2 route add 10.0.20.0/24 via 10.0.30.34 dev v6   #route to the network 10.0.20.0/24 via gateway 10.0.30.35 through device v6   
ip -n R3 route add 10.0.20.0/24 via 10.0.40.34 dev v8   #route to the network 10.0.20.0/24 via gateway 10.0.40.35 through device v8  
ip -n H3 route add 10.0.10.0/24 via 10.0.50.34 dev v10  #route to the network 10.0.10.0/24 via gateway 10.0.50.35 through device v10  
ip -n H3 route add 10.0.20.0/24 via 10.0.50.34 dev v10  #route to the network 10.0.20.0/24 via gateway 10.0.50.35 through device v10  
ip -n H4 route add 10.0.10.0/24 via 10.0.60.34 dev v12  #route to the network 10.0.10.0/24 via gateway 10.0.60.35 through device v12   
ip -n H4 route add 10.0.20.0/24 via 10.0.60.34 dev v12  #route to the network 10.0.20.0/24 via gateway 10.0.60.35 through device v12   
ip -n H3 route add 10.0.30.0/24 via 10.0.50.34 dev v10  #route to the network 10.0.30.0/24 via gateway 10.0.50.35 through device v10  
ip -n H4 route add 10.0.30.0/24 via 10.0.60.34 dev v12  #route to the network 10.0.30.0/24 via gateway 10.0.60.35 through device v12  

ip netns exec H1 sysctl -w net.ipv4.ip_forward=1       #enables ip forwarding at H1
ip netns exec H2 sysctl -w net.ipv4.ip_forward=1       #enables ip forwarding at H2
ip netns exec H3 sysctl -w net.ipv4.ip_forward=1       #enables ip forwarding at H3
ip netns exec H4 sysctl -w net.ipv4.ip_forward=1       #enables ip forwarding at H4
ip netns exec R1 sysctl -w net.ipv4.ip_forward=1       #enables ip forwarding at R1
ip netns exec R2 sysctl -w net.ipv4.ip_forward=1       #enables ip forwarding at R2
ip netns exec R3 sysctl -w net.ipv4.ip_forward=1       #enables ip forwarding at R3

#all the ping commands
x=1
while [ $x -le 6 ]
do
	y=$((x*10))
	sudo ip netns exec H1 ping -c3 10.0."$y".34 
	sudo ip netns exec H2 ping -c3 10.0."$y".34 
	sudo ip netns exec H3 ping -c3 10.0."$y".34 
	sudo ip netns exec H4 ping -c3 10.0."$y".34 
	sudo ip netns exec R1 ping -c3 10.0."$y".34 
	sudo ip netns exec R2 ping -c3 10.0."$y".34 
	sudo ip netns exec R3 ping -c3 10.0."$y".34 
	sudo ip netns exec H1 ping -c3 10.0."$y".35 
	sudo ip netns exec H2 ping -c3 10.0."$y".35 
	sudo ip netns exec H3 ping -c3 10.0."$y".35 
	sudo ip netns exec H4 ping -c3 10.0."$y".35 
	sudo ip netns exec R1 ping -c3 10.0."$y".35 
	sudo ip netns exec R2 ping -c3 10.0."$y".35 
	sudo ip netns exec R3 ping -c3 10.0."$y".35 
	x=$((x+1))
done

#traceroute commands
ip netns exec H1 traceroute 10.0.60.35   #H1 to H4
sleep 3   #sleep for 3 sec
ip netns exec H3 traceroute 10.0.60.35   #H3 to H4
sleep 3
ip netns exec H4 traceroute 10.0.20.34   #H4 to H2
sleep 3










