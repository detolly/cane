//
// Created by Thomas on 1/23/2021.
//

#ifndef CANE_SINGLETON_H
#define CANE_SINGLETON_H

template<typename T>
class Singleton {
private:
    friend T;

protected:
    static T m_static_instance;
public:
    static T& the() { return m_static_instance; };

};

template<typename T>
T Singleton<T>::m_static_instance;


#endif //CANE_SINGLETON_H
