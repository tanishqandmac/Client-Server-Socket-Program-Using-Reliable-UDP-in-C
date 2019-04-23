Readme 

How to execute the code ?

1. Open terminal
2. Open two separate tabs for client and server

for Server side:
1. 'make server'
2. './server 9999'
where 9999 - port number
      
for client side:
1. 'make client'
2. './client 0.0.0.0 9999 n'
where 0.0.0.0 - ip address
      9999 - port number
      n - arbitrary window size

Here the reliable connection over udp uses the Selective repeat protocol for communication.