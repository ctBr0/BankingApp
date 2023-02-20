#define REQUEST 0
#define RESPONSE 1
#define SUCCESS 1
#define FAILURE 0

struct customer
{
    char* name;
    double balance;
    char* client_ip_addr;
    int port_to_bank;
    int port_to_other_customers;
};

