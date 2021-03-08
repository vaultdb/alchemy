#ifndef _FIELD_UTILITIES_H
#define _FIELD_UTILITIES_H

#include<type_traits>

// from https://ideone.com/pldMrr
namespace  vaultdb::check {
        struct No {};

        template<typename T, typename Arg>
        No operator==(const T &, const Arg &);

        template<typename T, typename Arg = T>
        struct EqualExists {
            enum {
                value = !std::is_same<decltype(*(T *) (0) == *(Arg *) (0)), No>::value
            };
        };

    template<typename T, typename Arg>
    No operator+(const T &, const Arg &);

    template<typename T, typename Arg = T>
    struct PlusExists {
        enum {
            value = !std::is_same<decltype(*(T *) (0) + *(Arg *) (0)), No>::value
        };
    };
}
#endif //_FIELD_UTILITIES_H
