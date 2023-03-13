// Implements the server side of an echo client-server application program.
// The client reads ITERATIONS strings from stdin, passes the string to the
// this server, which simply sends the string back to the client.
//
// Compile on general.asu.edu as:
//   g++ -o server UDPEchoServer.c
//
// Only on general3 and general4 have the ports >= 1024 been opened for
// application programs.
#include <stdio.h>      // for printf() and fprintf()
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h> // for socket() and bind()
#include <arpa/inet.h>  // for sockaddr_in and inet_ntoa()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include "../include/defns.h"

#define ECHOMAX 255     // Longest string to echo

void DieWithError( const char *errorMessage ) // External error handling function
{
    perror( errorMessage );
    exit( 1 );
}

int main( int argc, char *argv[] )
{
    int sock;                        // Socket
    struct sockaddr_in servAddr; // Local address of server
    struct sockaddr_in clientAddr; // Client address
    unsigned int cliAddrLen;         // Length of incoming message
    unsigned short servPort;     // Server port
    int recvMsgSize;                 // Size of received message
    
    struct Packet packet;
    struct CustomerInfo* customer_database;
    struct Cohort* cohort_database;
    int num_of_customers;
    int num_of_cohorts;
    bool failed;

    if( argc != 2 )         // Test for correct number of parameters
    {
        fprintf( stderr, "Usage:  %s <UDP SERVER PORT>\n", argv[ 0 ] );
        exit( 1 );
    }

    servPort = atoi(argv[1]);  // First arg: local port

    // Create socket for sending/receiving datagrams
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "server: socket() failed" );

    // Construct local address structure */
    memset( &servAddr, 0, sizeof( servAddr ) ); // Zero out structure
    servAddr.sin_family = AF_INET;                  // Internet address family
    servAddr.sin_addr.s_addr = htonl( INADDR_ANY ); // Any incoming interface
    servAddr.sin_port = htons( servPort );      // Local port

    // Bind to the local address
    if( bind( sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0 )
        DieWithError( "server: bind() failed" );

	printf( "server: Port server is listening to is: %d\n", servPort );

    num_of_customers = 0;
    num_of_cohorts = 0;
    customer_database = (struct CustomerInfo*)malloc(num_of_customers * sizeof(struct CustomerInfo));

    for(;;) // Run forever
    {
        failed = false;
        cliAddrLen = sizeof( clientAddr );

        // Block until receive message from a client
        if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, &cliAddrLen )) < 0 )
        {
            DieWithError( "server: recvfrom() failed" );
        }

        // open an account
        if (strcmp(packet.command_choice,"open") == 0)
        {
            for (int i = 0; i < num_of_customers; i++)
            {   
                // customer already exists in the database
                if (strcmp(customer_database[i].name, packet.customer_info.name) == 0)
                {
                    failed = true;
                    break;
                }
            }

            if (failed == false)
            {
                customer_database = (struct CustomerInfo*)realloc(customer_database, customer_database + sizeof(struct CustomerInfo));
                customer_database[num_of_customers] = packet.customer_info;
                num_of_customers++;

                printf( "server: new customer added to database\n");

                // Send received datagram back to the client
                packet.req_res = 1; // response
                packet.succ_fail = 1; // success
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );
            }
            else
            {
                printf( "server: customer already exists\n");

                // Send received datagram back to the client
                packet.req_res = 1; // response
                packet.succ_fail = 0; // failure
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );
            }
        }
        else if (strcmp(packet.command_choice,"new_cohort") == 0)
        {
            if (num_of_cohorts != 0) // check if client is already in an existing cohort
            {
                for (int i = 0; i < num_of_customers; i++)
                {   
                    // customer already exists in a cohort
                    if (strcmp(customer_database[i].name, packet.cohort.founder_name) == 0 && customer_database[i].in_cohort == true)
                    {
                        failed = true;
                        break;
                    }
                }
            }

            if (failed == false)
            {
                
            }
            /*
            if (currentArraySize < packet.cohort.size)
            {
                failed = true;
                printf( "server: not enough customers\n");
            }
            else
            {
                int index;
                srand(time(NULL));
                packet.cohort.customer_info_array = (struct CustomerInfo*)malloc(packet.cohort.size * sizeof(struct CustomerInfo));

                int randArray[packet.cohort.size];
                for (int i = 0; i < packet.cohort.size; i++)
                {
                    randArray[i] = i;
                }
                for (int i = 0; i < packet.cohort.size; i++)
                {
                    int temp = randArray[i];
                    int randomIndex = rand() % packet.cohort.size;

                    randArray[i] = randArray[randomIndex];
                    randArray[randomIndex] = temp;
                }

                for (int i = 0; i < packet.cohort.size; i++)
                {
                    packet.cohort.customer_info_array[i] = CustomerInfoArray[randArray[index]];
                }

                packet.cohort.inCohort = 0; // true

                printf( "server: new cohort created\n");

                if (failed == false)
                {
                    // Send received datagram back to the client
                    packet.status = 1;
                    if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                        DieWithError( "server: sendto() sent a different number of bytes than expected" );

                }
                else
                {
                    // Send received datagram back to the client
                    packet.status = 0;
                    if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                        DieWithError( "server: sendto() sent a different number of bytes than expected" );
                }
            }
            */
        }
        else if (strcmp(packet.command_choice,"delete_cohort") == 0)
        {
            if (packet.cohort.inCohort == 1) // not in cohort
            {
                failed = true;
                printf(" server: client is not in an existing cohort\n");
            }
            else
            {
                packet.cohort.inCohort = 0;
                packet.cohort.customer_info_array = (struct CustomerInfo*)malloc(0);

                printf( "server: cohort deleted successfully\n");

                if (failed == false)
                {
                    // Send received datagram back to the client
                    packet.status = 1;
                    if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                        DieWithError( "server: sendto() sent a different number of bytes than expected" );

                }
                else
                {
                    // Send received datagram back to the client
                    packet.status = 0;
                    if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                        DieWithError( "server: sendto() sent a different number of bytes than expected" );
                }
            }
        }
        else if (strcmp(packet.command_choice,"exit") == 0)
        {
            for (int i = 0; i < currentArraySize; i++)
            {
                if (strcmp(packet.customer_info.name, CustomerInfoArray[i].name) == 0)
                {
                    packet.status = 1;
                    if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                        DieWithError( "server: sendto() sent a different number of bytes than expected" );
                }
            }
        }
        else
        {
            exit(1); // never gets here
        }


    }
    // NOT REACHED */
}
