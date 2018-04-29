build:
	g++ -Wall -g server.cpp -o server
	g++ -Wall -g client.cpp -o client
clean:
	rm server client client.o server.o
