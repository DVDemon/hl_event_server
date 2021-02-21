#ifndef CACHE_H
#define CACHE_H

#include <ignite/thin/ignite_client.h>
#include <ignite/thin/ignite_client_configuration.h>

namespace database
{
    class Cache
    {
        private:
            Cache();
            ignite::thin::IgniteClient _client;
            ignite::thin::cache::CacheClient<long, std::string> _cache;
        public:
            static Cache get();
            ignite::thin::cache::CacheClient<long, std::string>& cache(); 

    };
}

#endif