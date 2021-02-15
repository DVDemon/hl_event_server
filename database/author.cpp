#include "author.h"
#include "database.h"
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database{

void Author::init(){
try{ 
 
        Poco::Data::Session session = database::Database::get().create_session_write();
        /*
        Statement drop_stmt(session);
        drop_stmt << "DROP TABLE IF EXISTS Author", now;*/
 
        // (re)create table
        Statement create_stmt(session);
        create_stmt << "CREATE TABLE IF NOT EXISTS `Author` (`id` INT NOT NULL AUTO_INCREMENT, `first_name` VARCHAR(256) NOT NULL,`last_name` VARCHAR(256) NOT NULL,`email` VARCHAR(256) NULL, `title` VARCHAR(1024) NULL,  PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));" , now;
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

Author Author::read_by_id(long id){
try{       
        Poco::Data::Session session = database::Database::get().create_session_read();
        Poco::Data::Statement select(session);
        Author a;
        select << "SELECT id, first_name, last_name, email, title FROM Author where id=?",
            into(a._id),
            into(a._first_name),
            into(a._last_name),
            into(a._email),
            into(a._title),
            use(id),
            range(0, 1); //  iterate over result set one row at a time

        if (!select.done()){
            select.execute();
            
        }

        return a;
    
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

std::vector<Author> Author::read_all(){
try{       
        Poco::Data::Session session = database::Database::get().create_session_read();
        Statement select(session);
        std::vector<Author> result;
        Author a;
        select << "SELECT id, first_name, last_name, email, title FROM Author",
            into(a._id),
            into(a._first_name),
            into(a._last_name),
            into(a._email),
            into(a._title),
            range(0, 1); //  iterate over result set one row at a time

        while (!select.done()){
            select.execute();
            result.push_back(a);
        }
        return result;
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

std::vector<Author> Author::search(std::string first_name,std::string last_name){
try{       
        Poco::Data::Session session = database::Database::get().create_session_read();
        Statement select(session);
        std::vector<Author> result;
        Author a;
        select << "SELECT id, first_name, last_name, email, title FROM Author where first_name LIKE ? and last_name LIKE ?",
            into(a._id),
            into(a._first_name),
            into(a._last_name),
            into(a._email),
            into(a._title),
            use(first_name),
            use(last_name),
            range(0, 1); //  iterate over result set one row at a time

        while (!select.done()){
            select.execute();
            result.push_back(a);
        }
        return result;
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

void Author::insert(){
    
    try{
        Poco::Data::Session session = database::Database::get().create_session_write();
        Statement insert(session);
        
        insert << "INSERT INTO Author (first_name,last_name,email,title) VALUES(?, ?, ?, ?)",
            use(_first_name),
            use(_last_name),
            use(_email),
            use(_title);

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


long         Author::get_id() const{
    return _id;
}

const std::string &Author::get_first_name() const{
    return _first_name;
}

const std::string &Author::get_last_name() const{
    return _last_name;
}

const std::string &Author::get_email() const{
    return _email;
}

const std::string &Author::get_title() const{
    return _title;
}

long         Author::id(){
    return _id;
}

std::string &Author::first_name(){
    return _first_name;
}

std::string &Author::last_name(){
    return _last_name;
}

std::string &Author::email(){
    return _email;
}

std::string &Author::title(){
    return _title;
}
}