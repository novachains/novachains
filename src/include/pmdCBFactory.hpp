/*******************************************************************************


   Copyright (C) 2011-2018 SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Source File Name = pmdCBFactory.hpp

   Descriptive Name = Process MoDel Header

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains structure kernel control block,
   which is the most critical data structure in the engine process.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMD_CB_FACTORY_HPP__
#define PMD_CB_FACTORY_HPP__

#include "core.hpp"
#include "pmdInterface.hpp"
#include <map>

using namespace std ;

namespace engine
{

   typedef IPmdCB* (*PMD_CREATE_CB_FUNC)() ;
   typedef void (*PMD_RELEASE_CB_FUNC)( IPmdCB *cb ) ;

   typedef pair<PMD_CREATE_CB_FUNC, PMD_RELEASE_CB_FUNC>    PMD_FUNC_PAIR ;

   typedef map<PMD_CB_TYPE, PMD_FUNC_PAIR>         MAP_TYPE_2_FUNC ;
   typedef MAP_TYPE_2_FUNC::iterator               MAP_TYPE_2_FUNC_IT ;

   typedef map<string, PMD_FUNC_PAIR>              MAP_NAME_2_FUNC ;
   typedef MAP_NAME_2_FUNC::iterator               MAP_NAME_2_FUNC_IT ;

   /*
      AUTO CB REGISTER DEFINE
   */
   #define PMD_DECLARE_CB_AUTO_REGISTER() \
      public:\
         static IPmdCB* newThis() ;\
         static void    deleteThis( IPmdCB *CB )

   /*
      AUTO CB REGISTER IMPLEMENT
   */
   #define PMD_IMPLEMENT_CB_AUTO_REGISTER(theClass)\
      IPmdCB* theClass::newThis()\
      {\
         return SDB_OSS_NEW theClass() ;\
      }\
      void theClass::deleteThis( IPmdCB *cb )\
      {\
         SDB_OSS_DEL cb ;\
      }\
      pmdCBAssist theClass##Assist( theClass::newThis, theClass::deleteThis )

   /*
      _pmdCBAssist define
   */
   class _pmdCBAssist
   {
      public:
         _pmdCBAssist( PMD_CREATE_CB_FUNC pCreate,
                       PMD_RELEASE_CB_FUNC pRelease ) ;
         ~_pmdCBAssist() ;
   } ;
   typedef _pmdCBAssist pmdCBAssist ;

   /*
      _pmdCBFactory define
   */
   class _pmdCBFactory
   {
      friend class _pmdCBAssist ;
      public:
         _pmdCBFactory() ;
         ~_pmdCBFactory() ;

         BOOLEAN           hasCB( PMD_CB_TYPE type ) const ;
         BOOLEAN           hasCB( const string &name ) const ;
         IPmdCB*           createCB( PMD_CB_TYPE type ) ;
         IPmdCB*           createCB( const string &name ) ;
         void              releaseCB( IPmdCB *&cb ) ;

      protected:

         BOOLEAN           registerCB( PMD_CB_TYPE type,
                                       const string &name,
                                       PMD_CREATE_CB_FUNC pCreateFunc,
                                       PMD_RELEASE_CB_FUNC pReleaseFunc ) ;

      private:
         MAP_TYPE_2_FUNC            _mapCB2Func ;
         MAP_NAME_2_FUNC            _mapName2Func ;

   } ;
   typedef _pmdCBFactory pmdCBFactory ;

   /*
      Global functions
   */
   pmdCBFactory* pmdGetCBFactory() ;

}

#endif //PMD_CB_FACTORY_HPP__

