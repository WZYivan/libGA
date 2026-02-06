#ifndef M_libga_impl_concepts
#define M_libga_impl_concepts

#include <iostream>

#include <lga/Base>

M_libga_begin

    namespace concepts
{
    template <typename T>
    concept Can_Output_To_Ostream = requires(T t) {
        { std::cout << t };
    };

    template <typename T>
    concept Can_Access_Begin_End = requires(T t) {
        t.begin();
        t.end();
    };
}

M_libga_end

#endif