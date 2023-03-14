// Implements the client side of an echo client-server application program.
//
// Compile on general.asu.edu as:
//   g++ -o client UDPEchoClient.c
//
// Only on general3 and general4 have the ports >= 1024 been opened for
// application programs.
#include <stdio.h> 
#include <stdbool.h>     // for printf() and fprintf()
#include <sys/socket.h> // for socket(), connect(), sendto(), and recvfrom()
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include "../include/defns.h"

void DieWithError( const char *errorMessage ) // External error handling function
{
    perror( errorMessage );
    exit(1);
}

int main( int argc, char *argv[] )
{
    size_t nread;
    int sock;                        // Socket descriptor
    struct sockaddr_in servAddr; // Server address
    struct sockaddr_in fromAddr; 
    unsigned short servPort;     // Echo server port
    char *servIP;                    // IP address of server
    int recvMsgSize;                 // Size of received message

    char* name;
    double balance;
    char* client_ip_address;
    int port_to_bank;
    int port_to_other_customers;

    int cohort_size;

    if (argc < 3)
    {
        fprintf( stderr, "Usage: %s <Server ip address> <Server port>\n", argv[0] );
        exit( 1 );
    }

    servIP = argv[ 1 ];
    servPort = atoi( argv[2] );

    printf( "client: Arguments passed: server IP %s, port %d\n", servIP, servPort );

    /*
    if (argv[0] == "open")    // Test for correct number of arguments
    {
        if (argc == 8)
        {
            struct CustomerInfo CustomerInfo = { argv[1], atof(argv[2]), argv[3], atoi(argv[4]), atoi(argv[5]) };

            servIP = argv[6];
            servPort = stoi(argv[7]);

            printf( "client: Arguments passed: name %s, balance %d, client ip address %s, port to bank %d, port to other CustomerInfos %d, server ip address %s, server port %d\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
        }
        else
        {
            fprintf( stderr, "Usage: %s <CustomerInfo-name> <balance> <client-ip-address> <port-to-bank> <port-to-other-CustomerInfos> <server-ip-address> <server-port>\n", argv[0] );
            exit( 1 );
        }
    }
    else if (argv[0] == "new-cohort")
    {
        if (argc == 5)
        {

        }
    }
    else if (argv[0] == "delete-cohort")
    {

    }
    else if (argv[0] == "exit")
    {

    }
    else
    {
        printf( "invalid command ");
        exit(1);
    }

    */
    
    // Create a datagram/UDP socket
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "client: socket() failed" );

    // Construct the server address structure
    memset( &servAddr, 0, sizeof( servAddr ) ); // Zero out structure
    servAddr.sin_family = AF_INET;                  // Use internet addr family
    servAddr.sin_addr.s_addr = inet_addr( servIP ); // Set server's IP address
    servAddr.sin_port = htons( servPort );      // Set server's port

    // Open an account
    printf( "Open an account\n" );

    int userInput;
    scanf("%d", &userInput);
    
    printf( "client: Enter your name\n");
    scanf("%s", &name);
    printf( "client: Enter your balance\n");
    scanf("%d", &balance);
    printf( "client: Enter your ip address\n");
    scanf("%d", &client_ip_address);
    printf( "client: Enter your port number to the bank\n");
    scanf("%d", &port_to_bank);
    printf( "client: Enter your port number to other customers\n");
    scanf("%d", &port_to_other_customers);

    struct Cohort cohort = { "", (struct CustomerInfo*)malloc(sizeof(struct CustomerInfo)), 0}; // null cohort struct
    struct CustomerInfo customer_info = { name, balance, client_ip_address, port_to_bank, port_to_other_customers, false};

    struct Packet packet = 
    {
        0, // request
        0, // status is not needed here
        "open",
        customer_info,
        cohort // cohort is not needed here
    };

    // Send the struct to the server
    if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &servAddr, sizeof( servAddr ) ) != sizeof(struct Packet) )
    {
        DieWithError( "client: sendto() sent a different number of bytes than expected" );
    }

    // Receive a response

    if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &fromAddr, sizeof( fromAddr ) ) ) > sizeof(struct Packet) )
        DieWithError( "client: recvfrom() failed" );

    if( servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr ) // from server
        DieWithError( "client: Error: received a packet from unknown source.\n" );

    if (packet.succ_fail == 0) // success
    {
        printf( "client: account created successfully\n" );
    }
    else // failure
    {
        DieWithError( "client: failed to create account\n");
    }


	// Command selection

    bool done = false;
    while( done == false )
    {
        printf( "client: Choose an action\n" );
        printf(" 1. Listen for packets\n");
        printf( "2. Create a cohort\n");
        printf( "3. Delete a cohort\n");
        printf( "4. Delete an account\n");
        printf( "5. Make a deposit\n");
        printf( "6. Make a withdrawal\n");
        printf( "7. Transfer money\n");

        scanf("%d", &userInput);
        switch(userInput)
        {
            // Listen for packets
            case 1:

            printf( "client: Listening for packets\n");
            if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &fromAddr, sizeof(fromAddr) )) < 0 )
            {
                DieWithError( "server: recvfrom() failed" );
            }

            if ( servAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr ) // packet is from server
            {
                if (strcmp(packet.command_choice,"new_cohort") == 0)
                {
                    cohort = packet.cohort;
                    printf( "client: joined a cohort\n");
                }
                
                if (strcmp(packet.command_choice,"delete_cohort") == 0)
                {
                    customer_info.in_cohort = false;
                    cohort.cohort_member_array = (struct CustomerInfo*)malloc(sizeof(struct CustomerInfo));
                    cohort.founder_name = "";
                    cohort.size = 0;
                    printf( "client: cohort disbanded\n");
                }
            }
            /*
            else if () // packet is from cohort members
            {

            }
            */
            else
            {
                DieWithError( "client: Error: received a packet from unknown source.\n" );
            }

            break;

            // Create a cohort
            case 2:

            printf( "client: Enter your name\n");
            scanf("%s", &name);
            printf( "client: Enter the total number of people in the cohort\n");
            scanf("%d", &cohort_size);
            while (cohort_size < 2)
            {
                printf( "client: invalid cohort size\n" );
                printf( "client: Enter the total number of people in the cohort\n" );
                scanf("%d", &cohort_size);
            }

            cohort.founder_name = name;
            cohort.size = cohort_size;

            packet = (struct Packet)
            {
                0,
                0,
                "new_cohort",
                customer_info,
                cohort
            };

            // Send the struct to the server
            if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &servAddr, sizeof( servAddr ) ) != sizeof(struct Packet) )
            {
                DieWithError( "client: sendto() sent a different number of bytes than expected" );
            }

            // Receive a response

            if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &fromAddr, sizeof(fromAddr) ) ) > sizeof(struct Packet) )
                DieWithError( "client: recvfrom() failed" );

            if( servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr ) // not from server
                DieWithError( "client: Error: received a packet from unknown source.\n" );
            
            if (packet.succ_fail == 0)
            {
                printf( "client: cohort created successfully\n" );
            }
            else
            {
                printf( "client: failed to create cohort\n");
            }

            break;

            // Delete a cohort
            case 3:

            printf( "client: Enter your name\n");
            scanf("%s", &name);

            packet = (struct Packet)
            {
                0,
                0,
                "delete_cohort",
                customer_info,
                cohort
            };

            // Send the struct to the server
            if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &servAddr, sizeof( servAddr ) ) != sizeof(struct Packet) )
            {
                DieWithError( "client: sendto() sent a different number of bytes than expected" );
            }

            // Receive a response

            if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &fromAddr, sizeof(fromAddr) ) ) > sizeof(struct Packet) )
                DieWithError( "client: recvfrom() failed" );

            if( servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr ) // not from server
                DieWithError( "client: Error: received a packet from unknown source.\n" );
            
            if (packet.succ_fail == 0)
            {
                printf( "client: cohort deleted successfully\n" );
            }
            else
            {
                printf( "client: failed to delete cohort\n");
            }

            // Delete an account
            case 4:

            printf( "client: Enter your name\n");
            scanf("%s", &name);

            packet = (struct Packet)
            {
                0,
                0,
                "exit",
                customer_info,
                cohort
            };

            // Send the struct to the server
            if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &servAddr, sizeof( servAddr ) ) != sizeof(struct Packet) )
            {
                DieWithError( "client: sendto() sent a different number of bytes than expected" );
            }

            // Receive a response

            if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &fromAddr, sizeof(fromAddr) ) ) > sizeof(struct Packet) )
                DieWithError( "client: recvfrom() failed" );

            if( servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr )
                DieWithError( "client: Error: received a packet from unknown source.\n" );
            
            if (packet.succ_fail == 0)
            {
                printf( "client: customer information deleted successfully\n" );
                done = true; // terminates the process
            }
            else
            {
                printf( "client: failed to delete customer information\n");
            }

            break;

            // Make a deposit
            case 5:

            if (customer_info.in_cohort == false)
            {
                printf(" client: customer is not in a cohort\n");
            }
            else
            {
                printf( "Enter amount to deposit\n" );
                scanf("%d", &userInput);
                
                customer_info.balance = customer_info.balance + userInput;

                // update bank database

                packet = (struct Packet)
                {
                    0, // request
                    0, // status is not needed here
                    "deposit",
                    customer_info,
                    cohort // cohort is not needed here
                };

                // Send the struct to the server
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &servAddr, sizeof( servAddr ) ) != sizeof(struct Packet) )
                {
                    DieWithError( "client: sendto() sent a different number of bytes than expected" );
                }

                // Receive a response

                if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &fromAddr, sizeof( fromAddr ) ) ) > sizeof(struct Packet) )
                    DieWithError( "client: recvfrom() failed" );

                if( servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr ) // from server
                    DieWithError( "client: Error: received a packet from unknown source.\n" );

                if (packet.succ_fail == 0) // success
                {
                    printf( " client: Successful deposit\n");
                }
                else // failure
                {
                    DieWithError( "client: deposit failed\n");
                }
            }

            break;

            case 6:

            if (customer_info.in_cohort == false)
            {
                printf( " client: customer is not in a cohort\n");
            }
            else
            {
                printf( "Enter amount to withdraw\n" );
                scanf("%d", &userInput);
                
                customer_info.balance = customer_info.balance - userInput;
                printf( " client: Successful withdrawal\n");

                // update bank database

                packet = (struct Packet)
                {
                    0, // request
                    0, // status is not needed here
                    "deposit",
                    customer_info,
                    cohort // cohort is not needed here
                };

                // Send the struct to the server
                if( sendto( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &servAddr, sizeof( servAddr ) ) != sizeof(struct Packet) )
                {
                    DieWithError( "client: sendto() sent a different number of bytes than expected" );
                }

                // Receive a response

                if( ( recvMsgSize = recvfrom( sock, &packet, sizeof(struct Packet), 0, (struct sockaddr *) &fromAddr, sizeof( fromAddr ) ) ) > sizeof(struct Packet) )
                    DieWithError( "client: recvfrom() failed" );

                if( servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr ) // from server
                    DieWithError( "client: Error: received a packet from unknown source.\n" );

                if (packet.succ_fail == 0) // success
                {
                    printf( " client: Successful withdrawal\n");
                }
                else // failure
                {
                    DieWithError( "client: withdrawal failed\n");
                }
            }

            break;
            
            case 7:

            if (customer_info.in_cohort == false)
            {
                printf( " client: customer is not in a cohort\n");
            }
            else
            {






            }

            break;

        }
    }
    
    close( sock );
    exit( 0 );
}
