/*! \file singleton.h
\brief A template for singleton classes.

A singleton class is a class where only <b>one</b> instance exists.

*/
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdlib>		// atexit
#include <typeinfo>

template< class T >
class Singleton {

public:
	typedef T InstanceType;

	static InstanceType * Instance();
	static void DestroySingleton();

private:
	Singleton() { }
	Singleton( const Singleton<T> & ) { }
	Singleton<T> & operator = (const Singleton & ) { }

	static InstanceType		*pInstance_;
	static bool				destroyed_;
};

template< class T >
typename Singleton<T>::InstanceType * Singleton<T>::pInstance_		= NULL;

template< class T >
bool Singleton<T>::destroyed_		= false;

template< class T >
typename Singleton<T>::InstanceType * Singleton<T>::Instance() {

	if( destroyed_ )
		throw std::runtime_error(std::string("singleton Singleton<") + typeid(T).name() + "> already destroyed");

	if( pInstance_ == NULL ) {
		pInstance_ = new InstanceType();
		std::atexit( DestroySingleton );	// register destroy function
	}
	return pInstance_;
}

template< class T >
void Singleton<T>::DestroySingleton() {

	if( pInstance_ ) {
		delete pInstance_;
		pInstance_ = NULL;
		destroyed_ = true;
	}
}

#endif
