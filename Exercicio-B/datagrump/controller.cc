#include <iostream>
#include "controller.hh"
#include "timestamp.hh"
#include <math.h>
#include <algorithm>
#include <ctime>

using namespace std;

// Globals
double timeouts_thresh = 0;
unsigned int num_timeouts = 0;
double the_window_size = 13.0;
bool slow_start = true;
double ALPHA = 1.15;
double BETA = 0.5;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */

  if ( debug_ ) {
  cerr << "At time " << timestamp_ms()
   << " window size is " << the_window_size << endl;
  }
  if (the_window_size < 1) the_window_size = 1.0;
  return (unsigned int) the_window_size;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
          /* of the sent datagram */
          const uint64_t send_timestamp,
                  /* in milliseconds */
          const bool after_timeout )
{
  /* Default: take no action */

  if ( debug_ ) {
  cerr << "At time " << send_timestamp
   << " sent datagram " << sequence_number << " after timeout " << after_timeout << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
           /* what sequence number was acknowledged */
           const uint64_t send_timestamp_acked,
           /* when the acknowledged datagram was sent (sender's clock) */
           const uint64_t recv_timestamp_acked,
           /* when the acknowledged datagram was received (receiver's clock)*/
           const uint64_t timestamp_ack_received )
           /* when the ack was received (by sender) */
{
  /* Default: take no action */

if ( debug_ ) {
  cerr << "At time " << timestamp_ack_received
   << " received ack for datagram " << sequence_number_acked
   << " (send @ time " << send_timestamp_acked
   << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
   << endl;
}


uint64_t rtt = timestamp_ack_received - send_timestamp_acked;

if (rtt > timeout_ms())
{
	slow_start = false; // Exit Slow start
    if (num_timeouts >= timeouts_thresh)
    {
        num_timeouts = 0;
        timeouts_thresh = the_window_size;
        the_window_size *= BETA;
    }
    else
        num_timeouts += 1;
}
else if (slow_start) // Slow start
	the_window_size *= 2;
else // Congestion avoidance
	the_window_size += ALPHA / the_window_size;
}



/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
	return 80;
}
