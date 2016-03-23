#pragma once
#include <atomic>
namespace MIDI2LR
{
    template <typename T>
    struct counter
    {
        static std::atomic_int objects_created;
        static std::atomic_int objects_alive;

        counter()
        {
            ++objects_created;
            ++objects_alive;
        }

        counter(const counter&)
        {
            ++objects_created;
            ++objects_alive;
        }
    protected:
        ~counter() // objects should never be removed through pointers of this type
        {
            --objects_alive;
        }
    };
    template <typename T> std::atomic_int counter<T>::objects_created(0);
    template <typename T> std::atomic_int counter<T>::objects_alive(0);

}
/*
Usage:

class X :  MIDI2LR::counter<X>
{
// ...
};


*/
