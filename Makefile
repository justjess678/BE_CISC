All: compile

service_nominal:
	gcc -Wall -o service_nominal service_nominal.c -pthread
	

compile: service_nominal

clean:
	rm service_nominal


