#include <iostream>
#include <math.h>
#include "controller.hh"
#include "timestamp.hh"

using namespace std;



/* AIMD constants to be used to*/

const unsigned int ALPHA = 1; //Additive-Increase
const float BETA = .5; //Multiplicative-Decrease
float the_window_size = 1;
unsigned int rtt_threshold = 120;

// End AIMD insertion


/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size()
{

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
  }

  return the_window_size;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp,
                    /* in milliseconds */
				    const bool after_timeout
				    /* datagram was sent because of a timeout */ )
{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << " (timeout = " << after_timeout << ")\n";
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

// Agnaldo - 09/10/18 - Inserting AIMD
  // Calculate round trip time (rtt)
  float rtt = float(timestamp_ack_received - send_timestamp_acked);
  
  // AIMD: If rff is bigger than or equal rtt_threshold, decrease window size.
  // Otherwise, increase. To avoid underflow, window size should be greater than 1
  if (rtt >= rtt_threshold && the_window_size > 1) 
  {
    the_window_size *= BETA;
  } 
  else 
  {
	// AIMD: When ACK is received, increase window size
    the_window_size += ALPHA / the_window_size;
  }

 // End AIMD insertion 
  
  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms()
{
  return 60;	
}