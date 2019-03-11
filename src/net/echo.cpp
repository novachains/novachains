//JUST FOR TESTING NET SERVICE -- JIAQI


#include "echo.hpp"
#include "netService.hpp"
#include <iostream>
#include <string.h>

using namespace std;

namespace engine
{

   echoStream::echoStream( _MsgRouteID& id, const char* pBuf)
   {
      _id = id ;
      ossMemcpy(_data, pBuf, BUF_SIZE) ;
      _len = strlen(pBuf) ;
   }

   char* echoStream::getData() const
   {
      char* pChar = new char[BUF_SIZE] ;
      ossMemcpy(pChar, _data, BUF_SIZE) ;
      return pChar ;
   }

   _netMsgParser* echoParser::clone()
   {
      _netMsgParser* parser = SDB_OSS_NEW echoParser() ;
      return parser ;
   }

   BOOLEAN echoParser::push(const char* pBuf, const std::size_t& dataRecvd)
   {
      BOOLEAN EOM = FALSE ;
      cout<< " the data now is " << pBuf << endl ;
      SDB_ASSERT( dataRecvd < _msgLen , "received msg of unexpected length") ;
      char buf[4] = "end" ;
      if ( ! strcmp( &(pBuf[dataRecvd-3]), buf))
      {
         EOM = TRUE ;
         _dataRecvd = 0 ;
      }
      return EOM ;
   }

   char * echoStream::serialize() const 
   {

      char* pChar = (char *)SDB_OSS_MALLOC(BUF_SIZE) ;
      ossMemset(pChar, 0, BUF_SIZE) ;
      ossMemcpy(pChar, _data, strlen(_data)) ;
      return pChar ;
   }


   _netMsgStream* echoParser::get(const char* pBuf)
   {
      _MsgRouteID id ;
      id.columns.serviceID = MSG_ROUTE_TEST ;
      _netMsgStream * p = SDB_OSS_NEW echoStream( id, pBuf) ;
      return p ;
   }

   INT32 echoHandler::handleStream( const _MsgRouteID &id, MsgStream *pMsg )
   {
      if ( pMsg)
      {
         char * pBuf = pMsg->getData() ;
         ossMemcpy(msg, pBuf, BUF_SIZE) ;
         cout << " message in the handler is " << msg <<endl;
         SDB_OSS_DEL pMsg ;
//         delete []pBuf ;
         hasData = TRUE ;
      }
      return SDB_OK ;
   }

   void echoHandler::handleConnect( const NET_HANDLE &handle, _MsgRouteID id, BOOLEAN isPositive )
   {
      cout<< "establish a new connectioni at handle: "<< UINT64(handle) << endl;
   }

   void handleEcho()
   {
      INT32 rc = SDB_OK ;
      echoHandler *pHandler = SDB_OSS_NEW echoHandler() ;
      _netMsgParser *parser = SDB_OSS_NEW echoParser() ;
      _MsgRouteID id ;
      id.value = 0 ;
      id.columns.serviceID = MSG_ROUTE_REST_SERVICE ;
      MsgRouter *router = SDB_OSS_NEW MsgRouter(id) ;
      router->createRouterItem(id, "localhost", "8080") ;
      _MsgRouteID id2 ;
      id2.value = 0 ;
      id2.columns.serviceID = MSG_ROUTE_TEST ;
      rc = router->createRouterItem(id2, "localhost", "8090") ;
      
      _netServiceItem * pItem = SDB_OSS_NEW _netServiceItem(pHandler, *router, parser) ;
      rc = pItem->listen(id) ;
      if ( !rc )
      {cout<< "start listening" << endl;}

      while (true)
      {
         cout<< "wating for data" << endl;
         if (pHandler->hasData)
         {
             cout<< "start sending to id" << id2.value << endl;
             _netMsgStream *pMsg = SDB_OSS_NEW echoStream(id2, pHandler->msg) ;
             cout<< " create send msg succeed"<< endl;
             rc = pItem->send(*pMsg) ;
             if (rc) { cout<< "send fail and the return code is " << rc << endl; }
             SDB_OSS_DEL pMsg ;
             pHandler->hasData = FALSE ;
             ossMemset( pHandler->msg, 0, BUF_SIZE) ;
         }
         sleep(1) ;
      }
   }

   void handleDel( _IPmdCB * netCB)
   {
      IPmdRoot::IF_PUB_MODE pMode = netCB->publishMode() ;
      cout << " publish mode of the netCB is " << pMode << endl;
      vector<PMD_IF_TYPE> vecIF ;
      netCB->publishList (vecIF) ;
      cout << " the size of publish list is : " << vecIF.size() << " and the type is " << vecIF[0] << endl;
      _netSvcManager *netMgr = (_netSvcManager*)netCB->queryIF(vecIF[0]) ;
      cout << " got the manager start to create item " << endl ;

      INT32 rc = SDB_OK ;
      echoHandler *pHandler = SDB_OSS_NEW echoHandler() ;
      _netMsgParser *parser = SDB_OSS_NEW echoParser() ;
      _MsgRouteID id ;
      id.value = 0 ;
      id.columns.serviceID = MSG_ROUTE_REST_SERVICE ;
      MsgRouter *router = SDB_OSS_NEW MsgRouter(id) ;
      router->createRouterItem(id, "localhost", "8080") ;
      _MsgRouteID id2 ;
      id2.value = 0 ;
      id2.columns.serviceID = MSG_ROUTE_TEST ;
      rc = router->createRouterItem(id2, "localhost", "8090") ;

      _netServiceItem * pItem = netMgr->createSvcItem(pHandler, *router, parser) ;

      rc = pItem->listen(id) ;
      while (true)
      {
         cout<< "wating for data" << endl;
         if (pHandler->hasData)
         {
             cout<< "start sending to id" << id2.value << endl;
             _netMsgStream *pMsg = SDB_OSS_NEW echoStream(id2, pHandler->msg) ;
             cout<< " create send msg succeed"<< endl;
             rc = pItem->send(*pMsg) ;
             if (rc) { cout<< "send fail and the return code is " << rc << endl; }
             SDB_OSS_DEL pMsg ;
             pHandler->hasData = FALSE ;
             ossMemset( pHandler->msg, 0, BUF_SIZE) ;
             pItem->close(id2) ;
             break ;
         }
         sleep(1) ;
      }


      cout << " create complete and go to delete phase " << endl ;
      SDB_OSS_DEL parser ;
      SDB_OSS_DEL router ;

      netMgr->freeSvcItem ( &pItem ) ;

      netCB -> fini()  ;
   }

}
