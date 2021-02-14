#include <set>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <ignite/thin/ignite_client.h>
#include <ignite/thin/ignite_client_configuration.h>

using namespace ignite::thin;

#include <exception>
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>


using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

struct Person
{
    std::string name;
    std::string address;
    int         age;
};

void TestMySQL(){
    
    // create a session
    try
    {
        std::string str = "host=127.0.0.1;user=test;db=hl;password=pzjqUkMnc7vfNHET;";
        Poco::Data::Session session(Poco::Data::SessionFactory::instance().create(Poco::Data::MySQL::Connector::KEY, str ));
    
        Statement drop_stmt(session);
        drop_stmt << "DROP TABLE IF EXISTS Person", now;

        // (re)create table
        Statement create_stmt(session);
        create_stmt << "CREATE TABLE Person (Name VARCHAR(30), Address VARCHAR(255), Age INTEGER(3))", now;

        // insert some rows
        Person person = 
        {
            "Bart Simpson",
            "Springfield",
            12
        };

        Statement insert(session);
        insert << "INSERT INTO Person VALUES(?, ?, ?)",
            use(person.name),
            use(person.address),
            use(person.age);

        insert.execute();

        person.name    = "Lisa Simpson";
        person.address = "Springfield";
        person.age     = 10;

        insert.execute();

    // a simple query
        Statement select(session);
        select << "SELECT Name, Address, Age FROM Person",
            into(person.name),
            into(person.address),
            into(person.age),
            range(0, 1); //  iterate over result set one row at a time

        while (!select.done())
        {
            select.execute();
            std::cout << person.name << " " << person.address << " " << person.age << std::endl;
        }
    
    }
    catch (Poco::Data::MySQL::ConnectionException& e)
    {
        std::cout << "connection:" << e.what() << std::endl;
        return ;
    }
    catch(Poco::Data::MySQL::StatementException& e)
    {
        
        std::cout << "statement:" << e.what() << std::endl;
        return ;
    }
}

void TestClient()
{
    IgniteClientConfiguration cfg;

    //Endpoints list format is "<host>[port[..range]][,...]"
    cfg.SetEndPoints("127.0.0.1:10800,192.168.40.129:10800");

    IgniteClient client = IgniteClient::Start(cfg);

    cache::CacheClient<int32_t, std::string> cacheClient =
        client.GetOrCreateCache<int32_t, std::string>("TestCache");

    cacheClient.Put(42, "Hello Ignite Thin Client!");
    cacheClient.RefreshAffinityMapping();

    // Getting a value
    std::string val = cacheClient.Get(42);

    std::cout << val << std::endl;
}

#define COUNT 1000

Person generate_person(){
    Person result { std::string(10,' '),std::string(100,' '),0};

    auto func = [](std::string& val){
                    std::generate(std::begin(val),
                                  std::end(val),
                                    [](){
                                        return 'a'+std::rand()%('Z'-'a');
                                    });
    };

    func(result.name);
    func(result.address);
    result.age = std::rand()%120;


    return result;
}

Poco::Data::Session init_database(){
    try{
        std::string str = "host=127.0.0.1;user=test;db=hl;password=pzjqUkMnc7vfNHET;";
        Poco::Data::Session session(Poco::Data::SessionFactory::instance().create(Poco::Data::MySQL::Connector::KEY, str ));
    
        Statement drop_stmt(session);
        drop_stmt << "DROP TABLE IF EXISTS Person", now;
        Statement create_stmt(session);
        create_stmt << "CREATE TABLE Person (Name VARCHAR(30) NOT NULL, Address VARCHAR(255), Age INTEGER(3), PRIMARY KEY (`Name`))", now;
        return session;
    }
    catch (Poco::Data::MySQL::ConnectionException& e){
        std::cout << "connection:" << e.what() << std::endl;
        throw;
    }
    catch(Poco::Data::MySQL::StatementException& e){
        std::cout << "statement:" << e.what() << std::endl;
        throw;
    }
}

void insert_record(Poco::Data::Session& session, Person& person){
    try{
        
        Statement insert(session);
        insert << "INSERT INTO Person VALUES(?, ?, ?)",
            use(person.name),
            use(person.address),
            use(person.age);

        insert.execute();
    
    }
    catch (Poco::Data::MySQL::ConnectionException& e){
        std::cout << "connection:" << e.what() << std::endl;
        throw ;
    }
    catch(Poco::Data::MySQL::StatementException& e){
        
        std::cout << "statement:" << e.what() << std::endl;
        throw ;
    }
}

void read_record(Poco::Data::Session& session, std::string & name, Person& person){
    try{       
        Statement select(session);
        select << "SELECT Name, Address, Age FROM Person WHERE Name=?",
            into(person.name),
            into(person.address),
            into(person.age),
            use(name),
            range(0, 1); //  iterate over result set one row at a time

        if (!select.done())
            select.execute();
    
    }

    catch (Poco::Data::MySQL::ConnectionException& e){
        std::cout << "connection:" << e.what() << std::endl;
        throw ;
    }
    catch(Poco::Data::MySQL::StatementException& e){
        
        std::cout << "statement:" << e.what() << std::endl;
        throw ;
    }
}

void cache_person(cache::CacheClient<std::string, std::string> &cache,Person &person){
    Poco::JSON::Object obj;

    obj.set("name",person.name);
    obj.set("address",person.address);
    obj.set("age",person.age);

    std::ostringstream oss;
    Poco::JSON::Stringifier::stringify(obj, oss);      
    cache.Put(person.name,oss.str());
}

void read_cache_person(cache::CacheClient<std::string, std::string> &cache,std::string &name,Person &person){
    Poco::JSON::Parser parser;
    std::string json = cache.Get(name);
    Poco::Dynamic::Var result = parser.parse(json);
    Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();


    person.name = object->get("name").convert<std::string>();
    person.address = object->get("address").convert<std::string>();
    person.age = object->get("age").convert<int>();
}

int main()
{
    Poco::Data::MySQL::Connector::registerConnector();
    std::srand(std::time(nullptr));

    IgniteClientConfiguration cfg;
    cfg.SetEndPoints("192.168.40.129:10800");
    IgniteClient client = IgniteClient::Start(cfg);

    cache::CacheClient<std::string, std::string> cacheClient = client.GetOrCreateCache<std::string, std::string>("PersonCache");

    std::set<std::string> index;

    Poco::Data::Session session = init_database();
    for(size_t i=0;i<COUNT;++i){
        Person person = generate_person();
        if(index.insert(person.name).second){
           // insert_record(session,person);
            cache_person(cacheClient,person);
        }
    }
    cacheClient.RefreshAffinityMapping();


    auto start = std::chrono::high_resolution_clock::now();
    /*
    for(std::string s : index){
        Person person;
        read_record(session,s,person);
        ///std::cout << person.name << ", "<< person.address << ", " << person.age << std::endl;
    }*/
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "duration:" << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for(std::string s : index){
        Person person;
        read_cache_person(cacheClient,s,person);
        //std::cout << person.name << ", "<< person.address << ", " << person.age << std::endl;
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "duration:" << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << std::endl;

//    TestMySQL();
//    TestClient();
    return 0;
}