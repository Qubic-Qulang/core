using namespace QPI;

#define HASHMAP_SIZE 2 << 30 // 2^30=1 073 741 824 (1 billion)
#define MAX_USERS 2 << 24 // 2^24=16 777 216 (16 million)
#define MAX_PROVIDERS 2 << 24 // 2^24=16 777 216 (16 million)
#define MAX_BURN_RATE 100 
#define MAX_REPUTATION 2 << 30 // 2^30=1 073 741 824 (1 billion)
#define MAX_PRICE_BY_TOKEN 2 << 30 // 2^30=1 073 741 824 (1 billion)
#define MAX_BALENCE_2 2 << 40 // 2^40=1 099 511 627 776 (1 trillion)
#define

struct HM252
{
};

struct HM25 : public ContractBase
{
public:
    // struct Echo_input{};
    // struct Echo_output{};

    // struct Burn_input{};
    // struct Burn_output{};

    // struct GetStats_input {};


    // struct GetStats_output
    // {
    //     uint64 numberOfEchoCalls;
    //     uint64 numberOfBurnCalls;

    //     bool operator==(const GetStats_output& other) const
    //     {
    //         return numberOfEchoCalls == other.numberOfEchoCalls &&
    //                numberOfBurnCalls == other.numberOfBurnCalls;
    //     }

    //     bool operator!=(const GetStats_output& other) const
    //     {
    //         return !(*this == other);
    //     }
    // };
    // ─── USERS STRUCTURES ─────────────────────────────────────────────────────
    struct RegisterUser_input {
        id user_id;             
        uint64 initial_balance; 
    };
    struct RegisterUser_output {};

    struct GetUser_input {
        id user_id;
    };
    struct GetUser_output {
        uint64 balance;         
    };

    // ─── PROVIDER STRUCTURES ──────────────────────────────────────
    struct RegisterProvider_input {
        id provider_id;        
        uint64 burn_rate;        
        uint64 price_input;      
        uint64 price_output;     
        uint64 reputation;  
    };
    struct RegisterProvider_output {};

    struct GetProvider_input {
        id provider_id;
    };
    struct GetProvider_output {
        uint64 burn_rate;
        uint64 price_input;
        uint64 price_output;
        uint64 reputation;
    };

    struct ProcessRequest_input {
        id provider_id;
        id user_id;
        uint64 token_count;
    };
    struct ProcessRequest_output {
        uint64 total_price;
    };

private:
    // uint64 numberOfEchoCalls;
    // uint64 numberOfBurnCalls;

    // QPI::Array<GetStats_output, 256> statsArray;
    // QPI::Array<User, 1024> users;
    // QPI::Array<Provider, 1024> providers;
   
    /**
    Send back the invocation amount
    */
   struct User {
        id user_id;
        uint64 balance;
    };

    struct Provider {
        id provider_id;
        uint64 burn_rate;
        uint64 price_input;
        uint64 price_output;
        uint64 reputation;
    };

    
    QPI::HashMap<id, User, HASHMAP_SIZE> users;
    QPI::HashMap<id, Provider, HASHMAP_SIZE> providers;

    
    // PUBLIC_PROCEDURE(Echo)
    //     GetStats_output stat;
    //     stat.numberOfEchoCalls = 69;
    //     stat.numberOfBurnCalls = 89;
        
    //     state.statsArray.set(0, stat);
    //     state.statsArray.set(1, stat);

    //     state.numberOfEchoCalls = state.statsArray.capacity();

    //     if (qpi.invocationReward() > 0)
    //     {
    //         qpi.transfer(qpi.invocator(), qpi.invocationReward());
    //     }

    /**
     Register a new user
    */
    PUBLIC_PROCEDURE(RegisterUser)
        {
            
            User u;
            u.user_id = input.user_id;
            u.balance = input.initial_balance;
        
            users.set(input.user_id, u);
        }
    _

    /**
    * Get the balance of a user 
    */

    PUBLIC_FUNCTION(GetUser)
        {
            User u;
            
            if(users.get(input.user_id, u))
            {
                output.balance = u.balance;
            }
            else
            {
                output.balance = 0;
            }
        }

    // /**
    // * Burn all invocation amount
    // */
    // PUBLIC_PROCEDURE(Burn)
    //     state.numberOfBurnCalls++;
    //     if (qpi.invocationReward() > 0)
    //     {
    //         qpi.burn(qpi.invocationReward());
    //     }
    _


    /**
     * Register a new provider
     */

    PUBLIC_PROCEDURE(RegisterProvider)
        {
            Provider p;
            p.provider_id = input.provider_id;
            p.burn_rate = input.burn_rate;
            p.price_input = input.price_input;
            p.price_output = input.price_output;
            p.reputation = input.reputation;
            // Stocker dans la HashMap des providers
            providers.set(input.provider_id, p);
        }
    
    _
    // PUBLIC_FUNCTION(GetStats)
    //     output.numberOfBurnCalls = state.numberOfBurnCalls;
    //     output.numberOfEchoCalls = state.numberOfEchoCalls;
    // _
    
    /**
     * Get the provider information
     */
    PUBLIC_FUNCTION(GetProvider)
        {
            Provider p;
            
            if(providers.get(input.provider_id, p))
            {
                output.burn_rate = p.burn_rate;
                output.price_input = p.price_input;
                output.price_output = p.price_output;
                output.reputation = p.reputation;
            }
            else
            {
                output.burn_rate = 0;
                output.price_input = 0;
                output.price_output = 0;
                output.reputation = 0;
            }
        }
    _

    /**
     * Process a request and transfer the amount to the provider
     */

     PUBLIC_FUNCTION(ProcessRequest)
     {
         Provider p;
   
         if (!providers.get(input.provider_id, p))
         {
             output.remaining_balance = 0;
             return;
         }
         User u;
    
         if (!users.get(input.user_id, u))
         {
             output.remaining_balance = 0;
             return;
         }
     
         uint64 cost = input.token_count * p.price_output;
         
         if (u.balance < cost)
         {
             output.remaining_balance = u.balance;
             return;
         }
         
 
         if (qpi.invocationReward() < cost)
         {
             output.remaining_balance = u.balance;
             return;
         }
         

         u.balance -= cost;
         users.set(input.user_id, u);
         
         
         uint64 burn_amount = (cost * p.burn_rate) / 100;
         uint64 net_amount = cost - burn_amount;
     
         qpi.transfer(p.provider_id, net_amount);
         qpi.burn(burn_amount);
         
         
         output.remaining_balance = u.balance;
     }
    
    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES

        // REGISTER_USER_PROCEDURE(Echo, 1);
        // REGISTER_USER_PROCEDURE(Burn, 2);

        // REGISTER_USER_FUNCTION(GetStats, 1);

        REGISTER_USER_PROCEDURE(RegisterUser, 1);
        REGISTER_USER_PROCEDURE(RegisterProvider, 2);
        REGISTER_USER_FUNCTION(GetUser, 1);
        REGISTER_USER_FUNCTION(GetProvider, 2);
        REGISTER_USER_FUNCTION(ProcessRequest, 3);
    _

    INITIALIZE

        users.reset();
        providers.reset();
        // state.numberOfEchoCalls = 0;
        // state.numberOfBurnCalls = 0;
    _
};

