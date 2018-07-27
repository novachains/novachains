/*    Copyright 2012 SequoiaDB Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/* This list file is automatically generated,you shoud NOT modify this file anyway! test comment*/
#ifndef utilTRACE_H__
#define utilTRACE_H__
// Component: util
#define SDB__UTILBSONHASHER_HASHOBJ                        0x2000000002cL
#define SDB__UTILBSONHASHER_HASHELE                        0x2000000002dL
#define SDB__UTILBSONHASHER_HASH                           0x2000000002eL
#define SDB__UTILCACHEPAGE__WRITE                          0x2000000002fL
#define SDB__UTILCACHEPAGE_READ                            0x20000000030L
#define SDB__UTILCACHEPAGE_COPY                            0x20000000031L
#define SDB__UTILCACHEMGR_ALLOC                            0x20000000032L
#define SDB__UTILCACHEMGR_ALLOCWHOLE                       0x20000000033L
#define SDB__UTILCACHEMGR_RELEASE                          0x20000000034L
#define SDB__UTILCACHEMGR_ALLOCBLOCK                       0x20000000035L
#define SDB__UTILCACHEMGR_REALLOCBLOCK                     0x20000000036L
#define SDB__UTILCACHEMGR_RELEASEBLOCK                     0x20000000037L
#define SDB__UTILCACHEMGR__ALLOCMEM                        0x20000000038L
#define SDB__UTILCACHEMGR_CANRECYCLE                       0x20000000039L
#define SDB__UTILCACHEMGR_RECYCLEBLOCKS                    0x2000000003aL
#define SDB__UTILCACHEMGR__RECYCLEBLK                      0x2000000003bL
#define SDB__UTILCACHEBLK_LOCK                             0x2000000003cL
#define SDB__UTILCACHEBLK_UNLOCK                           0x2000000003dL
#define SDB__UTILCACHECTX_WRITE                            0x2000000003eL
#define SDB__UTILCACHECTX_READ                             0x2000000003fL
#define SDB__UTILCACHECTX_SUBMIT                           0x20000000040L
#define SDB__UTILCACHECTX__LOADPAGE                        0x20000000041L
#define SDB__UTILCACHEMGR_WRITE                            0x20000000042L
#define SDB__UTILCACHEMGR_SYNC                             0x20000000043L
#define SDB__UTILCACHEUNIT_GETANDLOCK                      0x20000000044L
#define SDB__UTILCACHEUNIT__SYNCPAGE                       0x20000000045L
#define SDB__UTILCACHEUNIT_LOCKCLEANER                     0x20000000046L
#define SDB__UTILCACHEUNIT_UNLOCKCLEANER                   0x20000000047L
#define SDB__UTILCACHEUNIT_CANSYNC                         0x20000000048L
#define SDB__UTILCACHEUNIT_SYNCPAGES                       0x20000000049L
#define SDB__UTILCACHEUNIT__UNPINPAGES                     0x2000000004aL
#define SDB__UTILCACHEUNIT_DROPDIRTY                       0x2000000004bL
#define SDB__UTILCACHEUNIT__SYNCPAGES                      0x2000000004cL
#define SDB__UTILCACHEUNIT_CANRECYCLE                      0x2000000004dL
#define SDB__UTILCACHEUNIT_RECYCLEPAGES                    0x2000000004eL
#define SDB__UTILCACHEUNIT__ALLOCFROMBLK                   0x2000000004fL
#define SDB_WIN32READ                                      0x20000000050L
#define SDB_LNSETMULTILINE                                 0x20000000051L
#define SDB_ISUNSUPPORTTERM                                0x20000000052L
#define SDB_ENABLERAWMODE                                  0x20000000053L
#define SDB_DISABLERAWMODE                                 0x20000000054L
#define SDB_GETCURSORPOSITION                              0x20000000055L
#define SDB_GETCOLUMNS                                     0x20000000056L
#define SDB_LNCLEARSCREEN                                  0x20000000057L
#define SDB_LNBEEP                                         0x20000000058L
#define SDB_FREECOMPLETIONS                                0x20000000059L
#define SDB_COMPLETELINE                                   0x2000000005aL
#define SDB_LNHISTORYCLEAR                                 0x2000000005bL
#define SDB_LNSETCPLCALLBACK                               0x2000000005cL
#define SDB_LNADDCPL                                       0x2000000005dL
#define SDB_ABINIT                                         0x2000000005eL
#define SDB_ABAPPEND                                       0x2000000005fL
#define SDB_ABFREE                                         0x20000000060L
#define SDB_CALCHIGHLIGHTPOS                               0x20000000061L
#define SDB_SETDISPLAYATTR                                 0x20000000062L
#define SDB_REFRESHSINGLELINE                              0x20000000063L
#define SDB_REFRESHMULTILINE                               0x20000000064L
#define SDB_REFRESHLINE                                    0x20000000065L
#define SDB_LNEDITINSERT                                   0x20000000066L
#define SDB_LNEDITMOVELEFT                                 0x20000000067L
#define SDB_LNEDITMOVERIGHT                                0x20000000068L
#define SDB_LNEDITMOVEHOME                                 0x20000000069L
#define SDB_LNEDITMOVEEND                                  0x2000000006aL
#define SDB_LNEDITHISTORYNEXT                              0x2000000006bL
#define SDB_LNEDITDELETE                                   0x2000000006cL
#define SDB_LNEDITBACKSPACE                                0x2000000006dL
#define SDB_LNEDITDELPREVWORD                              0x2000000006eL
#define SDB_LNEDIT                                         0x2000000006fL
#define SDB_LNPRINTKEYCODES                                0x20000000070L
#define SDB_LNRAW                                          0x20000000071L
#define SDB_LN                                             0x20000000072L
#define SDB_FREEHISTORYINLINENOISE                         0x20000000073L
#define SDB_LNNOISEATEXT                                   0x20000000074L
#define SDB_LNHISTORYGET                                   0x20000000075L
#define SDB_LNHISTORYADD                                   0x20000000076L
#define SDB_LNHISTORYSETMAXLEN                             0x20000000077L
#define SDB_LNHISTORYSAVE                                  0x20000000078L
#define SDB_LNHISTORYLOAD                                  0x20000000079L
#define SDB_SETDISPLAYATTRIBUTE                            0x2000000007aL
#define SDB__LINENOISECMDBLD__RELSNODE                     0x2000000007bL
#define SDB__LINENOISECMDBLD__LOADCMD                      0x2000000007cL
#define SDB__LINENOISECMDBLD__ADDCMD                       0x2000000007dL
#define SDB__LINENOISECMDBLD__DELCMD                       0x2000000007eL
#define SDB__LINENOISECMDBLD__INSERT                       0x2000000007fL
#define SDB__LINENOISECMDBLD__GETCOMPLETIONS2              0x20000000080L
#define SDB__LINENOISECMDBLD__PREFIND                      0x20000000081L
#define SDB__LINENOISECMDBLD__GETCOMPLETIONS               0x20000000082L
#define SDB_LINECOMPLETE                                   0x20000000083L
#define SDB_CANCONTINUENXTLINE                             0x20000000084L
#define SDB_HISTORYCLEAR                                   0x20000000085L
#define SDB_GETNXTCMD                                      0x20000000086L
#define SDB_HISTORYINIT                                    0x20000000087L
#endif
