#pragma once
// see http://silentbreed.eljako.org/c++11/Singleton.pdf

#include <list>
#include <mutex>
using namespace std;



// ----------
//! \class		ISingleton
//! \brief		ISingleton [abstract] class
//! \details	A pure C++11 generic singleton interface
//! \author		ASintes
class ISingleton abstract
{
    friend class SingletonMgr; // dtor must be accessed by the manager
protected:
    // dtor
    virtual ~ISingleton()
    {}
};



// ----------
//! \class		SingletonMgr
//! \brief		SingletonMgr [sealed] class
//! \details	A pure C++11 generic singleton manager
//! \author		ASintes
class SingletonMgr sealed
{
private:
    // ctor (cannot be constructed)
    SingletonMgr()
    {}

    // dtor
    ~SingletonMgr()
    {
        // remove all known singletons in reversed registration order 
        for (auto pSingleton : m_lSingleton)
            delete pSingleton;
    }


public:
    // static registration of a singleton object
    static void Register(const ISingleton & _singleton)
    {

        // Scott Meyers singleton is safe in C++11 because of the local static thread safety
        static SingletonMgr instance;

        // lock access to the list and push the new singleton
        lock_guard< mutex > lock(instance.m_mtxSingleton);
        instance.m_lSingleton.push_front(&_singleton);
    }


private:
    mutex m_mtxSingleton;
    list< const ISingleton * > m_lSingleton;
};



// ----------
//! \class		Singleton
//! \brief		Singleton [abstract] [template] class
//! \details	A pure C++11 generic singleton object
//! \author		ASintes
template< class T >
class Singleton abstract: private ISingleton
{
protected:
    // ctor (just register in manager)
    Singleton()
    {
        SingletonMgr::Register(*this);
    }


public:
    // static instance getter
    static T & GetInstance()
    {
        // Scott Meyers singleton is safe in C++11 because of the local static thread safety
        static T * pInstance = new T();
        return *pInstance;
    }
};
