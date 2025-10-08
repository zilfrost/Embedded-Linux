
IPv4_SS: ./IPv4_StreamSocket/client.c ./IPv4_StreamSocket/server.c
	gcc ./IPv4_StreamSocket/server.c -o ipv4_tcp_server
	gcc ./IPv4_StreamSocket/client.c -o ipv4_tcp_client
IPv4_DS: ./IPv4_DatagramSocket/client.c ./IPv4_DatagramSocket/server.c
	gcc ./IPv4_DatagramSocket/server.c -o ipv4_udp_server
	gcc ./IPv4_DatagramSocket/client.c -o ipv4_udp_client
Unix_SS: ./Unix_StreamSocket/client.c ./Unix_StreamSocket/server.c
	gcc ./Unix_StreamSocket/server.c -o unix_tcp_server
	gcc ./Unix_StreamSocket/client.c -o unix_tcp_client
Unix_DS: ./Unix_DatagramSocket/client.c ./Unix_DatagramSocket/server.c
	gcc ./Unix_DatagramSocket/server.c -o unix_udp_server
	gcc ./Unix_DatagramSocket/client.c -o unix_udp_client
all: IPv4_SS IPv4_DS Unix_SS Unix_DS

clean:
	rm -rf ipv4_tcp_server ipv4_tcp_client ipv4_udp_server ipv4_udp_client unix_tcp_server unix_tcp_client unix_udp_server unix_udp_client
