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

    int transfer_amount;
    char* receiver;

    if (argc < 3)
    {
        fprintf( stderr, "Usage: %s <Server ip address> <Server port>\n", argv[0] );
        exit( 1 );
    }

    servIP = argv[ 1 ];
    servPort = atoi( argv[2] );

    printf( "client: Arguments passed: server IP %s, port %d\n", servIP, servPort );
    
    // Create a datagram/UDP socket
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "client: socket() failed" );

    // Construct the server address structure
    memset( &servAddr, 0, sizeof( servAddr ) ); // Zero out structure
    servAddr.sin_family = AF_INET;                  // Use internet addr family
    servAddr.sin_addr.s_addr = inet_addr( servIP ); // Set server's IP address
    servAddr.sin_port = htons( servPort );      // Set server's port

    // Open an account
    printf( " client: Create an account\n" );
    
    printf( "client: Enter your name\n");
    scanf("%s", &name);
    printf( "client: Enter your balance\n");
    scanf("%d", &balance);
    printf( "client: Enter your ip address\n");
    scanf("%s", &client_ip_address);
    printf( "client: Enter your port number to the bank\n");
    scanf("%d", &port_to_bank);
    printf( "client: Enter your port number to other customers\n");
    scanf("%d", &port_to_other_customers);

    struct Cohort cohort = { "", (struct CustomerInfo*)malloc(sizeof(struct CustomerInfo)), 0}; // null cohort struct
    struct CustomerInfo customer_info = { name, balance, client_ip_address, port_to_bank, port_to_other_customers, false};

    struct Packet packet = 
    {
        htons(0), // request
        htons(0), // status is not needed here
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

    int userInput;
    bool done = false;
    while( done == false )
    {
        printf( "client: Choose an action\n" );
        printf(" 1. Listen for packets from server\n");
        printf(" 2. Listen for packets from cohort members\n");
        printf( "3. Create a cohort\n");
        printf( "4. Delete a cohort\n");
        printf( "5. Delete an account\n");
        printf( "6. Make a deposit\n");
        printf( "7. Make a withdrawal\n");
        printf( "8. Transfer money\n");
        printf( "9. Simulate lost transfer\n");
        printf( "10. Make a checkpoint\n");
        printf( "11. Rollback\n");

        scanf("%d", &userInput);
        switch(userInput)
        {
            // Listen for packets from server
            case 1:

            printf( "client: Listening for packets from server\n");
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
            else
            {
                DieWithError( "client: Error: received a packet from unknown source.\n" );
            }

            break;
            
            // Listen for packets from cohort members
            case 2:
















            printf( "client: Listening for packets from cohort members\n");
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
            else
            {
                DieWithError( "client: Error: received a packet from unknown source.\n" );
            }

            break;

            // Create a cohort
            case 3:

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
            case 4:

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
            case 5:

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
            case 6:

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

            // Make a withdrawal
            case 7:

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
            
            // Transfer money
            case 8:

            if (customer_info.in_cohort == false)
            {
                printf( " client: customer is not in a cohort\n");
            }
            else
            {
                printf( " client: showing members in cohort\n");

                for (int i = 1; i < cohort.size; i++)
                {
                    printf( "%s\n", cohort.cohort_member_array[i].name);
                }

                printf( " client: enter amount to transfer\n");
                scanf("%d", &transfer_amount);
                while (transfer_amount <= 0)
                {
                    printf( "client: invalid transfer amount\n");
                    printf( " client: enter amount to transfer\n");
                    scanf("%d", &transfer_amount);
                }
                printf( "client: enter the person you want to send the above amount to\n");
                scanf("%s", &receiver);
                while (!IsMember(receiver, cohort.cohort_member_array, cohort.size))
                {
                    printf( "client: member does not exist\n");
                    printf( "client: enter the person you want to send the above amount to\n");
                    scanf("%s", &receiver);
                }

            





            }

            break;

            // Simulate lost transfer
            case 9:

            if (customer_info.in_cohort == false)
            {
                printf( " client: customer is not in a cohort\n");
            }
            else
            {



            





            }

            break;

            // Make a checkpoint
            case 10:

            if (customer_info.in_cohort == false)
            {
                printf( " client: customer is not in a cohort\n");
            }
            else
            {



            





            }

            break;

            // Rollback
            case 11:

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

bool IsMember(char* input, struct CustomerInfo* array, int size)
{
    bool output = false;
    for (int i = 1; i < size; i++)
    {
        if (strcmp(input, array[i].name) == 0)
        {
            output = true;
            i = size;
        }
    }
    return output;
}