#define REQUEST 0
#define RESPONSE 1
#define SUCCESS 1
#define FAILURE 0

struct Packet
{
    int status;
    char* command_choice;
    struct CustomerInfo customer_info;
    struct Cohort cohort;

};
struct CustomerInfo
{
    char* name;
    double balance;
    char* client_ip_addr;
    int port_to_bank;
    int port_to_other_customers;
};

struct Cohort
{
    char* founder_name;
    int inCohort; // 0: true, 1: false
    struct CustomerInfo* customer_info_array;
    int size;
};

