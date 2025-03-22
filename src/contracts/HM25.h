using namespace QPI;

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
    // ─── STRUCTURES POUR LES UTILISATEURS ────────────────────────────────
    struct RegisterUser_input {
        id user_id;              // L'identité publique de l'utilisateur
        uint64 initial_balance;  // Son solde initial (en tokens IA)
    };
    struct RegisterUser_output {}; // Pas de sortie nécessaire pour une procédure

    struct GetUser_input {
        id user_id;
    };
    struct GetUser_output {
        uint64 balance;          // Le solde actuel de l'utilisateur
    };

    // ─── STRUCTURES POUR LES PROVIDERS ──────────────────────────────────────
    struct RegisterProvider_input {
        id provider_id;          // L'identité publique du fournisseur
        uint64 burn_rate;        // Taux de burn (pour certains mécanismes de tarification)
        uint64 price_input;      // Prix d'entrée pour accéder au modèle IA
        uint64 price_output;     // Prix de sortie ou résultat
        uint64 reputation;       // Réputation du provider
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
        id provider_id;   // Provider qui fournit le modèle LLM
        id user_id;       // Utilisateur qui a fait la requête (prompt)
        uint64 token_count; // Nombre de tokens utilisés dans la requête
    };
    struct ProcessRequest_output {
        uint64 total_price; // Prix total calculé (token_count * price_output)
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

    
    QPI::HashMap<id, User, 256> users;
    QPI::HashMap<id, Provider, 256> providers;

    
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
    PUBLIC_PROCEDURE(RegisterUser)
        {
            
            User u;
            u.user_id = input.user_id;
            u.balance = input.initial_balance;
        
            users.set(input.user_id, u);
        }
    _

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

    // ─── FONCTION : Traiter la requête et effectuer le paiement ───────────
    PUBLIC_FUNCTION(ProcessRequest)
        {
            Provider p;
            // Vérifier que le provider est enregistré
            if (!providers.get(input.provider_id, p))
            {
                output.total_price = 0;
                return;
            }
            
            // Calculer le prix total en multipliant le nombre de tokens par le price_output
            uint64 total_price = input.token_count * p.price_output;
            
            // Récupérer l'utilisateur qui a initié la requête
            User u;
            if (!users.get(input.user_id, u))
            {
                output.total_price = 0;
                return;
            }
            
            // Vérifier que l'utilisateur a suffisamment de fonds
            if (u.balance < total_price)
            {
                // Optionnel : on pourrait lancer une erreur ici
                output.total_price = 0;
                return;
            }
            
            // Déduire le montant du solde de l'utilisateur
            u.balance -= total_price;
            users.set(input.user_id, u);
            
            // Transférer le montant au provider
            qpi.transfer(p.provider_id, total_price);
            
            // Renvoyer le montant débité pour affichage côté interface web
            output.total_price = total_price;
        }
    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES

        // REGISTER_USER_PROCEDURE(Echo, 1);
        // REGISTER_USER_PROCEDURE(Burn, 2);

        // REGISTER_USER_FUNCTION(GetStats, 1);

        REGISTER_USER_PROCEDURE(RegisterUser, 1);
        REGISTER_USER_PROCEDURE(RegisterProvider, 2);
        REGISTER_USER_FUNCTION(GetUser, 3);
        REGISTER_USER_FUNCTION(GetProvider, 4);
        REGISTER_USER_FUNCTION(ProcessRequest, 5);
    _

    INITIALIZE

        users.reset();
        providers.reset();
        // state.numberOfEchoCalls = 0;
        // state.numberOfBurnCalls = 0;
    _
};

