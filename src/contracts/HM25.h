using namespace QPI;

/****************************************************************************************/
/* @brief HM25 contract : This contract is using  to handle a peer to peer MarketPlace  */
/* of AI Models. The market place is allowed user to use the AI models by paying with  */
/* Tokens quibic.  The conctract handle all the transaction                            */
/***************************************************************************************/

// TODO:
// 1. Allow only the owner to use the ProcessRequest function 
// 2. Handle Errors et return the right error code

#define ARRAY_SIZE 2 << 20    // 2^20=1 048 576 (1 million)


struct HM252
{
};

struct HM25 : public ContractBase
{
public:
    // ─── USERS STRUCTURES ─────────────────────────────────────────────────────
    struct Topup_input
    {
    };
    struct Topup_output
    {
    };

    struct GetUser_input
    {
        id user_id;
    };
    struct GetUser_output
    {
        uint64 balance;
    };

    // ─── PROVIDER STRUCTURES ──────────────────────────────────────
    struct UpdateProvider_input
    {
        id provider_id;
        uint64 price_input;
        uint64 price_output;
        uint64 burn_rate;
        uint64 reputation;
    };
    struct UpdateProvider_output
    {
    };

    struct GetProvider_input
    {
        id provider_id;
    };
    struct GetProvider_output
    {
        uint64 price_input;
        uint64 price_output;
        uint64 burn_rate;
        uint64 reputation;
    };

    struct ProcessRequest_input
    {
        id provider_id;
        id user_id;
        uint64 token_input;
        uint64 token_output;
    };
    struct ProcessRequest_output
    {
        uint64 remaining_balance;
    };

    struct Withdraw_input
    {
        uint64 amount;
    };
    struct Withdraw_output
    {
    };

private:
    struct User
    {
        id user_id;
        uint64 balance;
    };

    struct Provider
    {
        id provider_id;
        uint64 price_input;
        uint64 price_output;
        uint64 burn_rate;
        uint64 reputation;
    };

    QPI::Array<User, ARRAY_SIZE> users;
    QPI::Array<Provider, ARRAY_SIZE> providers;

    struct findEmptyUserSlot_input
    {
    };
    struct findEmptyUserSlot_output
    {
        uint64 index;
    };

    /*
    * @brief Find the first empty slot in the users array
    */
    PRIVATE_FUNCTION(findEmptyUserSlot)
    {
        output.index = NULL_INDEX;
        uint64 cap = state.users.capacity();
        for (uint64 i = 0; i < cap; ++i)
        {
            User u = state.users.get(i);
            if (u.user_id == NULL_ID)
            {
                output.index = i;
                break;
            }
        }
    }
    _

    struct findUserIndex_input
    {
        id user_id;
    };
    struct findUserIndex_output
    {
        uint64 index;
    };

    /*
    * @brief Find the index of a user in the users array
    */
    PRIVATE_FUNCTION(findUserIndex)
    {
        output.index = NULL_INDEX;
        uint64 cap = state.users.capacity();
        for (uint64 i = 0; i < cap; ++i)
        {
            User u = state.users.get(i);
            if (u.user_id == input.user_id)
            {
                output.index = i;
                break;
            }
        }
    }
    _

    
    struct findEmptyProviderSlot_input
    {
    };
    struct findEmptyProviderSlot_output
    {
        uint64 index;
    };


    /*
     * @brief Find the first empty slot in the providers array
    */
    PRIVATE_FUNCTION(findEmptyProviderSlot)
    {
        output.index = NULL_INDEX;
        uint64 cap = state.providers.capacity();
        for (uint64 i = 0; i < cap; ++i)
        {
            Provider p = state.providers.get(i);
            if (p.provider_id == NULL_ID)
            {
                output.index = i;
                break;
            }
        }
    }
    _

    /*
    * @brief Find the index of a provider in the providers array
    */
    struct findProviderIndex_input
    {
        id provider_id;
    };
    struct findProviderIndex_output
    {
        uint64 index;
    };


    /*
     * @brief Find the index of a provider in the providers array
     */
    PRIVATE_FUNCTION(findProviderIndex)
    {
        output.index = NULL_INDEX;
        uint64 cap = state.providers.capacity();
        for (uint64 i = 0; i < cap; ++i)
        {
            Provider p = state.providers.get(i);
            if (p.provider_id == input.provider_id)
            {
                output.index = i;
                break;
            }
        }
    }
    _

    // ─── PUBLIC FUNCTIONS AND PROCEDURES ────────────────────────────────────────

    /*
     * @brief Add funds to the invocator's balance of users
     */
    PUBLIC_PROCEDURE(Topup)
    {

        findUserIndex_input fuInput;
        fuInput.user_id = qpi.invocator();
        findUserIndex_output fuOutput;

        CALL(findUserIndex, fuInput, fuOutput);
        if (fuOutput.index == NULL_INDEX)
        {
            findEmptyUserSlot_input fsInput2;
            findEmptyUserSlot_output fsOutput2;
            CALL(findEmptyUserSlot, fsInput2, fsOutput2);
            User u;
            u.user_id = qpi.invocator();
            u.balance = qpi.invocationReward();
            state.users.set(fsOutput2.index, u);
        }
        else
        {
            User u = state.users.get(fuOutput.index);
            u.balance = u.balance + qpi.invocationReward();
            state.users.set(fuOutput.index, u);
        }
    }
    _

    /*
    * @brief Withdraw funds from the invocator's balance of users
    */
    PUBLIC_PROCEDURE(Withdraw)
    {
        findUserIndex_input fuInput;
        fuInput.user_id = qpi.invocator();
        findUserIndex_output fuOutput;
        CALL(findUserIndex, fuInput, fuOutput);
        if (fuOutput.index == NULL_INDEX)
        {
            qpi.__qpiAbort(1);
            // TODO: return error
        }
        User u = state.users.get(fuOutput.index);
        qpi.transfer(qpi.invocator(), input.amount);
        if (u.balance < input.amount)
        {
            qpi.__qpiAbort(1);
        }
        u.balance -= input.amount;
        state.users.set(fuOutput.index, u);
    }
    _

    /*
     * @brief Get User Balance
     */
    PUBLIC_FUNCTION(GetUser)
    {
        findUserIndex_input fuInput;
        fuInput.user_id = input.user_id;
        findUserIndex_output fuOutput;

        CALL(findUserIndex, fuInput, fuOutput);
        if (fuOutput.index == NULL_INDEX)
        {
            output.balance = 0;
        }
        else
        {
            User u = state.users.get(fuOutput.index);
            output.balance = u.balance;
        }
    }
    _

    /*
     *@Brief Update Provider Details
     */
    PUBLIC_PROCEDURE(UpdateProvider)
    {
        findProviderIndex_input fpInput;
        fpInput.provider_id = qpi.invocator();
        findProviderIndex_output fpOutput;
        CALL(findProviderIndex, fpInput, fpOutput);
        if (fpOutput.index == NULL_INDEX)
        {

            findEmptyProviderSlot_input fpInput2;
            findEmptyProviderSlot_output fpOutput2;
            CALL(findEmptyProviderSlot, fpInput2, fpOutput2);

            Provider p;
            p.provider_id = qpi.invocator();
            p.burn_rate = input.burn_rate;
            p.price_input = input.price_input;
            p.price_output = input.price_output;
            p.reputation = input.reputation;
            state.providers.set(fpOutput2.index, p);

            // check if the provider is already a user if not add it to the users array
            findUserIndex_input fuInput;
            fuInput.user_id = qpi.invocator();
            findUserIndex_output fuOutput;

            CALL(findUserIndex, fuInput, fuOutput);
            if (fuOutput.index == NULL_INDEX)
            {
                findEmptyUserSlot_input fsInput2;
                findEmptyUserSlot_output fsOutput2;
                CALL(findEmptyUserSlot, fsInput2, fsOutput2);
                User u;
                u.user_id = qpi.invocator();
                u.balance = 0;
                state.users.set(fsOutput2.index, u);
            }          
        }
        else
        {
            findProviderIndex_input fpInput;
            fpInput.provider_id = qpi.invocator();
            findProviderIndex_output fpOutput;
            CALL(findProviderIndex, fpInput, fpOutput);
            Provider p = state.providers.get(fpOutput.index);
            p.burn_rate = input.burn_rate;
            p.price_input = input.price_input;
            p.price_output = input.price_output;
            state.providers.set(fpOutput.index, p);
        }
    }
    _


    /*
     * @brief Get Provider Details
     */
    PUBLIC_FUNCTION(GetProvider)
    {
        findProviderIndex_input fpInput;
        fpInput.provider_id = input.provider_id;
        findProviderIndex_output fpOutput;
        CALL(findProviderIndex, fpInput, fpOutput);
        if (fpOutput.index == NULL_INDEX)
        {
            output.burn_rate = 0;
            output.price_input = 0;
            output.price_output = 0;
            output.reputation = 0;
        }
        else
        {
            Provider p = state.providers.get(fpOutput.index);
            output.burn_rate = p.burn_rate;
            output.price_input = p.price_input;
            output.price_output = p.price_output;
            output.reputation = p.reputation;
        }
    }
    _

    /*
     * @brief Process Request : This the main function that process the request from the users
     * handle the token transfer and burn
     */
    PUBLIC_PROCEDURE(ProcessRequest)
    {
        findProviderIndex_input fpInput;
        fpInput.provider_id = input.provider_id;
        findProviderIndex_output fpOutput;
        CALL(findProviderIndex, fpInput, fpOutput);
        if (fpOutput.index == NULL_INDEX)
        {
            output.remaining_balance = 0;
            return;
        }
        Provider p = state.providers.get(fpOutput.index);

        findUserIndex_input fuInput;
        fuInput.user_id = input.user_id;
        findUserIndex_output fuOutput;
        CALL(findUserIndex, fuInput, fuOutput);
        if (fuOutput.index == NULL_INDEX)
        {
            output.remaining_balance = 0;
            return;
        }
        User u = state.users.get(fuOutput.index);

        uint64 cost = (input.token_output * p.price_input) + (input.token_input * p.price_output);
        uint64 burn_amount = (cost * p.burn_rate) / 10000;

        if (u.balance < cost)
        {
            // TODO: return error
            return;
        }
        else
        {
            u.balance -= cost;
            state.users.set(fuOutput.index, u);

            findUserIndex_input fuInput;
            fuInput.user_id = p.provider_id;
            findUserIndex_output fuOutput;
            CALL(findUserIndex, fuInput, fuOutput);

            User u_provider = state.users.get(fuOutput.index);
            u_provider.balance = cost - burn_amount + u_provider.balance;
            state.providers.set(fpOutput.index, p);
        }
        qpi.burn(burn_amount);
    }
    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES

    REGISTER_USER_PROCEDURE(Topup, 1);
    REGISTER_USER_PROCEDURE(Withdraw, 2);
    REGISTER_USER_FUNCTION(GetUser, 1);
    REGISTER_USER_PROCEDURE(UpdateProvider, 3);
    REGISTER_USER_FUNCTION(GetProvider, 2);
    REGISTER_USER_PROCEDURE(ProcessRequest, 4);
    _

    INITIALIZE

    {
        // Initialize the users and providers arrays : Set all the values to 0
        for (uint64 i = 0; i < state.users.capacity(); ++i)
        {
            User empty;
            empty.user_id = NULL_ID;
            empty.balance = 0;
            state.users.set(i, empty);
        }

        // Initialize the providers array : Set all the values to 0
        for (uint64 i = 0; i < state.providers.capacity(); ++i)
        {
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
