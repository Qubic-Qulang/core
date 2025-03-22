using namespace QPI;

struct HM252
{
};

struct HM25 : public ContractBase
{
public:
    struct Echo_input{};
    struct Echo_output{};

    struct Burn_input{};
    struct Burn_output{};

    struct GetStats_input {};
    struct GetStats_output
    {
        uint64 numberOfEchoCalls;
        uint64 numberOfBurnCalls;

        bool operator==(const GetStats_output& other) const
        {
            return numberOfEchoCalls == other.numberOfEchoCalls &&
                   numberOfBurnCalls == other.numberOfBurnCalls;
        }

        bool operator!=(const GetStats_output& other) const
        {
            return !(*this == other);
        }
    };


private:
    uint64 numberOfEchoCalls;
    uint64 numberOfBurnCalls;
    QPI::Array<GetStats_output, 8> statsArray;

    GetStats_output addStats( uint64 a, uint64 b)
    {
        GetStats_output result;
        result.numberOfEchoCalls = a;
        result.numberOfBurnCalls = b;
        return result;
    }
    
    /**
    Send back the invocation amount
    */
    PUBLIC_PROCEDURE(Echo)
        GetStats_output stat = addStats(69, 420);
        state.statsArray.set(0, stat);
        state.numberOfEchoCalls = stat.numberOfEchoCalls;

        if (qpi.invocationReward() > 0)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
        }
    _

    /**
    * Burn all invocation amount
    */
    PUBLIC_PROCEDURE(Burn)
        state.numberOfBurnCalls++;
        if (qpi.invocationReward() > 0)
        {
            qpi.burn(qpi.invocationReward());
        }
    _

    PUBLIC_FUNCTION(GetStats)
        output.numberOfBurnCalls = state.numberOfBurnCalls;
        output.numberOfEchoCalls = state.numberOfEchoCalls;
    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES

        REGISTER_USER_PROCEDURE(Echo, 1);
        REGISTER_USER_PROCEDURE(Burn, 2);

        REGISTER_USER_FUNCTION(GetStats, 1);
    _

    INITIALIZE
        state.numberOfEchoCalls = 0;
        state.numberOfBurnCalls = 0;
    _
};
