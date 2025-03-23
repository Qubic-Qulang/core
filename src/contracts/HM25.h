using namespace QPI;

#define HASHMAP_SIZE 2 << 20 // 2^20=1 048 576 (1 million)
#define MAX_USERS 2 << 24 // 2^24=16 777 216 (16 million)
#define MAX_PROVIDERS 2 << 24 // 2^24=16 777 216 (16 million)
#define MAX_BURN_RATE 100 
#define MAX_REPUTATION 2 << 30 // 2^30=1 073 741 824 (1 billion)
#define MAX_PRICE_BY_TOKEN 2 << 30 // 2^30=1 073 741 824 (1 billion)
#define MAX_BALENCE 2 << 30 // 2^30=1 073 741 824 (1 billion)


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
    struct Topup_input {           
        uint64 initial_balance; 
    };
    struct Topup_output {};

    struct GetUser_input {
        id user_id;
    };
    struct GetUser_output {
        uint64 balance;         
    };

    struct DepositFunds_input {
        id user_id;
    };

    struct DepositFunds_output {
        uint64 new_balance;
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
        uint64 remaining_balance;
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

    
    QPI::Array< User, HASHMAP_SIZE> users;
    QPI::Array< Provider, HASHMAP_SIZE> providers;

    struct findEmptyUserSlot_input { };  // Pas d'input nécessaire
    struct findEmptyUserSlot_output { uint64 index; };

    PRIVATE_FUNCTION(findEmptyUserSlot)
    {
        output.index = NULL_INDEX;
        uint64 cap = state.users.capacity();
        for(uint64 i = 0; i < cap; ++i) {
            User u = state.users.get(i);
            if(u.user_id == NULL_ID) {
                output.index = i;
                break;
            }
        }
    }
    _

    struct findUserIndex_input { id user_id; };
    struct findUserIndex_output { uint64 index; };

    PRIVATE_FUNCTION(findUserIndex)
    {
        output.index = NULL_INDEX;
        uint64 cap = state.users.capacity();
        for(uint64 i = 0; i < cap; ++i) {
            User u = state.users.get(i);
            if(u.user_id == input.user_id) {
                output.index = i;
                break;
            }
        }
    }
    _

    struct findEmptyProviderSlot_input { };
    struct findEmptyProviderSlot_output { uint64 index; };

    PRIVATE_FUNCTION(findEmptyProviderSlot)
    {
        output.index = NULL_INDEX;
        uint64 cap = state.providers.capacity();
        for(uint64 i = 0; i < cap; ++i) {
            Provider p = state.providers.get(i);
            if(p.provider_id == NULL_ID) {
                output.index = i;
                break;
            }
        }
    }
    _


    struct findProviderIndex_input { id provider_id; };
    struct findProviderIndex_output { uint64 index; };

    PRIVATE_FUNCTION(findProviderIndex)
    {
        output.index = NULL_INDEX;
        uint64 cap = state.providers.capacity();
        for(uint64 i = 0; i < cap; ++i) {
            Provider p = state.providers.get(i);
            if(p.provider_id == input.provider_id) {
                output.index = i;
                break;
            }
        }
    }
    _
    

   PUBLIC_PROCEDURE(Topup)
   {

        findUserIndex_input fuInput;
        fuInput.user_id = qpi.invocator();
        findUserIndex_output fuOutput;


        CALL(findUserIndex, fuInput, fuOutput);
        if(fuOutput.index == NULL_INDEX) {
            findEmptyUserSlot_input fsInput2;
            findEmptyUserSlot_output fsOutput2;
            CALL(findEmptyUserSlot, fsInput2, fsOutput2);
            User u;
            u.user_id = qpi.invocator();
            u.balance = qpi.invocationReward();
            state.users.set(fsOutput2.index, u);
        } else {
        User u = state.users.get(fuOutput.index);
            u.balance = u.balance + qpi.invocationReward();
            state.users.set(fuOutput.index, u);
        }
        
   }
   _

   PUBLIC_PROCEDURE(DepositFunds)
   {
       findUserIndex_input fuInput;
       fuInput.user_id = input.user_id;
       findUserIndex_output fuOutput;
       CALL(findUserIndex, fuInput, fuOutput);
       if(fuOutput.index == NULL_INDEX) {
           qpi.__qpiAbort(2);
       }
       User u = state.users.get(fuOutput.index);
       u.balance += qpi.invocationReward();
       state.users.set(fuOutput.index, u);
       output.new_balance = u.balance;
   }
   _

   // Obtenir le solde d'un utilisateur
   PUBLIC_FUNCTION(GetUser)
   {
        // id uid = id(
        //     0x1d64a56ccf88cb0d,
        //     0xd3ed2db7170c2f51,
        //     0x562598ae7c8f9a27,
        //     0x5b04f91b2c5e83ef
        // );
        // uint64 balance = 56;
        // User u = {uid, balance};
        //state.users.set(0, u);

       findUserIndex_input fuInput;
       fuInput.user_id = input.user_id;
       findUserIndex_output fuOutput;



       CALL(findUserIndex, fuInput, fuOutput);
       if(fuOutput.index == NULL_INDEX) {
        output.balance = 0;
       } else {
        User u = state.users.get(fuOutput.index);
           output.balance = u.balance;
       }
        
    }   
   _

   // Enregistrer un provider
   PUBLIC_PROCEDURE(RegisterProvider)
   {
       findEmptyProviderSlot_input fpInput;
       findEmptyProviderSlot_output fpOutput;
       CALL(findEmptyProviderSlot, fpInput, fpOutput);
       if(fpOutput.index == NULL_INDEX) {
           qpi.__qpiAbort(3);
       }
       Provider p;
       p.provider_id = input.provider_id;
       p.burn_rate = input.burn_rate;
       p.price_input = input.price_input;
       p.price_output = input.price_output;
       p.reputation = input.reputation;
       state.providers.set(fpOutput.index, p);
   }
   _

   // Obtenir les infos d'un provider
  PUBLIC_FUNCTION(GetProvider)
   {
       findProviderIndex_input fpInput;
       fpInput.provider_id = input.provider_id;
       findProviderIndex_output fpOutput;
       CALL(findProviderIndex, fpInput, fpOutput);
       if(fpOutput.index == NULL_INDEX) {
           output.burn_rate = 0;
           output.price_input = 0;
           output.price_output = 0;
           output.reputation = 0;
       } else {
           Provider p = state.providers.get(fpOutput.index);
           output.burn_rate = p.burn_rate;
           output.price_input = p.price_input;
           output.price_output = p.price_output;
           output.reputation = p.reputation;
       }
   }
   _

   // Traiter une requête : déduction du coût, burn et transfert au provider
   PUBLIC_PROCEDURE(ProcessRequest)
   {
       // Recherche provider
       findProviderIndex_input fpInput;
       fpInput.provider_id = input.provider_id;
       findProviderIndex_output fpOutput;
       CALL(findProviderIndex, fpInput, fpOutput);
       if(fpOutput.index == NULL_INDEX) {
           output.remaining_balance = 0;
           return;
       }
       Provider p = state.providers.get(fpOutput.index);
       // Recherche utilisateur
       findUserIndex_input fuInput;
       fuInput.user_id = input.user_id;
       findUserIndex_output fuOutput;
       CALL(findUserIndex, fuInput, fuOutput);
       if(fuOutput.index == NULL_INDEX) {
           output.remaining_balance = 0;
           return;
       }
       User u = state.users.get(fuOutput.index);
       uint64 cost = input.token_count * p.price_output;
       if(u.balance < cost) {
           output.remaining_balance = u.balance;
           return;
       }
       if ((uint64)qpi.invocationReward() < cost) {
           output.remaining_balance = u.balance;
           return;
       }
       u.balance -= cost;
       state.users.set(fuOutput.index, u);
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

        REGISTER_USER_PROCEDURE(Topup, 1);
        REGISTER_USER_PROCEDURE(DepositFunds, 2);
        REGISTER_USER_FUNCTION(GetUser, 1);
        REGISTER_USER_PROCEDURE(RegisterProvider, 3);
        REGISTER_USER_FUNCTION(GetProvider, 2);
        REGISTER_USER_PROCEDURE(ProcessRequest, 4);
    _

    INITIALIZE

    {
        for (uint64 i = 0; i < state.users.capacity(); ++i) {
            User empty;
            empty.user_id = NULL_ID;
            empty.balance = 0;
            state.users.set(i, empty);
        }

        for (uint64 i = 0; i < state.providers.capacity(); ++i) {
            Provider empty;
            empty.provider_id = NULL_ID;
            empty.burn_rate = 0;
            empty.price_input = 0;
            empty.price_output = 0;
            empty.reputation = 0;
            state.providers.set(i, empty);
        }
    }
    _
};

