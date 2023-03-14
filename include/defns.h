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
    struct CustomerInfo* cohort_member_array; // includes the founder
    int size;
};

struct Packet
{
    unsigned short req_res; // request = 0, response = 1
    unsigned short succ_fail; // success = 0, failure = 1
    char* command_choice; // eg. "open", "new_cohort"
    struct CustomerInfo customer_info;
    struct Cohort cohort;
};

struct TransferPacket
{

};