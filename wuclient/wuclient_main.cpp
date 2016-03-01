//  Weather update client
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode

#include <zmq.h>
#include <stdio.h>
#include <assert.h>

int main (int argc, char *argv [])
{
    //  Socket to talk to server
    printf ("Collecting updates from weather server¡­\n");
    void *context = zmq_ctx_new ();
    void *subscriber = zmq_socket (context, ZMQ_SUB);
     int rc = zmq_connect (subscriber, "tcp://192.168.0.254:6660");
   //int rc = zmq_connect (subscriber, "tcp://192.168.0.254:5556");
    assert (rc == 0);

    //  Subscribe to zipcode, default is NYC, 10001
   //char *filter = (argc > 1)? argv [1]: "10001 ";
   char *filter = (argc > 1)? argv [1]: "hello";
   rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE,
                         filter, strlen (filter));
    assert (rc == 0);

    //  Process 100 updates
    int update_nbr;
    long total_temp = 0;
    for (update_nbr = 0; update_nbr < 100; update_nbr++) {
		char string [256];
		int size = zmq_recv (subscriber, string, 255, 0);
		if (size == -1)
		{
			return -1;
		}
		if (size > 255)
			size = 255;
		string [size] = 0;

		char buffer[256];
        int zipcode, temperature, relhumidity;
        sscanf (string, "%s %d %d %d",
            buffer, &zipcode, &temperature, &relhumidity);
        total_temp += temperature;
    }
    printf ("Average temperature for zipcode '%s' was %dF\n",
        filter, (int) (total_temp / update_nbr));

    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    return 0;
}