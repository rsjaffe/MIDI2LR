#include "Singleton.h"
// simplest form of threading: one-shot threaded computation
class MySingleton sealed: public Singleton< MySingleton >
{
    friend class Singleton< MySingleton >; // ctor must be accessed by the singleton
private:
    // ctor (cannot be constructed)
    MySingleton()
    {}
public:
    void Method()
    {}
};

void Dummy()
{
    // access to unique instance of MySingleton object and call Method()
    MySingleton::GetInstance().Method();
}
