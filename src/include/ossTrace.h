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
#ifndef ossTRACE_H__
#define ossTRACE_H__
// Component: oss
#define SDB_OSSRSTSYSSIG                                   0x40000008bL
#define SDB_OSSSIGHNDABT                                   0x40000008cL
#define SDB_OSSFUNCADDR2NM                                 0x40000008dL
#define SDB_OSSDUMPSYSTM                                   0x40000008eL
#define SDB_OSSDUMPDBINFO                                  0x40000008fL
#define SDB_OSSDUMPSYSINFO                                 0x400000090L
#define SDB_OSSMCHCODE                                     0x400000091L
#define SDB_OSSDUMPSIGINFO                                 0x400000092L
#define SDB_OSSWLKSTK                                      0x400000093L
#define SDB_OSSGETSYMBNFA                                  0x400000094L
#define SDB_OSSDUMPREGSINFO                                0x400000095L
#define SDB_OSSDUMPST                                      0x400000096L
#define SDB_OSSDUMPREGSINFO2                               0x400000097L
#define SDB_OSSDUMPST2                                     0x400000098L
#define SDB_OSSDUMPREGSINFO3                               0x400000099L
#define SDB_OSSDUMPST3                                     0x40000009aL
#define SDB_OSSSYMINIT                                     0x40000009bL
#define SDB_OSSWKSEX                                       0x40000009cL
#define SDB_OSSWS                                          0x40000009dL
#define SDB_OSSGETSYMBNFADDR                               0x40000009eL
#define SDB__OSSENUMNMPS                                   0x40000009fL
#define SDB__OSSENUMNMPS2                                  0x4000000a0L
#define SDB_OSSCRTNMP                                      0x4000000a1L
#define SDB_OSSOPENNMP                                     0x4000000a2L
#define SDB_OSSCONNNMP                                     0x4000000a3L
#define SDB_OSSRENMP                                       0x4000000a4L
#define SDB_OSSWTNMP                                       0x4000000a5L
#define SDB_OSSDISCONNNMP                                  0x4000000a6L
#define SDB_OSSCLSNMP                                      0x4000000a7L
#define SDB_OSSNMP2FD                                      0x4000000a8L
#define SDB_OSSCRTNP                                       0x4000000a9L
#define SDB_OSSOPENNP                                      0x4000000aaL
#define SDB_OSSRDNP                                        0x4000000abL
#define SDB__OSSWTNP                                       0x4000000acL
#define SDB_OSSDELNP                                       0x4000000adL
#define SDB_OSSNP2FD                                       0x4000000aeL
#define SDB_OSSCLNPBYNM                                    0x4000000afL
#define SDB_OSSNTHND                                       0x4000000b0L
#define SDB_OSSST                                          0x4000000b1L
#define SDB_OSSEDUCTHND                                    0x4000000b2L
#define SDB_OSSEDUEXCFLT                                   0x4000000b3L
#define SDB_OSSDMPSYSTM                                    0x4000000b4L
#define SDB_OSSDMPDBINFO                                   0x4000000b5L
#define SDB_OSSSTKTRA                                      0x4000000b6L
#define SDB_OSSREGSIGHND                                   0x4000000b7L
#define SDB__OSSMMF_OPEN                                   0x4000000b8L
#define SDB__OSSMMF_CLOSE                                  0x4000000b9L
#define SDB__OSSMMF_SIZE                                   0x4000000baL
#define SDB__OSSMMF_MAP                                    0x4000000bbL
#define SDB__OSSMMF_FLHALL                                 0x4000000bcL
#define SDB__OSSMMF_FLUSH                                  0x4000000bdL
#define SDB__OSSMMF_FLUSHBLOCK                             0x4000000beL
#define SDB__OSSMMF_UNLINK                                 0x4000000bfL
#define SDB_OSSTS2STR                                      0x4000000c0L
#define SDB_STR2OSSTS                                      0x4000000c1L
#define SDB_OSSGETCPUUSG                                   0x4000000c2L
#define SDB_OSSGETCPUUSG2                                  0x4000000c3L
#define SDB_OSSSRAND                                       0x4000000c4L
#define SDB_OSSRAND                                        0x4000000c5L
#define SDB_OSSHEXDL                                       0x4000000c6L
#define SDB_OSSHEXDUMPBUF                                  0x4000000c7L
#define SDB_OSSGETMEMINFO                                  0x4000000c8L
#define SDB_OSSGETDISKINFO                                 0x4000000c9L
#define SDB_OSSGETFILEDESP                                 0x4000000caL
#define SDB_OSSREADLINK                                    0x4000000cbL
#define SDB_OSSGETDISKIOSTAT                               0x4000000ccL
#define SDB_OSSGETCPUINFO                                  0x4000000cdL
#define SDB_OSSGETPROCMEMINFO                              0x4000000ceL
#define SDB_OSSMODULEHANDLE_INIT                           0x4000000cfL
#define SDB_OSSMODULEHANDLE_UNLOAD                         0x4000000d0L
#define SDB_OSSMODULEHANDLE_RESOLVEADDRESS                 0x4000000d1L
#define SDB__OSSRWM_LOCK_R                                 0x4000000d2L
#define SDB__OSSRWM_LOCK_W                                 0x4000000d3L
#define SDB__OSSRWM_RLS_R                                  0x4000000d4L
#define SDB__OSSRWM_RLS_W                                  0x4000000d5L
#define SDB_OSSOPEN                                        0x4000000d6L
#define SDB_OSSCLOSE                                       0x4000000d7L
#define SDB_OSSMKDIR                                       0x4000000d8L
#define SDB_OSSDELETE                                      0x4000000d9L
#define SDB_OSSFILECOPY                                    0x4000000daL
#define SDB_OSSACCESS                                      0x4000000dbL
#define SDB_OSSREAD                                        0x4000000dcL
#define SDB_OSSWRITE                                       0x4000000ddL
#define SDB_OSSSEEK                                        0x4000000deL
#define SDB_OSSSEEKANDREAD                                 0x4000000dfL
#define SDB_OSSSEEKANDWRITE                                0x4000000e0L
#define SDB_OSSFSYNC                                       0x4000000e1L
#define SDB_OSSGETPATHTYPE                                 0x4000000e2L
#define SDB_OSSGETFSBYNM                                   0x4000000e3L
#define SDB_OSSGETFILESIZE                                 0x4000000e4L
#define SDB_OSSTRUNCATEFILE                                0x4000000e5L
#define SDB_OSSEXTFILE                                     0x4000000e6L
#define SDB_OSSGETREALPATH                                 0x4000000e7L
#define SDB_OSSGETFSTYPE                                   0x4000000e8L
#define SDB_OSSRENMPATH                                    0x4000000e9L
#define SDB_OSSLOCKFILE                                    0x4000000eaL
#define SDB__OSSSK__OSSSK                                  0x4000000ebL
#define SDB__OSSSK__OSSSK2                                 0x4000000ecL
#define SDB__OSSSK__OSSSK3                                 0x4000000edL
#define SDB_OSSSK_INITTSK                                  0x4000000eeL
#define SDB_OSSSK_SETSKLI                                  0x4000000efL
#define SDB_OSSSK_KPAL                                     0x4000000f0L
#define SDB_OSSSK_BIND_LSTN                                0x4000000f1L
#define SDB_OSSSK_SEND                                     0x4000000f2L
#define SDB_OSSSK_ISCONN                                   0x4000000f3L
#define SDB_OSSSK_RECV                                     0x4000000f4L
#define SDB_OSSSK_CONNECT                                  0x4000000f5L
#define SDB_OSSSK_CLOSE                                    0x4000000f6L
#define SDB_OSSSK_DISNAG                                   0x4000000f7L
#define SDB_OSSSK_SECURE                                   0x4000000f8L
#define SDB_OSSSK_DOSSLHANDSHAKE                           0x4000000f9L
#define SDB_OSSSK__GETADDR                                 0x4000000faL
#define SDB_OSSSK_SETTMOUT                                 0x4000000fbL
#define SDB__OSSSK__COMPLETE                               0x4000000fcL
#define SDB__OSSMEMALLOC                                   0x4000000fdL
#define SDB__OSSMEMREALLOC                                 0x4000000feL
#define SDB__OSSMEMFREE                                    0x4000000ffL
#define SDB_OSSPFOP_OPEN                                   0x400000100L
#define SDB_OSSPFOP_READ                                   0x400000101L
#define SDB_OSSPFOP_WRITE                                  0x400000102L
#define SDB_OSSPFOP_FWRITE                                 0x400000103L
#define SDB_OSSPFOP_GETSIZE                                0x400000104L
#define SDB__OSSEVN_WAIT                                   0x400000105L
#define SDB__OSSEN_SIGNAL                                  0x400000106L
#define SDB__OSSEN_SIGALL                                  0x400000107L
#define SDB__OSSVN_RESET                                   0x400000108L
#define SDB_OSSISPROCRUNNING                               0x400000109L
#define SDB_OSSWAITCHLD                                    0x40000010aL
#define SDB_OSSCRTLST                                      0x40000010bL
#define SDB_OSSEXEC2                                       0x40000010cL
#define SDB_OSSEXEC                                        0x40000010dL
#define SDB_OSSENBNMCHGS                                   0x40000010eL
#define SDB_OSSRENMPROC                                    0x40000010fL
#define SDB_OSSVERIFYPID                                   0x400000110L
#define SDB_OSSRSVPATH                                     0x400000111L
#define SDB_OSSWTINT                                       0x400000112L
#define SDB_OSSSTARTSERVICE                                0x400000113L
#define SDB_OSS_WFSTRS                                     0x400000114L
#define SDB_OSS_STOPSERVICE                                0x400000115L
#define SDB_OSSCRTPADUPHND                                 0x400000116L
#define SDB_WIN_OSSEXEC                                    0x400000117L
#define SDB_OSSGETEWD                                      0x400000118L
#define SDB_OSSCMSTART_BLDARGS                             0x400000119L
#define SDB_OSS_STARTPROCESS                               0x40000011aL
#endif
