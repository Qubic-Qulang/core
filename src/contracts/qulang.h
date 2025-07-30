/******************************************************************************************/
/* @brief Qulang Contract                                                                 */
/*                                                                                        */
/*        Qulang is a blockchain token management contract that enables secure            */
/*        operations including deposits, withdrawals, and token burning.                  */
/*                                                                                        */
/*        Access control is implemented via a whitelist mechanism so that only            */
/*        authorized addresses can execute sensitive operations. The contract             */
/*        owner holds exclusive rights to modify the whitelist.                           */
/*                                                                                        */
/*        Error Codes:                                                                    */
/*          1 - InvalidInput:         The provided input is invalid.                      */
/*          2 - Unauthorized:         The invocator does not have permission.             */
/*          3 - InsufficientBalance:  The balance is insufficient for the request         */
/*          4 - NotWhitelisted:       The invocator is not whitelisted.                   */
/*          5 - AlreadyWhitelisted:   The address is already whitelisted.                 */
/******************************************************************************************/


using namespace QPI;

struct Qulang : public ContractBase
{
private:
    QPI::Map<id, id>    owner;
    QPI::Map<id, bool>  whitelist;
    QPI::Map<id, uint64> balances;

public:
    struct Deposit_input
    {
        uint64 amount;
    };
    struct Deposit_output
    {
        uint64 new_balance;
    };

    struct Withdraw_input
    {
        uint64 amount;
    };
    struct Withdraw_output
    {
        uint64 new_balance;
    };

    struct Burn_input
    {
        uint64 amount;
    };
    struct Burn_output
    {
        uint64 new_balance;
    };

    struct Initialize_input
    {
        // No input needed for initialization
    };
    struct Initialize_output
    {
        // No output needed for initialization
    };

    struct AddToWhitelist_input
    {
        id address;
    };

    struct AddToWhitelist_output
    {
        // No output needed for adding to whitelist
    };

    struct RemoveFromWhitelist_input
    {
        id address;
    };

    struct RemoveFromWhitelist_output
    {
        // No output needed for removing from whitelist
    };

    struct GetBalance_input
    {
        // No input needed for getting balance
    };
    struct GetBalance_output
    {
        uint64 balance;
    };

    struct GetWhitelistedAddresses_input
    {
        // No input needed for getting whitelisted addresses
    };

    struct GetWhitelistedAddresses_output
    {
        uint64 count;
        QPI::Vector<id> addresses;
    };

    PRIVATE_FUNCTION(ensureWhitelisted)
    {
        if (!state.whitelist.exists(qpi.invocator()) || !state.whitelist.get(qpi.invocator())) {
            qpi.__qpiAbort(4);
        }
    }
    _
    /*
    * @brief Initialize the contract state - used one Time
    */

    PUBLIC_PROCEDURE(Initialize)
    {
        // if owner is not set, set it to the invocator
        if (!state.owner.exists(id("owner"))) {
            state.owner.set(id("owner"), qpi.invocator());
            state.whitelist.set(qpi.invocator(), true);
        }
        else {
            qpi.__qpiAbort(1);
        }
    }
    _


    /*
     * @brief Add an address to the whitelist.
     */
    PUBLIC_PROCEDURE(AddToWhitelist)
    {

        if (qpi.invocator() != state.owner.get(id("owner"))) {
            qpi.__qpiAbort(2);
            return;
        }

        state.whitelist.set(input.address, true);
    }
    _

    /*
     * @brief Remove an address from the whitelist.
     */
    PUBLIC_PROCEDURE(RemoveFromWhitelist)
    {
        if (qpi.invocator() != state.owner.get(id("owner"))) {
            qpi.__qpiAbort(3);
            return;
        }
        state.whitelist.erase(input.address);
    }
    _


    /*
     * @brief Deposit funds into the contract.
     */
    PUBLIC_PROCEDURE(Deposit)
    {
        ensureWhitelisted();

        uint64 current_balance = 0;
        if(state.balances.exists(qpi.invocator()))
        {
            current_balance = state.balances.get(qpi.invocator());
        }
        current_balance += input.amount;
        state.balances.set(qpi.invocator(), current_balance);
        output.new_balance = current_balance;
    }
    _

    /*
     * @brief Withdraw funds from the contract.
     */
    PUBLIC_PROCEDURE(Withdraw)
    {
        ensureWhitelisted();
        
        uint64 current_balance = 0;
        if(state.balances.exists(qpi.invocator()))
        {
            current_balance = state.balances.get(qpi.invocator());
        }
        if (current_balance < input.amount)
        {
            qpi.__qpiAbort(1);
            return;
        }
        current_balance -= input.amount;
        state.balances.set(qpi.invocator(), current_balance);
        qpi.transfer(qpi.invocator(), input.amount);
        output.new_balance = current_balance;
    }
    _

    /*
     * @brief Burn funds from the contract.
     */
    PUBLIC_PROCEDURE(Burn)
    {
        ensureWhitelisted();

        uint64 current_balance = 0;
        if(state.balances.exists(qpi.invocator()))
        {
            current_balance = state.balances.get(qpi.invocator());
        }
        if (current_balance < input.amount)
        {
            qpi.__qpiAbort(1);
            return;
        }
        current_balance -= input.amount;
        state.balances.set(qpi.invocator(), current_balance);
        qpi.burn(input.amount);
        output.new_balance = current_balance;
    }
    _

    /*
     * @brief Get the balance of the invocator.
     */
    PUBLIC_FUNCTION(GetBalance)
    {
        ensureWhitelisted();

        if (state.balances.exists(qpi.invocator())) {
            output.balance = state.balances.get(qpi.invocator());
        } else {
            output.balance = 0;
        }
    }
    _

    /*
     * @brief Get list of whitelisted addresses.
     */
    PUBLIC_FUNCTION(GetWhitelistedAddresses)
    {
        ensureWhitelisted();

        output.addresses = state.whitelist.keys();
        output.count = output.addresses.size();
    }
    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES()
    {
        // procedures
        REGISTER_USER_PROCEDURE(Initialize, 1);
        REGISTER_USER_PROCEDURE(AddToWhitelist, 2);
        REGISTER_USER_PROCEDURE(RemoveFromWhitelist, 3);
        REGISTER_USER_PROCEDURE(Deposit, 4);
        REGISTER_USER_PROCEDURE(Withdraw, 5);
        REGISTER_USER_PROCEDURE(Burn, 6);

        // functions
        REGISTER_USER_FUNCTION(GetBalance, 1);
        REGISTER_USER_FUNCTION(GetWhitelistedAddresses, 2);
    
    }
    _
    
};

