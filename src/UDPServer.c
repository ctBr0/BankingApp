// Implements the server side of an echo client-server application program.
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

    // Construct local address structure
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
    cohort_database = (struct Cohort*)malloc(num_of_cohorts * sizeof(struct Cohort));

    for(;;) // Run forever
    {
        failed = false;
        cliAddrLen = sizeof( clientAddr );

        // Block until receive message from a client
        if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, &cliAddrLen )) < 0 )
        {
            DieWithError( "server: recvfrom() failed" );
        }

        // printf( "%s\n", packet.command_choice);

        // open an account
        if (packet.command_choice == 0)
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
                customer_database = (struct CustomerInfo*)realloc(customer_database, sizeof(customer_database) + sizeof(struct CustomerInfo));
                customer_database[num_of_customers] = packet.customer_info;
                num_of_customers++;

                printf( "server: new customer added to database\n");

                // Send received datagram back to the client
                packet.req_res = 1; // response
                packet.succ_fail = 0; // success
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );
            }
            else
            {
                printf( "server: customer already exists\n");

                // Send received datagram back to the client
                packet.req_res = 1; // response
                packet.succ_fail = 1; // failure
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );
            }
        }
        else if (strcmp(packet.command_choice,"new_cohort") == 0)
        {
            // if (packet.cohort.size > num_of_customers)
            if (num_of_cohorts != 0) // check if client is already in an existing cohort
            {
                for (int i = 0; i < num_of_customers; i++)
                {   
                    // customer already exists in a cohort
                    if (strcmp(customer_database[i].name, packet.cohort.founder_name) == 0)
                    {
                        if (customer_database[i].in_cohort == true)
                        {
                            failed = true;
                        }
                        else
                        {
                            customer_database[i].in_cohort = true; // updated customer in_cohort value
                            packet.cohort.cohort_member_array[0] = customer_database[i]; // updated the founder in the packet.cohort
                        }
                        break;
                    }
                }
            }

            int random;
            if (failed == false)
            {
                bool valid = false;
                int random;

                // add customers to cohort_member_array
                for (int i = 0; i < packet.cohort.size - 1; i++) // excluding the founder
                {
                    // pick a random customer whose is not already in a cohort
                    while (!valid)
                    {
                        random = rand() % num_of_customers;
                        if (customer_database[random].in_cohort == false) // valid
                        {
                            customer_database[random].in_cohort = true; // updating the database
                            packet.cohort.cohort_member_array = (struct CustomerInfo*)realloc(packet.cohort.cohort_member_array, sizeof(packet.cohort.cohort_member_array) + sizeof(struct CustomerInfo));
                            packet.cohort.cohort_member_array[i + 1] = customer_database[random]; // add new cohort member to the packet.cohort
                            valid = true;
                        }
                    }
                }

                // add cohort_member_array to cohort_database
                cohort_database[num_of_cohorts] = packet.cohort;
                num_of_cohorts++;

                // Send received datagram back to the client
                packet.req_res = 1; // response
                packet.succ_fail = 0; // success
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );

                // send received datagram to other cohort members
                for (int i = 1; i < packet.cohort.size; i++) // each member in the cohort except the founder
                {
                    memset( &clientAddr, 0, sizeof( clientAddr ) ); // Zero out structure
                    clientAddr.sin_family = AF_INET;                  // Internet address family
                    clientAddr.sin_addr.s_addr = htonl( packet.cohort.cohort_member_array[i].client_ip_addr );
                    clientAddr.sin_port = htons( packet.cohort.cohort_member_array[i].port_to_bank );      // Local port

                    if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                        DieWithError( "server: sendto() sent a different number of bytes than expected" );

                }

            }
            else
            {
                // Send received datagram back to the client
                packet.req_res = 1; // response
                packet.succ_fail = 1; // failure
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );
            }

        }
        else if (strcmp(packet.command_choice,"delete_cohort") == 0)
        {
            if (packet.customer_info.in_cohort == false) // not in cohort
            {
                failed = true;
                printf(" server: client is not in an existing cohort\n");
            }
            else
            {
                printf( "server: cohort deleted successfully\n");
            }

            if (failed == false) // success
            {
                // Send datagram to other cohort members
                for (int i = 1; i < packet.cohort.size; i++) // each member in the cohort except the founder
                {
                    memset( &clientAddr, 0, sizeof( clientAddr ) ); // Zero out structure
                    clientAddr.sin_family = AF_INET;                  // Internet address family
                    clientAddr.sin_addr.s_addr = htonl( packet.cohort.cohort_member_array[i].client_ip_addr );
                    clientAddr.sin_port = htons( packet.cohort.cohort_member_array[i].port_to_bank );      // Local port

                    if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                        DieWithError( "server: sendto() sent a different number of bytes than expected" );

                }

                // Modify values in customer database
                int count = 0;
                for (int i = 0; i < num_of_customers; i++)
                {
                    for (int j = 0; j < packet.cohort.size; j++)
                    {
                        if (strcmp(customer_database[i].name, packet.cohort.cohort_member_array[j].name) == 0)
                        {
                            customer_database[i].in_cohort = false;
                            j = packet.cohort.size;
                            count++;
                        }
                    }

                    if (count == packet.cohort.size)
                    {
                        i = num_of_customers;
                    }
                }

                // Delete cohort from database
                for (int i = 0; i < num_of_cohorts; i++)
                {
                    if (strcmp(cohort_database[i].founder_name, packet.cohort.founder_name) == 0)
                    {
                        cohort_database[i] = cohort_database[num_of_cohorts - 1];
                        num_of_cohorts--;
                        cohort_database = (struct Cohort*)realloc(cohort_database, num_of_cohorts * sizeof(struct Cohort));

                        i = num_of_cohorts;
                    }
                }

                // Send received datagram back to the client

                packet.customer_info.in_cohort = false;
                packet.cohort.cohort_member_array = (struct CustomerInfo*)malloc(sizeof(struct CustomerInfo));
                packet.cohort.founder_name = "";
                packet.cohort.size = 0;

                packet.req_res = 1;
                packet.succ_fail = 0;
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );

            }
            else
            {
                // Send received datagram back to the client
                packet.req_res = 1;
                packet.succ_fail = 1;
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );
            }
        }
        else if (strcmp(packet.command_choice,"exit") == 0)
        {
            if (packet.customer_info.in_cohort == true)
            {
                failed = true;
                printf(" server: client is in an existing cohort\n");
            }
            else
            {
                // Delete customer from database
                for (int i = 0; i < num_of_customers; i++)
                {
                    if (strcmp(customer_database[i].name, packet.customer_info.name) == 0)
                    {
                        customer_database[i] = customer_database[num_of_customers - 1];
                        num_of_customers--;
                        customer_database = (struct CustomerInfo*)realloc(customer_database, num_of_customers * sizeof(struct CustomerInfo));

                        i = num_of_customers;
                    }
                }
                printf(" server: customer deleted successfully\n");
            }

            if (failed == false)
            {
                // Send received datagram back to the client
                packet.req_res = 1; // response
                packet.succ_fail = 0; // success
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );
            }
            else
            {
                // Send received datagram back to the client
                packet.req_res = 1; // response
                packet.succ_fail = 1; // fail
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                    DieWithError( "server: sendto() sent a different number of bytes than expected" );

            }
        }
        else if (strcmp(packet.command_choice,"deposit") == 0 || strcmp(packet.command_choice,"withdrawal") == 0)
        {
            // Modify customer info in database
            for (int i = 0; i < num_of_customers; i++)
            {
                if (strcmp(customer_database[i].name, packet.customer_info.name) == 0)
                {
                    customer_database[i].balance = packet.customer_info.balance;

                    i = num_of_customers;
                }
            }
            printf(" server: customer info modified successfully\n");

            packet.req_res = 1; // response
            packet.succ_fail = 0; // success
            if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(struct Packet) )
                DieWithError( "server: sendto() sent a different number of bytes than expected" );
        }
        else
        {
            DieWithError(" server: shouldn't be here"); // never gets here
        }

    }
    // NOT REACHED */
}
