#include "czmq.h"

// This main thread simply starts several clients, and a server, and then
// waits for the server to finish.
//
int main (void)
{
    void *context = zmq_init (1);

    // Backend socket talks to workers over inproc
    void *pipein = zmq_socket (context, ZMQ_DEALER);
    zsocket_bind (pipein, "inproc://pipe");

    // Frontend socket talks to clients over TCP
    void *pipeout = zmq_socket (context, ZMQ_ROUTER);
    zsocket_connect (pipeout, "inproc://pipe");

    zstr_send (pipein, "ABC");
    
    int rc;
    zmq_msg_t msg;
    zmq_msg_init (&msg);
    rc = zmq_recvmsg (pipeout, &msg, 0);
    if (rc < 0) {
        printf ("E: error, failed\n");
        exit (1);
    }
    int rcvmore;
    size_t type_size = sizeof (int);
    rc = zmq_getsockopt (pipeout, ZMQ_RCVMORE, &rcvmore, &type_size);
    if (rc)
        puts (strerror (errno));
    assert (rc == 0);

    int rcvlabel;
    zmq_getsockopt (pipeout, ZMQ_RCVLABEL, &rcvlabel, &type_size);
    assert (rc == 0);

    printf ("Got first message part: size=%zd more=%d label=%d\n",
        zmq_msg_size (&msg), rcvmore, rcvlabel);

    return 0;
}

