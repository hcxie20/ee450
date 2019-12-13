CXX = g++
		
targets = awsoutput\
		client\
		monitoroutput\
		serverAoutput\
		serverBoutput\
		database.txt


all:
	g++ -o awsoutput aws.cpp
	g++ -o client client.cpp
	g++ -o monitoroutput monitor.cpp
	g++ -o serverAoutput serverA.cpp
	g++ -o serverBoutput serverB.cpp
.PHONY: aws monitor serverA serverB tar
aws:aws
	./awsoutput
monitor:monitor
	./monitoroutput
serverA:serverA
	./serverAoutput
serverB:serverB
	./serverBoutput
clean:
	rm  $(targets)
tar:
	tar cvf ee450_xiehaoch_session1.tar *
	gzip ee450_xiehaoch_session1.tar

