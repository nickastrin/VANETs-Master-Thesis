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

// Used for the test scenarios
enum class Scenario
{
    CENTRALITY,
    CACHE,
    ACKNOWLEDGEMENT
};

// Used for origin information caching policy
enum class OriginPolicy
{
    PULL,
    PUSH
};
