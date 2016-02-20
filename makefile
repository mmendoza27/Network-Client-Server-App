CC = cc
COPS =
LINT = lint
LOPS = -x -u
NETLIBS = 

all: client server client2 client3

server: server.c primelib.c restart.c uici.c uiciname.c
	$(CC) $(COPS) -o server server.c primelib.c restart.c uici.c uiciname.c -lm $(NETLIBS)

client: client.c restart.c uici.c uiciname.c
	$(CC) $(COPS) -o client client.c restart.c uici.c uiciname.c $(NETLIBS)

client2: client2.c restart.c uici.c uiciname.c
	$(CC) $(COPS) -o client2 client2.c restart.c uici.c uiciname.c -lpthread $(NETLIBS)

client3: client3.c restart.c uici.c uiciname.c
	$(CC) $(COPS) -o client3 client3.c restart.c uici.c uiciname.c -lpthread $(NETLIBS)

lintall: lints lintsp lintsp2 lintsr lintc lintcr lintca lintc2 lints2

lints:
	$(LINT) $(LOPS) server.c restart.c uici.c uiciname.c

lintsp:
	$(LINT) $(LOPS) serverp.c restart.c uici.c uiciname.c copy2files.c

lintsp2:
	$(LINT) $(LOPS) serverp2.c restart.c uici.c uiciname.c

lintsr:
	$(LINT) $(LOPS) reflectserver.c restart.c uici.c uiciname.c

lintc:
	$(LINT) $(LOPS) client.c restart.c uici.c uiciname.c

lintcr:
	$(LINT) $(LOPS) reflectclient.c restart.c uici.c uiciname.c

lintca:
	$(LINT) $(LOPS) client_audio.c restart.c uici.c uiciname.c hasvoice.c

lintc2:
	$(LINT) $(LOPS) client2.c restart.c uici.c uiciname.c copy2files.c

lints2:
	$(LINT) $(LOPS) server2.c restart.c uici.c uiciname.c copy2files.c

clean:
	rm -f *.o server client
