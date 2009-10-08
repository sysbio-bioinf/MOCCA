/*
 * error_handler.h
 *
 * written by Andre Mueller
 *
 * C++ error reporting and handling module.
 * This module should not depend on other modules of the application.
 *
 * $Id$
 *
 * $Log$
 *
 */

/*! \file error_handler.h
\brief Central error handling base.

*/

#ifndef __ERROR_HANDLER_H__
#define __ERROR_HANDLER_H__

#include <string>
#include <exception>
#include <stdexcept>
#include <ctime>
#include <deque>
#include <map>
#include <list>

#include <sys/types.h>		// pid_t

using namespace std;

#include "singleton.h"

// unused parameter
#define UPARAM( x )

typedef unsigned long MessageID;

// global help functions
const char * timetostr( time_t t );
const char * timestamp();

struct ErrorMessage {

	time_t			timestamp;		// when did the message/error occur
	string			exclass;		// class the exception
	string			exid;			// exception id
	string			context;		// context (e.g. object::function() ) where the error occured
	string			text;			// additional information text
	string			file;			// where did the exception occur
	string			description;	// what did occurs (see "errors.txt")
	int				line;			// ....

	pid_t			pid;			// process id
	MessageID		id;				// sequence number

	string	asString() const;
};


// Exception hierarchy
//
class BaseException {

public:
	BaseException( const string & exid, const string & context, const string & text, const string &  file, int line );
	virtual ~BaseException() { }

	virtual const char * name() { return "BaseException"; }

	const ErrorMessage & message() const { return message_; }
	string what() const { return message_.asString(); }

	const string & GetExClass() const { return message_.exclass; }
	const string & GetExID() const { return message_.exid; }

private:
	ErrorMessage		message_;
};

// Wrapper for catch( ... ) exceptions ...
//
class UnexpectedException : public BaseException {

public:
	UnexpectedException( const string & context , const string & file, int line ) :
	  BaseException( "EX_UNEXPECTED", context, string(), file, line ) { }
	  
	virtual const char * name() { return "UnexpectedException"; }
};

// Wrapper for STD exceptions
//
class StandardException : public BaseException {

public:
	StandardException( const string & exid, const string & context = "", const string & file = "", int line = 0 ) :
	  BaseException( exid, context, "", file, line ) { }

	explicit StandardException( const exception & e, const string & context = "", const string & file = "", int line = 0 ) :
	  BaseException( "EX_STD_EXCEPTION", context, e.what(), file, line ) { }

	virtual const char * name() { return "StandardException"; }
};


class RuntimeError : public BaseException {

public:
	RuntimeError( const string & exid, const string & context = "", const string & text = "", const string & file = "", int line = 0) :
	  BaseException( exid, context, text, file, line ) { }
	  
	virtual const char * name() { return "RuntimeError"; }
};

class IOError: public RuntimeError {

public:
	IOError( const string & exid, const string & context = "", const string & text = "", const  string & file = "", int line = 0 ) :
	  RuntimeError( exid, context, text, file, line ) { }
	  
  	virtual const char * name() { return "IOError"; }
};


// assertion errors
//
class AssertionError: public RuntimeError {

public:
	AssertionError( const string & exid, const string & context , const string & text, const string & file, int line ) :
	  RuntimeError( exid, context, text, file, line ) { }
	  
  	virtual const char * name() { return "AssertionError"; }	  
};

// Base for application errors
//
class ApplicationError : public BaseException {

public:
	ApplicationError( const string & exid, const string & context = "", const string & text = "", const string & file = "", int line = 0) :
	  BaseException( exid, context, text, file, line ) { }

	virtual const char * name() { return "ApplicationError"; }
};

//
// Observer pattern
//
template< class Message >
class Observer {

public:
	typedef Message	message_type;

	virtual void Notify( const message_type & msg ) = 0;
};

template< class Message >
class Notifier {

public:
	typedef Message	message_type;
	typedef Observer< Message >	observer_type;

	Notifier() { }
	virtual ~Notifier() { }

	void Attach( observer_type & ob );
	void Detach( observer_type & ob );

	void Notify( const message_type & msg );

private:
	typedef list< observer_type * >	Observers;

	Observers		_observers;
};

// MessageQueue
//
// Implements a queue which saves the notification messages
//
class MessageQueue : public Observer< ErrorMessage > {

public:
	MessageQueue( unsigned int maxsize = 256 );
	virtual ~MessageQueue() { }

	// messaging
	virtual void Notify( const message_type & msg );

	// ACCESSORS
	bool isEmpty() const { return mqueue_.empty(); }
	int size() const { return mqueue_.size(); }
	const ErrorMessage & Top() const { return mqueue_.front(); }
	bool Exists( MessageID id ) const;
	const ErrorMessage & Find( MessageID id ) const;

	// MUTATORS
	void Clear();
	bool Push( const ErrorMessage & msg );
	void Pop() { mqueue_.pop_front(); }
	bool Remove( MessageID id );


private:
	typedef deque< ErrorMessage >	MsgQueue;

	unsigned int					maxsize_;
	MsgQueue						mqueue_;
};


// FileLogger
//
//  Reports error messages to a file
//
class FileLogger : public Observer< ErrorMessage > {

public:
	FileLogger( const string & filename = "" );
	virtual ~FileLogger() { }

	// messaging
	virtual void Notify( const message_type & msg );

	// ACCESSORS
	bool IsActive() const { return active_; }
	bool HasFailed() const { return failure_; }

	// MUTATORS
	void SetFileName( const string & filename = "" );
	void Activate() { active_ = true; }
	void Deactivate() { active_ = false; }

private:
	bool		active_;
	bool		failure_;
	string		filename_;
};


// ErrorMapper
//
// Maps error codes [exid]'s to a readable string
//
class ErrorMapper {

public:

	ErrorMapper() { }
	virtual ~ErrorMapper() { }

	bool Exists( const string & exid ) const;
	const string & Lookup( const string & exid ) const;

	bool LoadFromFile( const string & fname );

private:
	typedef map< string, string >	ErrorMap;

	ErrorMap			_map;
};

// ErrorHandler (Singleton)
//
// Centralized error handler
//
//
//
class BasicErrorHandler {

public:
	BasicErrorHandler();
	virtual ~BasicErrorHandler();

	// ACCESSORS
	MessageQueue & GetQueue() { return ob_queue_; }
	FileLogger & GetFileLogger() { return ob_file_; }
//	ErrorMapper & GetErrorMapper() { return mapper_; }
	bool LoadErrorFile( const string & errfile ) { return mapper_.LoadFromFile(errfile); }

	// MUTATORS

	// basic access point
	MessageID ReportMessage( const ErrorMessage & msg );

	// exception reporting
	MessageID ReportException( const BaseException & ex, const char * context = NULL, const char *file = NULL, int line = 0 );
	MessageID ReportException( const exception & ex, const char * context = NULL, const char *file = NULL, int line = 0 );
	MessageID ReportUnexpected( const char * context = NULL, const char *file = NULL, int line = 0 );

	// extended functions
	MessageID ReportError( const char * exid, const char * context, const char * text, const char *file = NULL, int line = 0 );
	MessageID ReportInfo( const char * context, const char * text, const char *file = NULL, int line = 0 );
	MessageID ReportDebug( const char * context, const char * text, const char *file = NULL, int line = 0 );

	// access point for
	MessageID CreateMessageID();
	MessageID GetMessageID() const { return current_id_; }
	string GetDescription( const string & exid ) const;

private:
	MessageID				current_id_;
	Notifier<ErrorMessage>	notifier_;
	ErrorMapper				mapper_;

	// Observers
	MessageQueue			ob_queue_;
	FileLogger				ob_file_;
};

typedef Singleton< BasicErrorHandler >	ErrorHandler;

// ErrorDetector (Singleton)
//
/*
class ErrorDetector {

public:
	static ErrorDetector * Instance();

	void Check( const char * descr, bool condition, const char * fname, int line );
	void CheckPrecondition( const char *, bool condition, const char * fname, int line );
	void CheckPostcondition( const char *, bool condition, const char * fname, int line );
	void CheckInvariant( const char *, bool condition, const char * fname, int line );

protected:
	ErrorDetector();
	virtual ~ErrorDetector();

private:
	static ErrorDetector	*instance_;
};

#define _CHECK( condition ) \
	ErrorDetector::Instance()->Check( #condition, (condition), __FILE__, __LINE__ );

#define _CHECK_PRE( condition ) \
	ErrorDetector::Instance()->CheckPrecondition( #condition, (condition), __FILE__, __LINE__ );

#define _CHECK_POST( condition ) \
	ErrorDetector::Instance()->CheckPostcondition( #condition, (condition), __FILE__, __LINE__ );

#define _CHECK_INV( condition ) \
	ErrorDetector::Instance()->CheckInvariant( #condition, (condition), __FILE__, __LINE__ );
*/

// Assertions
//
#ifdef DISABLE_ASSERTIONS

// NO ASSERTIONS
#define _Assert( condition , context )
#define _AssertParam( condition , context )
#define _AssertPreCond( condition , context )
#define _AssertPostCond( condition , context )
#define _AssertInvariant( condition , context )


#else

// ASSERTIONS ENABLED

#define _Assert( condition , context ) \
	if( ! (condition) ) \
		throw AssertionError( "EX_ASSERT", context, #condition, __FILE__, __LINE__ );

#define _AssertParam( condition , context ) \
	if( ! (condition) ) \
		throw AssertionError( "EX_INVALID_PARAMETER", context, #condition, __FILE__, __LINE__ );

#define _AssertPreCond( condition , context ) \
	if( ! (condition) ) \
		throw AssertionError( "EX_VIOLATED_PRECONDITION", context, #condition, __FILE__, __LINE__ );

#define _AssertPostCond( condition , context ) \
	if( ! (condition) ) \
		throw AssertionError( "EX_VIOLATED_POSTCONDITION", context, #condition, __FILE__, __LINE__ );

#define _AssertInvariant( condition , context ) \
	if( ! (condition) ) \
		throw AssertionError( "EX_VIOLATED_INVARIANT", context, #condition, __FILE__, __LINE__ );
#endif

//
// Exceptions macros
//
#define _EX_THROWT(Ex, exid, context, text ) \
	throw Ex( #exid, context, text, __FILE__, __LINE__ );

#define _EX_THROW(Ex, exid, context ) \
	throw Ex( #exid, context, "", __FILE__, __LINE__ );


#define _EX_STD_CATCH \
	catch( BaseException & ex ) { \
		ErrorHandler::Instance()->ReportException( ex ); \
		throw; \
	} \
	catch( exception & ex ) { \
		ErrorHandler::Instance()->ReportException( ex ); \
		throw StandardException( ex, "",  __FILE__, __LINE__ ); \
	} \
	catch( ... ) { \
		throw UnexpectedException( "", __FILE__, __LINE__ ); \
	}

#define _EX_STD_CATCH_WITH_RETURN( context ) \
	catch( BaseException & ex ) { \
		return ErrorHandler::Instance()->ReportException( ex , context, __FILE__, __LINE__ ); \
	} \
	catch( exception & ex ) { \
		return ErrorHandler::Instance()->ReportException( ex , context, __FILE__, __LINE__ ); \
	} \
	catch( ... ) { \
		return ErrorHandler::Instance()->ReportUnexpected( context, __FILE__, __LINE__ ); \
	}

//
// Error Reporting Macros
//
#define _REPORT_EXCEPTION( ex , context ) \
	ErrorHandler::Instance()->ReportException( ex , context, __FILE__, __LINE__ );

#define _REPORT_UNEXPECTED( context ) \
	ErrorHandler::Instance()->ReportUnexpected( context, __FILE__, __LINE__ );

// Debug Message
#define _REPORT_ERROR( exid, context, text ) \
	ErrorHandler::Instance()->ReportError( #exid, context, text, __FILE__, __LINE__ );

#define _REPORT_DEBUG( context, text ) \
	ErrorHandler::Instance()->ReportDebug( context, text, __FILE__, __LINE__ );

#define _REPORT_INFO( context, text ) \
	ErrorHandler::Instance()->ReportInfo( context, text, __FILE__, __LINE__ );

#endif

