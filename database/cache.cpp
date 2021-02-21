#include "cache.h"
#include "../config/config.h"

namespace database
{
    Cache::Cache()
    {
        ignite::thin::IgniteClientConfiguration cfg;
        cfg.SetEndPoints(Config::get().get_cache_servers());
        try
        {
            _client = ignite::thin::IgniteClient::Start(cfg);
            _cache = _client.GetOrCreateCache<long, std::string>("authors");
        }
        catch (ignite::IgniteError err)
        {
            std::cout << "error:" << err.what() << std::endl;
            throw;
        }
    }

    Cache Cache::get()
    {
        static Cache instance;
        return instance;
    }

    ignite::thin::cache::CacheClient<long, std::string>& Cache::cache(){
        return _cache;
    }
}