#define REQUEST 0
#define RESPONSE 1
#define SUCCESS 1
#define FAILURE 0

struct CustomerInfo
{
    char* name;
    double balance;
    char* client_ip_addr;
    unsigned short port_to_bank;
    unsigned short port_to_other_customers;
    bool in_cohort;
};

struct Cohort
{
    char* founder_name;
    struct CustomerInfo* cohort_member_array;
    int size;
};

struct Packet
{
    int req_res;
    int succ_fail;
    char* command_choice;
    struct CustomerInfo customer_info;
    struct Cohort cohort;
};