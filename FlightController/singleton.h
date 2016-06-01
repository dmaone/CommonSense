#ifndef SINGLETON_H
#define SINGLETON_H

#include <QtGlobal>
#include <QScopedPointer>
#include "call_once.h"

template <class T>
class Singleton
{
public:
    static T& instance()
    {
        qCallOnce(init, flag);
        return *tptr;
    }

    static void init()
    {
        tptr.reset(new T);
    }

private:
    Singleton() {};
    ~Singleton() {};
    Q_DISABLE_COPY(Singleton)

    static QScopedPointer<T> tptr;
    static QBasicAtomicInt flag;
};

/* BUG (run-time error) : If an instance is get during an initialization of global or static data that
a Singleton<T>::instance() function can be call before a constructor of a Singleton<T>::tptr static
variable. As result, the tptr variable will be zero, but a Singleton<T>::flag will have a ECallOnce::CO_Finished
value, so a next call of QScopedPointer::operator * generates an run-time error. */
template<class T> QScopedPointer<T> Singleton<T>::tptr(0);
template<class T> QBasicAtomicInt Singleton<T>::flag = Q_BASIC_ATOMIC_INITIALIZER(CallOnce::CO_Request);

#endif // SINGLETON_H
