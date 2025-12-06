using namespace QPI;

constexpr uint64 HM25_MAX_USERS = 131072;
constexpr uint64 HM25_MAX_WHITELIST = 1024;

struct HM252
{
};

struct HM25 : public ContractBase
{
public:
    struct Deposit_input
    {
        uint64 amount;
    };
    struct Deposit_output
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

    struct Debit_input
    {
        id user;
        uint64 amount;
    };
    struct Debit_output
    {
        uint64 new_user_balance;
        uint64 new_contract_balance;
    };
    
    struct WithdrawTo_input
    {
        id to;
        uint64 amount;
    };
    struct WithdrawTo_output
    {
        uint64 new_contract_balance;
    };

    struct Initialize_input
    {
    };
    struct Initialize_output
    {
    };

    struct AddToWhitelist_input
    {
        id address;
    };
    struct AddToWhitelist_output
    {
    };

    struct RemoveFromWhitelist_input
    {
        id address;
    };
    struct RemoveFromWhitelist_output
    {
    };

    struct GetBalance_input
    {
    };
    struct GetBalance_output
    {
        uint64 balance;
    };

    struct GetWhitelistedCount_input
    {
    };
    struct GetWhitelistedCount_output
    {
        uint64 count;
    };

    struct GetContractBalance_input
    {
    };
    struct GetContractBalance_output
    {
        uint64 balance;
    };

    struct IsWhitelisted_input
    {
        id address;
    };
    struct IsWhitelisted_output
    {
        bit is_whitelisted;
    };

    struct Burn_locals
    {
        bit isAdmin;
    };

    struct Debit_locals
    {
        bit isAdmin;
    };

    struct WithdrawTo_locals
    {
        bit isAdmin;
    };

    struct GetWhitelistedCount_locals
    {
        bit isAdmin;
    };

    struct GetContractBalance_locals
    {
        bit isAdmin;
    };

private:
    id _owner;
    bit _initialized;
    uint64 _contractBalance;
    
    HashMap<id, uint64, HM25_MAX_USERS> _userBalances;
    HashMap<id, bit, HM25_MAX_WHITELIST> _whitelist;
    uint64 _whitelistCount;

public:
    PUBLIC_PROCEDURE(Initialize)
        if (state._initialized)
        {
            return;
        }
        state._owner = qpi.invocator();
        state._whitelist.set(qpi.invocator(), true);
        state._whitelistCount = 1;
        state._contractBalance = 0;
        state._initialized = true;
    _

    PUBLIC_PROCEDURE(AddToWhitelist)
        if (qpi.invocator() != state._owner)
        {
            return;
        }
        bit existing = false;
        if (!state._whitelist.get(input.address, existing) || !existing)
        {
            state._whitelist.set(input.address, true);
            state._whitelistCount = state._whitelistCount + 1;
        }
    _

    PUBLIC_PROCEDURE(RemoveFromWhitelist)
        if (qpi.invocator() != state._owner)
        {
            return;
        }
        bit existing = false;
        if (state._whitelist.get(input.address, existing) && existing)
        {
            state._whitelist.removeByKey(input.address);
            state._whitelistCount = state._whitelistCount - 1;
        }
    _

    PUBLIC_PROCEDURE(Deposit)
        if (input.amount == 0)
        {
            return;
        }
        
        uint64 currentBalance = 0;
        state._userBalances.get(qpi.invocator(), currentBalance);
        currentBalance = currentBalance + input.amount;
        state._userBalances.set(qpi.invocator(), currentBalance);
        state._contractBalance = state._contractBalance + input.amount;
        output.new_balance = currentBalance;
    _

    PUBLIC_PROCEDURE_WITH_LOCALS(Burn)
        if (input.amount == 0)
        {
            return;
        }
        
        locals.isAdmin = false;
        state._whitelist.get(qpi.invocator(), locals.isAdmin);
        if (!locals.isAdmin)
        {
            return;
        }

        uint64 currentBalance = 0;
        state._userBalances.get(qpi.invocator(), currentBalance);

        if (currentBalance < input.amount)
        {
            return;
        }

        currentBalance = currentBalance - input.amount;
        state._userBalances.set(qpi.invocator(), currentBalance);
        qpi.burn(input.amount);
        output.new_balance = currentBalance;
    _

    PUBLIC_PROCEDURE_WITH_LOCALS(Debit)
        if (input.amount == 0)
        {
            return;
        }
        
        locals.isAdmin = false;
        state._whitelist.get(qpi.invocator(), locals.isAdmin);
        if (!locals.isAdmin)
        {
            return;
        }

        uint64 currentBalance = 0;
        state._userBalances.get(input.user, currentBalance);
        
        if (currentBalance < input.amount)
        {
            return;
        }
        
        currentBalance = currentBalance - input.amount;
        state._userBalances.set(input.user, currentBalance);
        
        output.new_user_balance = currentBalance;
        output.new_contract_balance = state._contractBalance;
    _

    PUBLIC_PROCEDURE_WITH_LOCALS(WithdrawTo)
        if (input.amount == 0)
        {
            return;
        }
        
        locals.isAdmin = false;
        state._whitelist.get(qpi.invocator(), locals.isAdmin);
        if (!locals.isAdmin)
        {
            return;
        }

        if (state._contractBalance < input.amount)
        {
            return;
        }
        
        state._contractBalance = state._contractBalance - input.amount;
        qpi.transfer(input.to, input.amount);
        output.new_contract_balance = state._contractBalance;
    _

    PUBLIC_FUNCTION(GetBalance)
        uint64 balance = 0;
        state._userBalances.get(qpi.invocator(), balance);
        output.balance = balance;
    _

    PUBLIC_FUNCTION_WITH_LOCALS(GetWhitelistedCount)
        locals.isAdmin = false;
        state._whitelist.get(qpi.invocator(), locals.isAdmin);
        if (!locals.isAdmin)
        {
            output.count = 0;
            return;
        }
        output.count = state._whitelistCount;
    _

    PUBLIC_FUNCTION_WITH_LOCALS(GetContractBalance)
        locals.isAdmin = false;
        state._whitelist.get(qpi.invocator(), locals.isAdmin);
        if (!locals.isAdmin)
        {
            output.balance = 0;
            return;
        }
        output.balance = state._contractBalance;
    _

    PUBLIC_FUNCTION(IsWhitelisted)
        bit isWhitelisted = false;
        state._whitelist.get(input.address, isWhitelisted);
        output.is_whitelisted = isWhitelisted;
    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        REGISTER_USER_PROCEDURE(Initialize, 1);
        REGISTER_USER_PROCEDURE(AddToWhitelist, 2);
        REGISTER_USER_PROCEDURE(RemoveFromWhitelist, 3);
        REGISTER_USER_PROCEDURE(Deposit, 4);
        REGISTER_USER_PROCEDURE(Burn, 5);
        REGISTER_USER_PROCEDURE(Debit, 6);
        REGISTER_USER_PROCEDURE(WithdrawTo, 7);

        REGISTER_USER_FUNCTION(GetBalance, 1);
        REGISTER_USER_FUNCTION(GetWhitelistedCount, 2);
        REGISTER_USER_FUNCTION(GetContractBalance, 3);
        REGISTER_USER_FUNCTION(IsWhitelisted, 4);
    _
};
