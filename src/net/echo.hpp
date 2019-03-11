//JUST FOR TESTING NET SERVICE -- JIAQI

#include "netMsgStream.hpp"
#include "netMsgHandler.hpp"
#include "netCB.hpp"

using namespace std ;

namespace engine
{

    #define BUF_SIZE 16    
    class echoStream : public _netMsgStream
    {

      friend class echoParser ;
      public:
         echoStream ( _MsgRouteID& id, const char* pBuf) ;
         virtual ~echoStream () {}

      public:
         virtual UINT32 getSeq() const {}
         virtual _MsgRouteID getSenderID() const { _MsgRouteID id ; id.value = 0; id.columns.serviceID = MSG_ROUTE_REST_SERVICE; return id ; }
         virtual _MsgRouteID getRouteID() const { return _id ;}
         virtual UINT32 getReceiver() const {}
         virtual UINT32 getLen() const {return _len ;}
         virtual char* serialize() const ;
         virtual char* getData() const ;

      private:
         _MsgRouteID _id ;
         UINT32 _len ;
         char _data[BUF_SIZE] ;
    } ;


    class echoParser : public _netMsgParser
    {
      public:
         echoParser () { _msgLen = BUF_SIZE+2; _dataRecvd = 0; ossMemset(data, 0, _msgLen+1); }
         virtual ~echoParser () {}

      public:
         virtual _netMsgParser* clone() ;
         virtual UINT32 getLen() { return _msgLen+1;}
         virtual BOOLEAN push(const char* pBuf, const std::size_t& dataRecvd) ;
         virtual _netMsgStream* get(const char* pBuf) ;

      private:
         UINT32 _msgLen ;
         UINT32 _dataRecvd ;
         char data[BUF_SIZE+2] ;
    } ;

    class echoHandler : public _netMsgHandler
    {
      public:
         echoHandler() { ossMemset(msg, 0, BUF_SIZE); hasData = FALSE; }
         virtual ~echoHandler() {}

      public:
         virtual INT32 handleStream( const _MsgRouteID &id, MsgStream *pMsg ) ;
         virtual void handleConnect( const NET_HANDLE &handle, _MsgRouteID id, BOOLEAN isPositive ) ;
         char msg[BUF_SIZE] ;
         BOOLEAN hasData ;
    } ;

    void handleEcho() ;

    void handleDel( _IPmdCB * netCB) ;
}
