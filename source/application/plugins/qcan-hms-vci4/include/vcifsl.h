/*****************************************************************************
 HMS Technology Center Ravensburg GmbH
******************************************************************************

 Project : VCIFSL.DLL
 File    : VCIFSL.H
 Summary : VCI Frame & Signal Library.

 Date    : 2011-12-06

 Compiler: Microsoft Visual C

******************************************************************************
 all rights reserved
*****************************************************************************/

#ifndef _VCIFSL_H_
#define _VCIFSL_H_

#include <vcisdk.h>
#include <pshpack4.h>

/*##########################################################################*/
/*##                                                                      ##*/
/*##   CAN message switch                                                 ##*/
/*##                                                                      ##*/
/*##########################################################################*/

/*****************************************************************************
 * CAN message switch status
 ****************************************************************************/

typedef struct _CANMSGSWITCHSTATUS
{
  CANCHANSTATUS2 sChanStatus; // status of the used CAN channel
  UINT32         dwRxClients; // Number of attached RX clients
  UINT32         dwTxClients; // Number of attached TX clients
  UINT8          bWorkload;   // workload of scheduler thread in percent
} CANMSGSWITCHSTATUS, *PCANMSGSWITCHSTATUS;


/*****************************************************************************
 * Interface:
 *  ICanMsgSwitch
 *
 * Description:
 * Interface of CAN message switches.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ICanMsgSwitch
DECLARE_INTERFACE_(ICanMsgSwitch, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;

  //~~~ ICanMsgSwitch methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Initialize   )(THIS_ IN UINT32 dwTiming,
                                 IN INT32  lPriority,
                                 IN BOOL   fExclusive,
                                 IN UINT16 wRxFifoSize,
                                 IN UINT16 wTxFifoSize)              PURE;
  STDMETHOD(Activate     )(THIS)                                     PURE;
  STDMETHOD(Deactivate   )(THIS)                                     PURE;
  STDMETHOD(ForceReceive )(THIS)                                     PURE;
  STDMETHOD(ForceTransmit)(THIS)                                     PURE;
  STDMETHOD(GetStatus    )(THIS_ OUT PCANMSGSWITCHSTATUS pStatus)    PURE;
  STDMETHOD(GetControl   )(THIS_ OUT ICanControl2** ppCanCtl)        PURE;
  STDMETHOD(CreateClient )(THIS_ IN REFIID riid, OUT PVOID* ppv )    PURE;
  STDMETHOD(AttachClient )(THIS_ IN IUnknown* pClient )              PURE;
  STDMETHOD(DetachClient )(THIS_ IN IUnknown* pClient )              PURE;
};
#undef INTERFACE

typedef ICanMsgSwitch* PCANMSGSWITCH;

/*****************************************************************************
 * Interface ID of ICanMsgSwitch {E937DD49-0694-4418-96D1-0CAD103EB7B3}
 ****************************************************************************/
DEFINE_GUID(IID_ICanMsgSwitch,
       0xE937DD49,0x0694,0x4418,0x96,0xD1,0x0C,0xAD,0x10,0x3E,0xB7,0xB3);



/*##########################################################################*/
/*##                                                                      ##*/
/*##   CAN message specific client declarations                           ##*/
/*##                                                                      ##*/
/*##########################################################################*/

/*****************************************************************************
 * frame transmission modes
 ****************************************************************************/

#define CAN_TX_DIRECT   1 // transmit directly
#define CAN_TX_CYCLIC   2 // transmit in a cyclic (periodic) manner
#define CAN_TX_DELAYED  3 // transmit after a specified delay time


/*****************************************************************************
 * Interface:
 *  ICanRMsgBuffer
 *
 * Description:
 *  CAN message receive buffer interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ICanRMsgBuffer
DECLARE_INTERFACE_(ICanRMsgBuffer, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ICanRxBuffer methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Enable )(THIS_ IN UINT32 dwCanId)                        PURE;
  STDMETHOD(Disable)(THIS)                                           PURE;
  STDMETHOD(Read   )(THIS_ OUT PCANMSG2 pCanMsg,
                           OUT PUINT32  pdwRxCnt)                    PURE;
};
#undef INTERFACE

typedef ICanRMsgBuffer* PCANRMSGBUFFER;

/*****************************************************************************
 * Interface ID of ICanRMsgBuffer {C4693931-C415-45F2-BA83-E291C89AC9E5}
 ****************************************************************************/
DEFINE_GUID(IID_ICanRMsgBuffer,
       0xC4693931,0xC415,0x45F2,0xBA,0x83,0xE2,0x91,0xC8,0x9A,0xC9,0xE5);


/*****************************************************************************
 * Interface:
 *  ICanRMsgQueue
 *
 * Description:
 *  CAN receive queue interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ICanRMsgQueue
DECLARE_INTERFACE_(ICanRMsgQueue, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ICanRMsgQueue methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Enable )(THIS_ IN UINT32 dwCanId,
                           IN UINT16 wMsgDepth)                      PURE;
  STDMETHOD(Disable)(THIS)                                           PURE;
  STDMETHOD(Read   )(THIS_ OUT CANMSG2 aCanMsg[],
                           IN  UINT32  dwCount,
                           OUT PUINT32 pdwDone)                      PURE;
};
#undef INTERFACE

typedef ICanRMsgQueue* PCANRMSGQUEUE;

/*****************************************************************************
 * Interface ID of ICanRMsgQueue {D62C4F39-435F-4956-82BE-48051A995419}
 ****************************************************************************/
DEFINE_GUID(IID_ICanRMsgQueue,
       0xD62C4F39,0x435F,0x4956,0x82,0xBE,0x48,0x05,0x1A,0x99,0x54,0x19);


/*****************************************************************************
 * Interface:
 *  ICanRMsgSet
 *
 * Description:
 *  CAN receive message set interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ICanRMsgSet
DECLARE_INTERFACE_(ICanRMsgSet, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ICanRMsgSet methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Enable )(THIS_ IN UINT32 adwCanId[],
                           IN UINT16 awDepth[],
                           IN UINT32 dwCount)                        PURE;
  STDMETHOD(Disable)(THIS)                                           PURE;
  STDMETHOD(Read   )(THIS_ OUT CANMSG2 aCanMsg[],
                           OUT UINT32  adwRxCnt[],
                           IN  UINT32  dwFirst,
                           IN  UINT32  dwCount)                      PURE;
};
#undef INTERFACE

typedef ICanRMsgSet* PCANRMSGSET;

/*****************************************************************************
 * Interface ID of ICanRMsgSet {8B9C8BB9-4E63-4DA2-89FC-270D382B93E9}
 ****************************************************************************/
DEFINE_GUID(IID_ICanRMsgSet,
       0x8B9C8BB9,0x4E63,0x4DA2,0x89,0xFC,0x27,0x0D,0x38,0x2B,0x93,0xE9);


/*****************************************************************************
 * Interface:
 *  ICanTMsgBuffer
 *
 * Description:
 *  CAN message transmit buffer interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ICanTMsgBuffer
DECLARE_INTERFACE_(ICanTMsgBuffer, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ICanTMsgBuffer methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Enable  )(THIS_ IN UINT32 dwCanId,
                            IN UINT32 dwMode,
                            IN UINT32 dwTime)                        PURE;
  STDMETHOD(Disable )(THIS)                                          PURE;
  STDMETHOD(Write   )(THIS_ IN PCANMSG2 pCanMsg)                     PURE;
};
#undef INTERFACE

typedef ICanTMsgBuffer* PCANTMSGBUFFER;

/*****************************************************************************
 * Interface ID of ICanTMsgBuffer {35BF7752-89FE-4949-82FA-ECDF01FB197C}
 ****************************************************************************/
DEFINE_GUID(IID_ICanTMsgBuffer,
       0x35BF7752,0x89FE,0x4949,0x82,0xFA,0xEC,0xDF,0x01,0xFB,0x19,0x7C);


/*****************************************************************************
 * Interface:
 *  ICanTMsgQueue
 *
 * Description:
 *  CAN transmit queue interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ICanTMsgQueue
DECLARE_INTERFACE_(ICanTMsgQueue, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ICanTMsgQueue methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Enable  )(THIS_ IN UINT32 dwCanId,
                            IN UINT16 wDepth,
                            IN UINT32 dwMode,
                            IN UINT32 dwTime )                       PURE;
  STDMETHOD(Disable )(THIS)                                          PURE;
  STDMETHOD(Write   )(THIS_ IN  CANMSG2 aCanMsg[],
                            IN  UINT32  dwCount,
                            OUT PUINT32 pdwDone)                     PURE;
};
#undef INTERFACE

typedef ICanTMsgQueue* PCANTMSGQUEUE;

/*****************************************************************************
 * Interface ID of ICanTMsgQueue {79F5FE9C-155F-40E5-998E-F8EA24619A76}
 ****************************************************************************/
DEFINE_GUID(IID_ICanTMsgQueue,
       0x79F5FE9C,0x155F,0x40E5,0x99,0x8E,0xF8,0xEA,0x24,0x61,0x9A,0x76);


/*****************************************************************************
 * Interface:
 *  ICanTMsgSet
 *
 * Description:
 *  CAN transmit message set interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ICanTMsgSet
DECLARE_INTERFACE_(ICanTMsgSet, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ICanTMsgSet methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Enable )(THIS_ IN UINT32 adwCanId[],
                           IN UINT16 awDepth[],
                           IN UINT32 adwMode[],
                           IN UINT32 adwTime[],
                           IN UINT32 dwCount)                        PURE;
  STDMETHOD(Disable)(THIS)                                           PURE;
  STDMETHOD(Write  )(THIS_ IN  CANMSG2 aCanMsg[],
                           IN  BOOL8   afValid[],
                           OUT BOOL8   afDone[],
                           IN  UINT32  dwFirst,
                           IN  UINT32  dwCount)                      PURE;
};
#undef INTERFACE

typedef ICanTMsgSet* PCANTMSGSET;

/*****************************************************************************
 * Interface ID of ICanTMsgSet {93F6312B-6FB6-4AEA-A928-5BAF92EA77BC}
 ****************************************************************************/
DEFINE_GUID(IID_ICanTMsgSet,
       0x93F6312B,0x6FB6,0x4AEA,0xA9,0x28,0x5B,0xAF,0x92,0xEA,0x77,0xBC);



/*##########################################################################*/
/*##                                                                      ##*/
/*##   Signal specific client declarations                                ##*/
/*##                                                                      ##*/
/*##########################################################################*/

/*****************************************************************************
 * Signal API specific variant data type FSLVAR
 *
 * Please note:
 * The memory footprint of FSLVAR is always 16 bytes. Therefore FSLVAR is not
 * binary compatible to the VARIANT data type under 64 bit Windows, which has
 * a memory footprint of 24 bytes!
 ****************************************************************************/

#ifdef _MSC_VER
#pragma warning(disable:4201) // nonstandard extension used...
#endif

#ifdef _USE_VARIANT_NAMEING
typedef struct _FSLVAR
{
  VARTYPE        vt;         // type of variant (one of VARTYPE)
  WORD           wReserved1;
  WORD           wReserved2;
  WORD           wReserved3;
  union
  {
    LONGLONG     llVal;      // VT_I8
    LONG         lVal;       // VT_I4
    BYTE         bVal;       // VT_UI1
    SHORT        iVal;       // VT_I2
    FLOAT        fltVal;     // VT_R4
    DOUBLE       dblVal;     // VT_R8
    VARIANT_BOOL boolVal;    // VT_BOOL
    BSTR         bstrVal;    // VT_BSTR
    CHAR         cVal;       // VT_I1
    USHORT       uiVal;      // VT_UI2
    ULONG        ulVal;      // VT_UI4
    ULONGLONG    ullVal;     // VT_UI8
    INT          intVal;     // VT_INT
    UINT         uintVal;    // VT_UINT
  };
} FSLVAR, *PFSLVAR;
#else

enum FSLVARENUM
{
  FSL_VT_EMPTY   = VT_EMPTY,
  FSL_VT_BOOL    = VT_BOOL,
  FSL_VT_INT8    = VT_I1,
  FSL_VT_UINT8   = VT_UI1,
  FSL_VT_INT16   = VT_I2,
  FSL_VT_UINT16  = VT_UI2,
  FSL_VT_INT32   = VT_I4,
  FSL_VT_UINT32  = VT_UI4,
  FSL_VT_INT64   = VT_I8,
  FSL_VT_UINT64  = VT_UI8,
  FSL_VT_INT     = VT_INT,
  FSL_VT_UINT    = VT_UINT,
  FSL_VT_SINGLE  = VT_R4,
  FSL_VT_DOUBLE  = VT_R8,
  FSL_VT_BSTR    = VT_BSTR
};

typedef VARIANT_BOOL FSL_BOOL;
typedef CHAR         FSL_INT8;
typedef BYTE         FSL_UINT8;
typedef SHORT        FSL_INT16;
typedef USHORT       FSL_UINT16;
typedef LONG         FSL_INT32;
typedef ULONG        FSL_UINT32;
typedef LONGLONG     FSL_INT64;
typedef ULONGLONG    FSL_UINT64;
typedef INT          FSL_INT;
typedef UINT         FSL_UINT;
typedef FLOAT        FSL_SINGLE;
typedef DOUBLE       FSL_DOUBLE;
typedef OLECHAR*     FSL_BSTR;

typedef struct _FSLVAR
{
  UINT16       wVarType;     // type of variant (one of FSLVARENUM)
  UINT16       _rsvd1_;      // reserved (not used)
  UINT16       _rsvd2_;      // reserved (not used)
  UINT16       _rsvd3_;      // reserved (not used)
  union
  {
    FSL_BOOL   asBool;       // FSL_VT_BOOL
    FSL_INT8   asInt8;       // FSL_VT_INT8
    FSL_UINT8  asUInt8;      // FSL_VT_UINT8
    FSL_INT16  asInt16;      // FSL_VT_INT16
    FSL_UINT16 asUInt16;     // FSL_VT_UINT16
    FSL_INT32  asInt32;      // FSL_VT_INT32
    FSL_UINT32 asUInt32;     // FSL_VT_UINT32
    FSL_INT64  asInt64;      // FSL_VT_INT64
    FSL_UINT64 asUInt64;     // FSL_VT_UINT64
    FSL_INT32  asInt;        // FSL_VT_INT
    FSL_UINT32 asUInt;       // FSL_VT_UINT
    FSL_SINGLE asSingle;     // FSL_VT_SINGLE
    FSL_DOUBLE asDouble;     // FSL_VT_DOUBLE
    FSL_BSTR   asBStr;       // FSL_VT_BSTR
  };
} FSLVAR, *PFSLVAR;
#endif //_USE_VARIANT_NAMEING

#ifdef _MSC_VER
#pragma warning(default:4201) // nonstandard extension used...
#endif


/*****************************************************************************
 * signal type
 ****************************************************************************/

typedef struct _FSLSIGNAL
{
  HANDLE hSigId; // [in]  signal ID
  UINT64 qwTime; // [out] receive time
  UINT32 dwStat; // [out] signal status flags
  FSLVAR sValue; // [i/o] signal value
} FSLSIGNAL, *PFSLSIGNAL;


/*****************************************************************************
 * signal status flags
 ****************************************************************************/

#define FSL_SIG_STAT_CFAIL   0x0000001 // convertion failed
#define FSL_SIG_STAT_RXOVR   0x8000000 // receive data overrun


/*****************************************************************************
 * signal attributes
 ****************************************************************************/

#define FSL_SIG_ATTR_NAME    1 // signal name
                               // (data size: (strlen + 1) * sizeof(TCHAR)
#define FSL_SIG_ATTR_UNIT    2 // signal value unit
                               // (data size: (strlen + 1) * sizeof(TCHAR)
#define FSL_SIG_ATTR_DLID    3 // default language ID
                               // (data size: sizeof(LANGID))
#define FSL_SIG_ATTR_PLID    4 // preferred language ID
                               // (data size: sizeof(LANGID))


/*****************************************************************************
 * convertion modes
 ****************************************************************************/

#define FSL_SIG_CONV_RAWTOPHY 1  // raw value to physical value
#define FSL_SIG_CONV_PHYTORAW 2  // physical value to raw value


/*****************************************************************************
 * Interface:
 *  ISignalSet
 *
 * Description:
 *  Signal set interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ISignalSet
DECLARE_INTERFACE_(ISignalSet, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ISignalSet methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(LoadDBA   )(THIS_ IN  PCHAR  pszFile,
                              IN  PCHAR  pszPara,
                              IN  PCHAR  pszSigs,
                              IN  UINT16 awDepth[],
                              OUT HANDLE ahSigId[],
                              IN  UINT32 dwCount)                    PURE;
  STDMETHOD(LoadDBW   )(THIS_ IN  PWCHAR  pszFile,
                              IN  PWCHAR  pszPara,
                              IN  PWCHAR  pszSigs,
                              IN  UINT16  awDepth[],
                              OUT HANDLE  ahSigId[],
                              IN  UINT32  dwCount)                   PURE;
  STDMETHOD(CloseDB   )(THIS)                                        PURE;
  STDMETHOD(GetAttrA  )(THIS_ IN  HANDLE  hSigId,
                              IN  UINT32  dwAttr,
                              OUT PVOID   pvData,
                              IN  UINT32  dwSize,
                              OUT PUINT32 pdwOut)                    PURE;
  STDMETHOD(GetAttrW  )(THIS_ IN  HANDLE  hSigId,
                              IN  UINT32  dwAttr,
                              OUT PVOID   pvData,
                              IN  UINT32  dwSize,
                              OUT PUINT32 pdwOut)                    PURE;
  STDMETHOD(SetAttrA  )(THIS_ IN HANDLE hSigId,
                              IN UINT32 dwAttr,
                              IN PVOID  pvData,
                              IN UINT32 dwSize)                      PURE;
  STDMETHOD(SetAttrW  )(THIS_ IN HANDLE hSigId,
                              IN UINT32 dwAttr,
                              IN PVOID  pvData,
                              IN UINT32 dwSize)                      PURE;
  STDMETHOD(Convert   )(THIS_ IN  UINT32    dwMode,
                              IN  FSLSIGNAL aInSig[],
                              OUT FSLSIGNAL aOutSig[],
                              IN  UINT32    dwCount)                 PURE;
  STDMETHOD(Enable    )(THIS)                                        PURE;
  STDMETHOD(Disable   )(THIS)                                        PURE;
};
#undef INTERFACE

typedef ISignalSet* PSIGNALSET;

#ifdef UNICODE
#define LoadDB     LoadDBW
#define SaveDB     SaveDBW
#define AddSignal  AddSignalW
#define GetSignal  GetSignalW
#define SetSignal  SetSignalW
#define GetAttr    GetAttrW
#define SetAttr    SetAttrW
#else
#define LoadDB     LoadDBA
#define SaveDB     SaveDBA
#define AddSignal  AddSignalA
#define GetSignal  GetSignalA
#define SetSignal  SetSignalA
#define GetAttr    GetAttrA
#define SetAttr    SetAttrA
#endif

/*****************************************************************************
 * Interface ID of ISignalSet {D84BF001-C6F0-4B5D-A54E-A4C98D9AABD9}
 ****************************************************************************/
DEFINE_GUID(IID_ISignalSet,
       0xD84BF001,0xC6F0,0x4B5D,0xA5,0x4E,0xA4,0xC9,0x8D,0x9A,0xAB,0xD9);


/*****************************************************************************
 * Interface:
 *  IRSignalSet
 *
 * Description:
 *  Receive signal set interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE IRSignalSet
DECLARE_INTERFACE_(IRSignalSet, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ISignalSet methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(LoadDBA )(THIS_ IN  PCHAR  pszFile,
                            IN  PCHAR  pszPara,
                            IN  PCHAR  pszSigs,
                            IN  UINT16 awDepth[],
                            OUT HANDLE ahSigId[],
                            IN  UINT32 dwCount)                      PURE;
  STDMETHOD(LoadDBW )(THIS_ IN  PWCHAR  pszFile,
                            IN  PWCHAR  pszPara,
                            IN  PWCHAR  pszSigs,
                            IN  UINT16  awDepth[],
                            OUT HANDLE  ahSigId[],
                            IN  UINT32  dwCount)                     PURE;
  STDMETHOD(CloseDB )(THIS)                                          PURE;
  STDMETHOD(GetAttrA)(THIS_ IN  HANDLE  hSigId,
                            IN  UINT32  dwAttr,
                            OUT PVOID   pvData,
                            IN  UINT32  dwSize,
                            OUT PUINT32 pdwOut)                      PURE;
  STDMETHOD(GetAttrW)(THIS_ IN  HANDLE  hSigId,
                            IN  UINT32  dwAttr,
                            OUT PVOID   pvData,
                            IN  UINT32  dwSize,
                            OUT PUINT32 pdwOut)                      PURE;
  STDMETHOD(SetAttrA)(THIS_ IN HANDLE hSigId,
                            IN UINT32 dwAttr,
                            IN PVOID  pvData,
                            IN UINT32 dwSize)                        PURE;
  STDMETHOD(SetAttrW)(THIS_ IN HANDLE hSigId,
                            IN UINT32 dwAttr,
                            IN PVOID  pvData,
                            IN UINT32 dwSize)                        PURE;
  STDMETHOD(Convert )(THIS_ IN  UINT32    dwMode,
                            IN  FSLSIGNAL aInSig[],
                            OUT FSLSIGNAL aOutSig[],
                            IN  UINT32    dwCount)                   PURE;
  STDMETHOD(Enable  )(THIS)                                          PURE;
  STDMETHOD(Disable )(THIS)                                          PURE;
  //~~~ IRSignalSet methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Read    )(THIS_ IN  BOOL      fConvert,
                            OUT FSLSIGNAL aSignal[],
                            OUT UINT32    adwRxCnt[],
                            IN  UINT32    dwCount )                  PURE;
};
#undef INTERFACE

typedef IRSignalSet* PRSIGNALSET;

/*****************************************************************************
 * Interface ID of IRSignalSet {C6FDB39F-4380-48C2-9886-9926BC7757DB}
 ****************************************************************************/
DEFINE_GUID(IID_IRSignalSet,
       0xC6FDB39F,0x4380,0x48C2,0x98,0x86,0x99,0x26,0xBC,0x77,0x57,0xDB);


/*****************************************************************************
 * Interface:
 *  ITSignalSet
 *
 * Description:
 *  Transmit signal set interface.
 ****************************************************************************/
#undef  INTERFACE
#define INTERFACE ITSignalSet
DECLARE_INTERFACE_(ITSignalSet, IUnknown)
{
  //~~~ IUnknown methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(QueryInterface)(THIS_ IN REFIID riid, OUT PVOID *ppv)    PURE;
  STDMETHOD_(ULONG,AddRef )(THIS)                                    PURE;
  STDMETHOD_(ULONG,Release)(THIS)                                    PURE;
  //~~~ ISignalSet methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(LoadDBA )(THIS_ IN  PCHAR  pszFile,
                            IN  PCHAR  pszPara,
                            IN  PCHAR  pszSigs,
                            IN  UINT16 awDepth[],
                            OUT HANDLE ahSigId[],
                            IN  UINT32 dwCount)                      PURE;
  STDMETHOD(LoadDBW )(THIS_ IN  PWCHAR  pszFile,
                            IN  PWCHAR  pszPara,
                            IN  PWCHAR  pszSigs,
                            IN  UINT16  awDepth[],
                            OUT HANDLE  ahSigId[],
                            IN  UINT32  dwCount)                     PURE;
  STDMETHOD(CloseDB )(THIS)                                          PURE;
  STDMETHOD(GetAttrA)(THIS_ IN  HANDLE  hSigId,
                            IN  UINT32  dwAttr,
                            OUT PVOID   pvData,
                            IN  UINT32  dwSize,
                            OUT PUINT32 pdwOut)                      PURE;
  STDMETHOD(GetAttrW)(THIS_ IN  HANDLE  hSigId,
                            IN  UINT32  dwAttr,
                            OUT PVOID   pvData,
                            IN  UINT32  dwSize,
                            OUT PUINT32 pdwOut)                      PURE;
  STDMETHOD(SetAttrA)(THIS_ IN HANDLE hSigId,
                            IN UINT32 dwAttr,
                            IN PVOID  pvData,
                            IN UINT32 dwSize)                        PURE;
  STDMETHOD(SetAttrW)(THIS_ IN HANDLE hSigId,
                            IN UINT32 dwAttr,
                            IN PVOID  pvData,
                            IN UINT32 dwSize)                        PURE;
  STDMETHOD(Convert )(THIS_ IN  UINT32    dwMode,
                            IN  FSLSIGNAL aInSig[],
                            OUT FSLSIGNAL aOutSig[],
                            IN  UINT32    dwCount)                   PURE;
  STDMETHOD(Enable  )(THIS)                                          PURE;
  STDMETHOD(Disable )(THIS)                                          PURE;
  //~~~ ITSignalSet methods  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  STDMETHOD(Write   )(THIS_ IN  BOOL      fConvert,
                            IN  FSLSIGNAL aSignal[],
                            IN  BOOL8     afValid[],
                            OUT BOOL8     afDone[],
                            IN  UINT32    dwCount )                  PURE;
};
#undef INTERFACE

typedef ITSignalSet PTSIGNALSET;

/*****************************************************************************
 * Interface ID of ITSignalSet {743A09E8-B4EA-4A32-9C1A-0F1CC6E40AB0}
 ****************************************************************************/
DEFINE_GUID(IID_ITSignalSet,
       0x743A09E8,0xB4EA,0x4A32,0x9C,0x1A,0x0F,0x1C,0xC6,0xE4,0x0A,0xB0);



/*##########################################################################*/
/*##                                                                      ##*/
/*##   exported API functions                                             ##*/
/*##                                                                      ##*/
/*##########################################################################*/

EXTERN_C HRESULT VCIAPI
  VciCreateCanMsgSwitch( IN  IBalObject*     pBalObj,
                         IN  UINT32          dwBusNo,
                         OUT ICanMsgSwitch** ppSwitch );

typedef HRESULT (VCIAPI *PF_VciCreateCanMsgSwitch)(
  IN  IBalObject*     pBalObj,
  IN  UINT32          dwBusNo,
  OUT ICanMsgSwitch** ppSwitch );

#include <poppack.h>
#endif //_VCIFSL_H_
