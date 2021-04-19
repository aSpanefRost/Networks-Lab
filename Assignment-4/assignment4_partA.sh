#!/bin/bash

if [[ $EUID -ne 0 ]]; then										
	echo "Execute the code in 'sudo su' "		#this portion checks if the 
    exit 1	                                    #user is in super user mode or not
														
fi

ip netns add N1                            #adds the namespace N1
ip netns add N2                            #adds the namespace N2
ip netns add N3							   #adds the namespace N3
ip netns add N4							   #adds the namespace N4

ip link add v1 type veth peer name v2      #add a virtual ethernet peer betwenn v1 ane v2
ip link add v3 type veth peer name v4      #add a virtual ethernet peer betwenn v3 ane v4
ip link add v5 type veth peer name v6      #add a virtual ethernet peer betwenn v5 ane v6

ip link set v1 netns N1                    #connects the veth interface v1 to the namsespace N1
ip link set v2 netns N2                    #connects the veth interface v2 to the namsespace N2
ip link set v3 netns N2                    #connects the veth interface v3 to the namsespace N2
ip link set v4 netns N3                    #connects the veth interface v4 to the namsespace N3
ip link set v5 netns N3                    #connects the veth interface v5 to the namsespace N3
ip link set v6 netns N4                    #connects the veth interface v6 to the namsespace N4

ip -n N1 addr add 10.0.10.34/24 dev v1     #assigning the ip address of v1 to 10.0.10.34/24 in the namespace N1
ip -n N2 addr add 10.0.10.35/24 dev v2     #assigning the ip address of v2 to 10.0.10.35/24 in the namespace N2
ip -n N2 addr add 10.0.20.34/24 dev v3     #assigning the ip address of v3 to 10.0.20.34/24 in the namespace N2
ip -n N3 addr add 10.0.20.35/24 dev v4     #assigning the ip address of v4 to 10.0.20.35/24 in the namespace N3
ip -n N3 addr add 10.0.30.34/24 dev v5     #assigning the ip address of v5 to 10.0.30.34/24 in the namespace N3
ip -n N4 addr add 10.0.30.35/24 dev v6     #assigning the ip address of v6 to 10.0.30.35/24 in the namespace N4

ip -n N1 link set v1 up                    #in N1 setting up v1
ip -n N2 link set v2 up                    #in N2 setting up v2
ip -n N2 link set v3 up                    #in N2 setting up v3
ip -n N3 link set v4 up                    #in N3 setting up v4
ip -n N3 link set v5 up                    #in N3 setting up v5
ip -n N4 link set v6 up                    #in N4 setting up v6

ip -n N1 link set lo up                    #in N1 setting up lo
ip -n N2 link set lo up                    #in N2 setting up lo
ip -n N3 link set lo up                    #in N3 setting up lo
ip -n N4 link set lo up                    #in N4 setting up lo


#adding new routes
ip -n N1 route add 10.0.20.0/24 via 10.0.10.35 dev v1 #route to the network 10.0.20.0/24 via gateway 10.0.10.35 through device v1
ip -n N2 route add 10.0.30.0/24 via 10.0.20.35 dev v3 #route to the network 10.0.30.0/24 via gateway 10.0.20.35 through device v3
ip -n N4 route add 10.0.20.0/24 via 10.0.30.34 dev v6 #route to the network 10.0.20.0/24 via gateway 10.0.30.35 through device v6
ip -n N3 route add 10.0.10.0/24 via 10.0.20.34 dev v4 #route to the network 10.0.10.0/24 via gateway 10.0.20.35 through device v4 
ip -n N1 route add 10.0.30.0/24 via 10.0.10.35 dev v1 #route to the network 10.0.30.0/24 via gateway 10.0.10.35 through device v1
ip -n N4 route add 10.0.10.0/24 via 10.0.30.34 dev v6 #route to the network 10.0.10.0/24 via gateway 10.0.30.35 through device v6

ip netns exec N1 sysctl -w net.ipv4.ip_forward=1    #enables ip forwarding at N1
ip netns exec N2 sysctl -w net.ipv4.ip_forward=1    #enables ip forwarding at N2
ip netns exec N3 sysctl -w net.ipv4.ip_forward=1    #enables ip forwarding at N3
ip netns exec N4 sysctl -w net.ipv4.ip_forward=1    #enables ip forwarding at N4


#all the ping commands
sudo ip netns exec N1 ping -c3 10.0.10.34             
sudo ip netns exec N1 ping -c3 10.0.10.35
sudo ip netns exec N1 ping -c3 10.0.20.34
sudo ip netns exec N1 ping -c3 10.0.20.35
sudo ip netns exec N1 ping -c3 10.0.30.34
sudo ip netns exec N1 ping -c3 10.0.30.35

sudo ip netns exec N2 ping -c3 10.0.10.34
sudo ip netns exec N2 ping -c3 10.0.10.35
sudo ip netns exec N2 ping -c3 10.0.20.34
sudo ip netns exec N2 ping -c3 10.0.20.34
sudo ip netns exec N2 ping -c3 10.0.30.34
sudo ip netns exec N2 ping -c3 10.0.30.35

sudo ip netns exec N3 ping -c3 10.0.10.34
sudo ip netns exec N3 ping -c3 10.0.10.35
sudo ip netns exec N3 ping -c3 10.0.20.34
sudo ip netns exec N3 ping -c3 10.0.20.34
sudo ip netns exec N3 ping -c3 10.0.20.35
sudo ip netns exec N3 ping -c3 10.0.30.35

sudo ip netns exec N4 ping -c3 10.0.10.34
sudo ip netns exec N4 ping -c3 10.0.10.35
sudo ip netns exec N4 ping -c3 10.0.20.34
sudo ip netns exec N4 ping -c3 10.0.20.35
sudo ip netns exec N4 ping -c3 10.0.30.34
sudo ip netns exec N4 ping -c3 10.0.30.35