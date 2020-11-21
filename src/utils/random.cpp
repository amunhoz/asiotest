
#include <utils/random.h>

//std::mt19937 urbg(std::random_device{}());
std::mt19937 urbg(std::random_device{}());
uint32_t gen_random_number()
{
    return  urbg();    
}

std::string gen_random_string( const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "!@#$%&*()_-=+{[}]/?:;<,>.|"
        ;
    std::string s;
    
    for (int i = 0; i < len; ++i) {
        s.push_back(alphanum[gen_random_number() % (sizeof(alphanum) - 1)]);
        //s[i] = alphanum[gen_random_number() % (sizeof(alphanum) - 1)];
    }    
    return s;
}


std::string gen_random_string_safe( const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "_-."
        ;
    std::string s;
    
    for (int i = 0; i < len; ++i) {
        s.push_back(alphanum[gen_random_number() % (sizeof(alphanum) - 1)]);
        //s[i] = alphanum[gen_random_number() % (sizeof(alphanum) - 1)];
    }    
    return s;
}
