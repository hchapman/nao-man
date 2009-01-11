/**
 * \mainpage
 * \section Author
 * @author NorthernBites
 *
 * \section Copyright
 * Version : $Id$
 *
 * \section Description
 *
 * This file was generated by Aldebaran Robotics ModuleGenerator
 */

#ifndef _WIN32
#include <signal.h>
#endif
#ifdef NAOQI1
#include "altypes.h"
#include "alxplatform.h"
#include "manmodule.h"
#include "alptr.h"
#include "albroker.h"
#include "almodule.h"
#include "albrokermanager.h"
#include "alerror.h"

//NBites includes
#include "alproxy.h"

using namespace std;
using namespace AL;

//<EXE_INCLUDE> don't remove this comment
#include "Man.h" //EDIT -JS



//</EXE_INCLUDE> don't remove this comment

//<ODECLAREINSTANCE> don't remove this comment

//</ODECLAREINSTANCE> don't remove this comment
static ALPtr<ALProxy> man;

#ifndef MAN_IS_REMOTE

#ifdef _WIN32
#define ALCALL __declspec(dllexport)
#else
#define ALCALL
#endif

#ifdef __cplusplus
extern "C"
{
#endif

ALCALL int _createModule( ALPtr<ALBroker> pBroker )
{
  // init broker with the main broker inctance
  // from the parent executable
  ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
  ALBrokerManager::getInstance()->addBroker(pBroker);


  // create modules instance
//<OGETINSTANCE> don't remove this comment
ALModule::createModule<Man>(pBroker,"Man" );

//</OGETINSTANCE> don't remove this comment

//NBites code
man  = pBroker->getProxy("Man");
man->callVoid("start");

  return 0;
}

ALCALL int _closeModule(  )
{
  // Delete module instance
//<OKILLINSTANCE> don't remove this comment
//ALPtr<ALProxy>
// man  = pBroker->getProxy("Man");
man->callVoid("stop");
//</OKILLINSTANCE> don't remove this comment

  return 0;
}

# ifdef __cplusplus
}
# endif

#else
void _terminationHandler( int signum )
{
    //ALPtr<ALProxy> man  = pBroker->getProxy("Man");

  if (signum == SIGINT) {
    // no direct exit, main thread will exit when finished
    cerr << "Exiting Man via thread stop." << endl;
    man->callVoid("stop");
  }
  else {
    cerr << "Emergency stop -- exiting immediately" << endl;
    // fault, exit immediately
    ::exit(1);
  }
  ALBrokerManager::getInstance()->killAllBroker();
  ALBrokerManager::kill();
  exit(0);
}


int usage( char* progName )
{
  std::cout << progName <<", a remote module of naoqi !" << std::endl

            << "USAGE :" << std::endl
            << "-b\t<ip> : binding ip of the server. Default is 127.0.0.1" << std::endl
            << "-p\t<port> : binding port of the server. Default is 9559" << std::endl
            << "-pip\t<ip> : ip of the parent broker. Default is 127.0.0.1" << std::endl
            << "-pport\t<ip> : port of the parent broker. Default is 9559" << std::endl
            << "-h\t: Display this help\n" << std::endl;
  return 0;
}

int main( int argc, char *argv[] )
{
  std::cout << "..::: starting MANMODULE revision " << MANMODULE_VERSION_REVISION << " :::.." << std::endl;
  std::cout << "Copyright (c) 2007, Aldebaran-robotics" << std::endl << std::endl;

  int  i = 1;
  std::string brokerName = "manmodule";
  std::string brokerIP = "0.0.0.0";
  int brokerPort = 0 ;
  // Default parent broker IP
  std::string parentBrokerIP = "127.0.0.1";
  // Default parent broker port
  int parentBrokerPort = kBrokerPort;

  // checking options
  while( i < argc ) {
    if ( argv[i][0] != '-' ) return usage( argv[0] );
    else if ( std::string( argv[i] ) == "-b" )        brokerIP          = std::string( argv[++i] );
    else if ( std::string( argv[i] ) == "-p" )        brokerPort        = atoi( argv[++i] );
    else if ( std::string( argv[i] ) == "-pip" )      parentBrokerIP    = std::string( argv[++i] );
    else if ( std::string( argv[i] ) == "-pport" )    parentBrokerPort  = atoi( argv[++i] );
    else if ( std::string( argv[i] ) == "-h" )        return usage( argv[0] );
    i++;
  }

  // If server port is not set
  if ( !brokerPort )
    brokerPort = FindFreePort( brokerIP );

  std::cout << "Try to connect to parent Broker at ip :" << parentBrokerIP
            << " and port : " << parentBrokerPort << std::endl;
  std::cout << "Start the server bind on this ip :  " << brokerIP
            << " and port : " << brokerPort << std::endl;

  // Starting Broker
 ALPtr<ALBroker> pBroker = ALBroker::createBroker(brokerName, brokerIP, brokerPort, parentBrokerIP,  parentBrokerPort);
 pBroker->setBrokerManagerInstance(ALBrokerManager::getInstance());


//<OGETINSTANCE> don't remove this comment
ALModule::createModule<Man>(pBroker,"Man" );

//</OGETINSTANCE> don't remove this comment

#ifndef _WIN32
  struct sigaction new_action;
  /* Set up the structure to specify the new action. */
  new_action.sa_handler = _terminationHandler;
  sigemptyset( &new_action.sa_mask );
  new_action.sa_flags = 0;

  sigaction( SIGINT, &new_action, NULL );
#endif

  try{
  //ALPtr<ALProxy>
  man  = pBroker->getProxy("Man");
  // Start the separate head thread
  man->callVoid("start");
  //(*(ALFunctor0<ALModule, boost::shared_ptr<TriggeredEvent> *)(man->getModule()->getFunction("getTrigger"));
  man->callVoid("trigger_await_on");
  // Wait for the head thread to exit
  man->callVoid("trigger_await_off");
  cout << "Main method finished." << endl;
  }catch(ALError &e){
      cout <<e.toString() <<endl;
  }
//   Not sure what the purpose of this modulegenerator code is: //EDIT -JS
   pBroker.reset(); // because of while( 1 ), broker counted by brokermanager
   while( 1 )
   {
     SleepMs( 100 );
   }

#ifdef _WIN32
  _terminationHandler( 0 );
#endif

  exit( 0 );
}
#endif

#else //NAOQI1
#include "albroker.h"
#include "almodule.h"
#include "altypes.h"
#include "alxplatform.h"

#include <boost/shared_ptr.hpp>

#include "manmodule.h"
#include "Man.h"

////////////////////////////////////////////
// //
// Library or runtime entry definitions //
// //
////////////////////////////////////////////


#ifndef MAN_IS_REMOTE
// Non-remote module
// builds a shared library to be loaded at naoqi initialization

# ifdef _WIN32
# define ALCALL __declspec(dllexport)
# else
# define ALCALL
# endif

# ifdef __cplusplus
extern "C" {
# endif
// reference to the running instance
static boost::shared_ptr<Man> lMan;

ALCALL int
_createModule (ALBroker *pBroker)
{

  // init broker with the main broker instance
  // from the parent executable
  ALBroker::setInstance(pBroker);

  // create modules instance. This will register automatically to the broker
  lMan = boost::shared_ptr<Man>(new Man());
  // start Man in a new thread, so as to run the libraries main functions
  lMan->start();

  return 0;
}

ALCALL int
_closeModule ()
{
  // Delete module instance. Will unregister automatically.
  if (lMan != NULL)
    lMan->stop();

  return 0;
}

# ifdef __cplusplus
}
# endif



#else
// MAN_IS_REMOTE defined
// module is a remote module, so built as an executable binary


void
_terminationHandler (int signum)
{
  if (signum == SIGINT) {
    // no direct exit, main thread will exit when finished
    cerr << "Exiting Man via thread stop." << endl;
    lMan->stop();
  }
  else {
    cerr << "Emergency stop -- exiting immediately" << endl;
    // fault, exit immediately
    ::exit(1);
  }
}

int
usage (const char *name)
{
  cout << "USAGE: " << name << endl
       << "\t-h \t\t: Display this help" << endl
       << "\t-b <ip> \t: Binding address of the server. Default is 127.0.0.1" << endl
       << "\t-p <port> \t: Binding port of the server. Default is 9559" << endl
       << "\t-pip <ip> \t: Address of the parent broker. Default is 127.0.0.1" << endl
       << "\t-pport <ip> \t: Port of the parent broker. Default is 9559" << endl;

  return 0;
}

int
main (int argc, char **argv)
{
  int i = 1;
  std::string brokerName = "man";
  std::string brokerIP = "";
  int brokerPort = 0 ;
  // Default parent broker IP
  std::string parentBrokerIP = "127.0.0.1";
  // Default parent broker port
  int parentBrokerPort = kBrokerPort;

  // checking options
  while( i < argc ) {
    if ( argv[i][0] != '-' ) return usage( argv[0] );
    else if ( std::string( argv[i] ) == "-b" ) brokerIP = std::string( argv[++i] );
    else if ( std::string( argv[i] ) == "-p" ) brokerPort = atoi( argv[++i] );
    else if ( std::string( argv[i] ) == "-pip" ) parentBrokerIP = std::string( argv[++i] );
    else if ( std::string( argv[i] ) == "-pport" ) parentBrokerPort = atoi( argv[++i] );
    else if ( std::string( argv[i] ) == "-h" ) return usage( argv[0] );
    i++;
  }

  // If server port is not set
  if ( !brokerPort )
    brokerPort = FindFreePort( brokerIP );

  std::cout << "Try to connect to parent Broker at ip :" << parentBrokerIP
            << " and port : " << parentBrokerPort << std::endl;
  //std::cout << "Start the server bind on this ip : " << brokerIP
  // << " and port : " << brokerPort << std::endl;

  // Starting Broker
  AL::ALBroker* broker = AL::ALBroker::getInstance( );
  // init the broker with its ip and port, and the ip and port of a parent broker, if exist
  broker->init( brokerName, brokerIP, brokerPort, parentBrokerIP, parentBrokerPort );

# ifndef _WIN32
  struct sigaction new_action;
  // Set up the structure to specify the new action.
  new_action.sa_handler = _terminationHandler;
  sigemptyset( &new_action.sa_mask );
  new_action.sa_flags = 0;

  sigaction( SIGINT, &new_action, NULL );
#endif

  // Init Man. Module is automatically registered to the broker.
  lMan = boost::shared_ptr<Man>(new Man());
  // Start the separate head thread
  lMan->start();
  lMan->getTrigger()->await_on();
  // Wait for the head thread to exit
  lMan->getTrigger()->await_off();

  cout << "Main method finished." << endl;

  // successful exit
  return 0;
}

#endif // MAN_IS_REMOTE


#endif
