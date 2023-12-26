#pragma once

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
