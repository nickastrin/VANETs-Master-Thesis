// Defines unit type
enum class UnitType
{
    VEHICLE,
    RSU,
    ORIGIN
};

// Determines caching policy used
enum class CachingPolicy
{
    FIFO,
    LRU,
    LFU
};

// Used for origin information caching policy
enum class OriginPolicy
{
    PULL,
    PUSH
};

enum class Scenario 
{
    MACHINE_LEARNING,
    MANUAL_CENTRALITY
};

