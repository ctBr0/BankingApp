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

    struct CustomerInfo CustomerInfo;
    struct CustomerInfo* CustomerInfoArray;
    int currentSize;
    int used = 0;
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

    CustomerInfoArray = (struct CustomerInfo*)malloc(0);

    for(;;) // Run forever
    {
        failed = false;
        cliAddrLen = sizeof( clientAddr );

        // Block until receive message from a client
        if( ( recvMsgSize = recvfrom( sock, &CustomerInfo, sizeof(struct CustomerInfo), 0, (struct sockaddr *) &clientAddr, &cliAddrLen )) < 0 )
            DieWithError( "server: recvfrom() failed" );

        for (int i = 0; i < used; i++)
        {
            if (CustomerInfoArray[i].name == CustomerInfo.name)
            {
                failed = true;
                break;
            }
        }

        if (failed == false)
        {
            CustomerInfoArray = (struct CustomerInfo*)realloc(CustomerInfoArray, currentSize + sizeof(struct CustomerInfo));
            currentSize += sizeof(struct CustomerInfo);
            CustomerInfoArray[used] = CustomerInfo;
            used++;

            printf( "server: new CustomerInfo added to database\n");

            // Send received datagram back to the client
            if( sendto( sock, SUCCESS, sizeof(SUCCESS), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(SUCCESS) )
                DieWithError( "server: sendto() sent a different number of bytes than expected" );

        }
        else
        {
            printf( "server: CustomerInfo already exists\n");

            // Send received datagram back to the client
            if( sendto( sock, FAILURE, sizeof(FAILURE), 0, (struct sockaddr *) &clientAddr, sizeof( clientAddr ) ) != sizeof(FAILURE) )
                DieWithError( "server: sendto() sent a different number of bytes than expected" );
        }

    }
    // NOT REACHED */
}
