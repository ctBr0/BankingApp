#define REQUEST 0
#define RESPONSE 1
#define SUCCESS 1
#define FAILURE 0

// packet.command_choice
#define OPENACCOUNT 0
#define LISTENSERVER 1
#define LISTENCOHORT 2
#define CREATECOHORT 3
#define DELETECOHORT 4
#define DELETEACCOUNT 5
#define DEPOSIT 6
#define WITHDRAW 7
#define TRANSFER 8
#define LOSTTRANSFER 9
#define CHECKPOINT 10
#define ROLLBACK 11

struct CustomerInfo
{
    char* name;
    int balance;
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
    int command_choice; // eg. "open", "new_cohort"
    struct CustomerInfo customer_info;
    struct Cohort cohort;
};

struct Transfer
{
    int transfer_amount;
    char* sender;
    char* receiver;
    int label;
};

struct Checkpoint
{
    int action; // 0: Take a tentative checkpoint, 1: Make tentative checkpoint permanent, 2: Undo tentative checkpoint
    int label;
};

struct Rollback
{
    int action; // 0: Prepare to rollback, 1: Rollback, 2: Do not rollback
    int label;
};

struct P2PPacket
{
    int choice; // 0: Transfer, 1: Checkpoint, 2: Rollback
    struct Transfer transfer_info;
    struct Checkpoint checkpoint_info;
    struct Rollback rollback_info;
};

// returns index of member
// returns size of array otherwise
int IsMember(char*, struct CustomerInfo*, int);