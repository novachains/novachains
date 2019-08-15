/*******************************************************************************


   Copyright (C) 2011-2019 NOVACHAIN Ltd.

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

   Source File Name = dpsMergeBlock.cpp

   Descriptive Name = Data Protection Services Merge Block

   When/how to use: this program may be used on binary and text-formatted
   versions of DPS component. This file contains code logic for merging blocks

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/08/2019  YW  Initial Draft

   Last Changed =

*******************************************************************************/
#include "core.hpp"
#include "dpsMergeBlock.hpp"
#include "pd.hpp"
#include "ossUtil.hpp"
#include "dpsLogPage.hpp"
#include "dpsLogMgr.hpp"
#include "pdTrace.hpp"

namespace engine
{
   _dpsMergeBlock::_dpsMergeBlock():_isRow(FALSE)
   {
  //    clear() ;
   }

   _dpsMergeBlock::~_dpsMergeBlock()
   {
   }


   void _dpsMergeBlock::clear()
   {
      _isRow = FALSE;
      _record.clear() ;
      _pageMeta.clear() ;
   }

   _dpsMergeInfo::_dpsMergeInfo ()
   :_refer(_mergeBlock),
    _hasDummy(FALSE)
   {
      _csLID   = ~0 ;
      _clLID   = ~0 ;
      _extLID  = DMS_INVALID_EXTENT ;
      _needNty = FALSE ;
      _transEnabled = FALSE ;
   }

   _dpsMergeInfo::_dpsMergeInfo( dpsMergeBlock &block )
   :_refer(block),
    _hasDummy(FALSE)
   {
      _csLID   = ~0 ;
      _clLID   = ~0 ;
      _extLID  = DMS_INVALID_EXTENT ;
      _needNty = FALSE ;
      _transEnabled = FALSE ;
   }

   _dpsMergeInfo::~_dpsMergeInfo ()
   {
   }

   dpsMergeBlock &_dpsMergeInfo::getDummyBlock ()
   {
      return _dummyBlock ;
   }

   dpsMergeBlock &_dpsMergeInfo::getMergeBlock ()
   {
      return _refer ;
   }

   BOOLEAN _dpsMergeInfo::hasDummy ()
   {
      return _dummyBlock.pageMeta().valid() ? TRUE : FALSE ;
   }

}

