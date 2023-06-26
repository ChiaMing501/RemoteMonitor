all: remoteServer

CC := gcc

remoteServer: remoteServer.o mysqlServer.o
	$(CC) -o $@ $^ -lmysqlclient

remoteServer.o: remoteServer.c
	$(CC) -c $<

mysqlServer.o: mysqlServer.c
	$(CC) -c $<

clean:
	rm -f *.o
