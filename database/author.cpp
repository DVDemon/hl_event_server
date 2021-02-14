#include "author.h"

namespace database{

void Author::init(){

}

Author Author::read_by_id(){

}

std::vector<Author> Author::read_all(){

}

std::vector<Author> Author::search(std::string first_name,std::string last_name){

}

void Author::insert(){

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