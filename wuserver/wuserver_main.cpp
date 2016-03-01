#include <zmq.h>
#include <cassert>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#if (defined (WIN32))
#   define srandom srand
#   define random rand
#endif

//  Provide random number from 0..(num-1)
#define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))

int main (void)
{
    //  Prepare our context and publisher
    void *context = zmq_ctx_new ();
    void *publisher = zmq_socket (context, ZMQ_PUSH);
    //int rc = zmq_bind (publisher, "tcp://192.168.0.254:5556");
    int rc = zmq_connect (publisher, "tcp://192.168.0.254:5559");
   assert (rc == 0);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));
    while (1) {
        //  Get values that will fool the boss
        int zipcode, temperature, relhumidity;
        zipcode     = randof (100000);
        temperature = randof (215) - 80;
        relhumidity = randof (50) + 10;

        //  Send message to all subscribers
        char update [20];
        //sprintf (update, "%05d %d %d", zipcode, temperature, relhumidity);
        sprintf (update, "hello %05d %d %d", zipcode, temperature, relhumidity);

		printf(update);
	    int size = zmq_send (publisher, update, strlen (update), 0);

        //s_send (publisher, update);
    }
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;
}