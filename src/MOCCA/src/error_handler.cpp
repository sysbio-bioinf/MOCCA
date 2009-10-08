#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <unistd.h>		// getpid

using namespace std;

#include "error_handler.h"

// global help functions

const char * timetostr( time_t t ) {

	static char str[32];

	strftime(str, sizeof(str), "%Y/%m/%d %H:%M:%S", localtime( &t ) );

	return str;
}

const char * timestamp() {

	return timetostr( time(NULL) );
}

//
// struct ErrorMessage
//

string ErrorMessage::asString() const {

	try {
		ostringstream s;

		s << timetostr( timestamp ) << " ";
		s << "[" << exclass << "] " << exid << " IN ";

		if( file.length() > 0 ) {
			s << file;
			if( line > 0 ) 
				s << ":" << line << " ";
		}
		
		s << context << " ";
		s << "\"" << text << "\"";

		return s.str();
	}
	catch( ... ) {
		return "!UNDEFINED (FATAL)!";
	}
}

//
// class BaseException
//

BaseException::BaseException( const string & exid, const string & context, const string & text, const string &  file, int line )
{

	try {
		message_.timestamp	= time( NULL );		// create a new timestamp
		message_.exid 		= exid;
		message_.context 	= context;
		message_.text 		= text;
		message_.description= ErrorHandler::Instance()->GetDescription( exid );
		message_.file 		= file;
		message_.line		= line;
		message_.id			= ErrorHandler::Instance()->CreateMessageID();
		message_.pid		= getpid();

		const char * str  = name();
		if( str != NULL )
			message_.exclass	= str;
		else
			message_.exclass	= "NULL";
	}
	catch( ... ) {
		// terrible things !
		message_.id			= 0;	// invalid exception
	}
}

//
// The observer pattern
//

template< class Message >
void Notifier<Message>::Attach( observer_type & ob )
{
	if( find(  _observers.begin(), _observers.end(), & ob ) == _observers.end() ) 
		_observers.push_back( & ob );
}

template< class Message >
void Notifier<Message>::Detach( observer_type & ob ) 
{
	typename Observers::iterator iter = find(  _observers.begin(), _observers.end(), & ob );

	if( iter != _observers.end() )
		_observers.erase( iter );
}

template< class Message >
void Notifier<Message>::Notify( const message_type & msg  )
{

	typename Observers::iterator		first = _observers.begin(),
							last  = _observers.end();
							
	while( first != last )
	{
		if( (*first) != NULL )
			(*first)->Notify( msg );
		
		++first;
	}
}


//
// class MessageQueue
//

MessageQueue::MessageQueue( unsigned int maxsize ) : maxsize_( maxsize ),mqueue_() {

}

void MessageQueue::Notify( const message_type & msg ) {

	Push( msg );
}

bool MessageQueue::Exists( MessageID id ) const {
	
	MsgQueue::const_reverse_iterator	first = mqueue_.rbegin(),
										last = mqueue_.rend();

	while( first != last ) {

		if( (*first).id == id )
			return true;

		++first;
	}
	return false;
}

const ErrorMessage & MessageQueue::Find( MessageID id ) const {

	MsgQueue::const_reverse_iterator	first = mqueue_.rbegin(),
										last = mqueue_.rend();

	while( first != last ) {

		if( (*first).id == id )
			return (*first);

		++first;
	}
	_EX_THROW( RuntimeError, EX_INVALID_MESSAGE_ID, "MessageQueue::Find()" );
}

void MessageQueue::Clear() {

	mqueue_.clear();
}

bool MessageQueue::Push( const ErrorMessage & msg ) {

	bool overflow = false;

	if( mqueue_.size() >= maxsize_ ) {
		// queue overflow -> remove least recent message
		mqueue_.pop_front();
		overflow = true;
	}

	mqueue_.push_back( msg );

	return ! overflow;
}

bool MessageQueue::Remove( MessageID id ) {

	MsgQueue::iterator	first = mqueue_.begin(),
						last = mqueue_.end();

	while( first != last ) {

		if( (*first).id == id ) {

			mqueue_.erase( first );
			return true;
		}

		++first;
	}
	return false;
}

//
// class FileLogger
//

FileLogger::FileLogger( const string & filename ) :
	active_(false), failure_(false), filename_()
{

	SetFileName( filename );
}

void FileLogger::Notify( const message_type & msg ) {

	if( ! active_ )
		return;

	try {

		ofstream of( filename_.c_str() ,ios::out | ios::app);

		if( ! of )
			return;


		of << "|" << msg.pid << "/" << msg.id << "| ";

		of << msg.asString() << endl;

		of.close();

		failure_ = false;
	}
	catch( ... ) {
		failure_ = true;
	}
}

void FileLogger::SetFileName( const string & filename ) {

	if( filename.size() > 0 ) {

		filename_	= filename;
		active_		= true;
		failure_	= false;

	} else {

		filename_ 	= "";
		active_ 	= false;
		failure_	= false;
	}
}

//
// class ErrorMapper
//

bool ErrorMapper::Exists( const string & exid ) const {

	return _map.find(exid) != _map.end();
}

const string & ErrorMapper::Lookup( const string & exid ) const {

	ErrorMap::const_iterator it = _map.find( string(exid) );
	static string invalid_ = "n/a";

	if( it != _map.end() )
		return (*it).second;
	else
		return invalid_;
}

bool ErrorMapper::LoadFromFile( const string & fname ) {

	_map.clear();

	ifstream			f( fname.c_str() );
	string				str, key, value;
	string::size_type	i, j;

	if( f.fail() )
		return false;

    try {

		while( ! f.eof() && f.good() ) {

            getline( f, str );
            if( ! f.fail() ) {

                // remove comments
                i = str.find(";");
                if( i != string::npos )
                	str.erase(i);

                // remove leading/trailing whites
                while( str.size() > 0 && isspace(str[0]) )
                    str.erase(0,1);

                while( str.size() > 0 && iscntrl(str[str.size()-1]) )
                    str.erase( str.size()-1 );

                if( str.size() > 0 ) {
                    // there seems to be a content
                    i = str.find_first_of(" \t");

                    key = str.substr(0,i);

                    i = str.find_first_not_of(" \t",i);
                    if( i != string::npos ) {

                    	if( str[i] == '"' ) ++i;

						if( i < str.size() ) {
                     		j = str.rfind("\"");
                      		if( j != string::npos && j > i )
                       			--j;
                       		else
                        		j = str.size();

	                    	value = str.substr(i, j-i+1);
						} else {
							value = "empty";
						}
                    } else {
                    	value = "undefined";
                    }

                    _map[ key ] = value;
                }
            }
        }
	}
    catch ( ... ) {

		// TODO : set an error flag
		return false;
    }

	f.close();

	return true;
}

//
// class BasicErrorHandler
//
MessageID BasicErrorHandler::ReportMessage( const ErrorMessage & msg ) {

	try {
		notifier_.Notify( msg );
	}
	catch( ... ) {

		return 0;	// return invalid message id
	}
	return msg.id;
}

MessageID BasicErrorHandler::ReportException( const BaseException & ex, const char * UPARAM( context ), const char * UPARAM( file ) , int UPARAM( line ) ) {

	return ReportMessage( ex.message() );
}

MessageID BasicErrorHandler::ReportException( const exception & ex, const char * context , const char *file , int line ) {

	return ReportMessage( StandardException(ex, context, file, line).message() );
}


MessageID BasicErrorHandler::ReportUnexpected( const char * context, const char *file, int line ) {

	return ReportMessage( UnexpectedException( context, file, line ).message() );
}

MessageID BasicErrorHandler::ReportError( const char * exid, const char * context, const char * text, const char *file, int line ) {

	ErrorMessage	m;

	m.timestamp	= time( NULL );
	m.exclass	= "ERROR";

	if( exid != NULL )		m.exid = exid;
	if( context != NULL )	m.context	= context;
	if( text != NULL )		m.text		= text;
	if( file != NULL )		m.file		= file;

	m.line		= line;
	m.id		= 0;

	return ReportMessage( m );
}

MessageID BasicErrorHandler::ReportInfo( const char * context, const char * text, const char *file, int line ){

	ErrorMessage	m;

	m.timestamp	= time( NULL );
	m.exclass	= "INFO";
	m.exid		= "INFO";

	if( context != NULL )	m.context	= context;
	if( text != NULL )		m.text		= text;
	if( file != NULL )		m.file		= file;

	m.line		= line;
	m.id		= 0;

	return ReportMessage( m );
}

MessageID BasicErrorHandler::ReportDebug( const char * context, const char * text, const char *file, int line ){

	ErrorMessage	m;

	m.timestamp	= time( NULL );
	m.exclass	= "DEBUG";
	m.exid		= "DEBUG";

	if( context != NULL )	m.context	= context;
	if( text != NULL )		m.text		= text;
	if( file != NULL )		m.file		= file;

	m.line		= line;
	m.id		= 0;

	return ReportMessage( m );
}

MessageID BasicErrorHandler::CreateMessageID() {

	++current_id_;
	if( current_id_ == 0 )
		++current_id_;
		
	return current_id_;
}
	
string BasicErrorHandler::GetDescription( const string & exid ) const {
	
	try {
		if( mapper_.Exists( exid ) )
			return mapper_.Lookup( exid );
		else
			return "n/a " + exid;
	}
	catch( ... ) {
		return "n/a (failed) " + exid;
	}
}

BasicErrorHandler::BasicErrorHandler() : current_id_(0) {

	notifier_.Attach( ob_queue_ );
	notifier_.Attach( ob_file_ );
}

BasicErrorHandler::~BasicErrorHandler() {

	notifier_.Detach( ob_queue_ );
	notifier_.Detach( ob_file_ );
}

// ErrorDetector
//
/*
ErrorDetector * ErrorDetector::instance_ = NULL;

ErrorDetector * ErrorDetector::Instance() {
	if( instance_ == NULL ) {
		instance_ = new BasicErrorHandler;

		if( instance_ == NULL )
			throw out_of_memory("ErrorDetector::Instance(): out of memory!!");
	}

	return instance_;
}

void ErrorDetector::Check( const char * descr, bool condition, const char * fname, int line ) {
	if( ! condition ) {
		throw 
	}
}

void ErrorDetector::CheckPrecondition( const char *, bool condition, const char * fname, int line ) {

}

void ErrorDetector::CheckPostcondition( const char *, bool condition, const char * fname, int line ) {
}

void ErrorDetector::CheckInvariant( const char *, bool condition, const char * fname, int line ) {
}

ErrorDetector::ErrorDetector() {
}

ErrorDetector::~ErrorDetector() {
}
*/
