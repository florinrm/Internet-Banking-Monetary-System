build:
	g++ -Wall -g server.cpp -o server
	g++ -Wall -g client.cpp -o client

run_server:
	./server 5000 users_data_file

run_client:
	./client 127.0.0.1 5000

clean:
	rm server client
	rm *.log
