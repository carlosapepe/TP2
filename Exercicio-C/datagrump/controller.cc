#include <iostream>
#include <math.h>
#include "controller.hh"
#include "timestamp.hh"

double the_window_size = 1; // Window size
int64_t DATAtt_min = 99999;	// Datagram trip time minimum
double RTT_min = 99999; 	// Round trip time minimum

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
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
			       /* Quando o datagrama que recebeu ACK foi enviado (hora de quem envia) */
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* Quando o datagrama que recebeu ACK foi recebido (hora de quem recebe) */
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
			       /* Quando um ACK foi recebido pelo remetente */
                   /* when the ack was received (by sender) */
{
	// Calculate Datagram trip time (DATAtt) from sender to receiver
    int64_t DATAtt = recv_timestamp_acked - send_timestamp_acked; 
	DATAtt_min = min (DATAtt, DATAtt_min);  
	
    // Calculate Round trip time (RTT)
    double RTT =  timestamp_ack_received - send_timestamp_acked;
    RTT_min = min (RTT, RTT_min);

	// Estimate minimum Datagram trip time. It's half of RTT_min
    double est_DATAtt_min = (RTT_min / 2);
    
    // Estimate Datagram trip time to make changes in the_window_size
    double est_DATAtt = (DATAtt - DATAtt_min) + est_DATAtt_min;
	
	if (est_DATAtt > 1.5 * est_DATAtt_min)
        the_window_size -= 0.3;
    else
        the_window_size += 0.5;

    if (the_window_size < 1) 
        the_window_size = 1; // 
    else if (the_window_size > 70){
        the_window_size = 70; // 
	}
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
unsigned int Controller::timeout_ms( void )
{
  return 80;
}
