/*
 *        Function Prototypes and Useful Defined Constants for the           
 *               Microsoft C Language Interface for the                      
 *                  GPIB DLL (Dynamic Link Library)                          
 *                     for Microsoft Windows 3.0/3.1
 *
 */
 
#ifdef __cplusplus
extern "C" {
#endif

extern unsigned short ibsta, iberr, ibcnt;
extern unsigned long ibcntl;

/**************************************************************************/
/*  NI-488 Function Prototypes  */

short _far _pascal ibclr (short ud);
short _far _pascal ibloc (short ud);
short _far _pascal ibpct (short ud);
short _far _pascal ibsic (short ud);
short _far _pascal ibstop (short ud);
short _far _pascal ibtrg (short ud);
short _far _pascal ibcac (short ud, short v);
short _far _pascal ibdma (short ud, short v);
short _far _pascal ibeos (short ud, short v);
short _far _pascal ibeot (short ud, short v);
short _far _pascal ibgts (short ud, short v);
short _far _pascal ibist (short ud, short v);
short _far _pascal ibonl (short ud, short v);
short _far _pascal ibpad (short ud, short v);
short _far _pascal ibppc (short ud, short v);
short _far _pascal ibrsc (short ud, short v);
short _far _pascal ibrsv (short ud, short v);
short _far _pascal ibsad (short ud, short v);
short _far _pascal ibsre (short ud, short v);
short _far _pascal ibtmo (short ud, short v);
short _far _pascal ibtrap (short mask, short value);
short _far _pascal ibwait (short ud, short mask);

short _far _pascal ibbna (short ud, char huge *udname);
short _far _pascal ibcmd (short ud, char huge *buf, unsigned long cnt);
short _far _pascal ibcmda (short ud, char huge *buf, unsigned long cnt);
short _far _pascal ibdiag (short ud, char huge *buf, unsigned long cnt);
short _far _pascal ibevent (short ud, unsigned short huge *event);
short _far _pascal iblines (short ud, unsigned short huge *result);
short _far _pascal ibrd (short ud, char huge *buf, unsigned long cnt);
short _far _pascal ibrda (short ud, char huge *buf, unsigned long cnt);
short _far _pascal ibrdf (short ud, char huge *filename);
short _far _pascal ibrdkey (short ud, char huge *buf, unsigned short cnt);
short _far _pascal ibrpp (short ud, char huge *ppr);
short _far _pascal ibrsp (short ud, char huge *spr);
short _far _pascal ibwrt (short ud, char huge *buf, unsigned long cnt);
short _far _pascal ibwrta (short ud, char huge *buf, unsigned long cnt);
short _far _pascal ibwrtf (short ud, char huge *filename);
short _far _pascal ibwrtkey (short ud, char huge *buf, unsigned short cnt);
short _far _pascal ibxtrc (short ud, char huge *buf, unsigned long cnt);

short _far _pascal ibfind (char huge *udname);
short _far _pascal ibconfig (short bdid, short option, short v);
short _far _pascal ibpoke (short ud, short option, short v);
short _far _pascal ibdev (short ud, short pad, short sad, short tmo, short eot, short eos);
short _far _pascal ibln (short ud, short pad, short sad, unsigned short huge *listen);

/**************************************************************************/
/*  NI-488.2 Function Prototypes  */
void _far _pascal SendCmds(short boardID, unsigned char huge *buf, unsigned long cnt);
void _far _pascal SendSetup(short boardID, unsigned short huge listen[]);
void _far _pascal SendDataBytes(short boardID, unsigned char huge *buffer, unsigned long cnt, unsigned short eot_mode);
void _far _pascal Send(short boardID, unsigned short listener, unsigned char huge *databuf, unsigned long datacnt, unsigned short eotMode);
void _far _pascal SendList(short boardID, unsigned short huge listeners[], unsigned char huge *databuf, unsigned long datacnt, unsigned short eotMode);
void _far _pascal ReceiveSetup(short boardID, unsigned short talker);
void _far _pascal RcvRespMsg(short boardID, unsigned char huge *buffer, unsigned long cnt, unsigned short eotMode);
void _far _pascal Receive(short boardID, unsigned short talker, unsigned char huge *buffer, unsigned long cnt, unsigned short eotMode);
void _far _pascal SendIFC(short boardID);
void _far _pascal DevClear(short boardID, unsigned short address);
void _far _pascal DevClearList(short boardID, unsigned short huge addrlist[]);
void _far _pascal EnableLocal(short boardID, unsigned short huge laddrs[]);
void _far _pascal EnableRemote(short boardID, unsigned short huge laddrs[]);
void _far _pascal SetRWLS(short boardID, unsigned short huge laddrs[]);
void _far _pascal SendLLO(short boardID);
void _far _pascal PassControl(short boardID, unsigned short talker);
void _far _pascal ReadStatusByte(short boardID, unsigned short talker, unsigned short huge *result);
void _far _pascal Trigger(short boardID, unsigned short laddrs);
void _far _pascal TriggerList(short boardID, unsigned short huge laddrs[]);
void _far _pascal PPollConfig(short boardID, unsigned short laddr, unsigned short dataLine, unsigned short lineSense);
void _far _pascal PPollUnconfig(short boardID, unsigned short huge laddrs[]);
void _far _pascal PPoll(short boardID, unsigned short huge *res_ptr);
void _far _pascal TestSRQ(short boardID, short huge *result);
void _far _pascal WaitSRQ(short boardID, short huge *result);
void _far _pascal ResetSys(short boardID, unsigned short huge laddrs[]);
void _far _pascal FindRQS(short boardID, unsigned short huge taddrs[], unsigned short huge *dev_stat);
void _far _pascal AllSpoll(short boardID, unsigned short huge taddrs[], unsigned short huge res[]);
void _far _pascal FindLstn(short boardID, unsigned short huge pads[], unsigned short huge results[], unsigned short limit);
void _far _pascal TestSys(short boardID, unsigned short huge addrs[], unsigned short huge result[]);


void _far _pascal GotoMultAddr(short boardID, unsigned short type,unsigned short (_far _loadds *addrfunc)(),unsigned short (_far _loadds *spollfunc)());
void _far _pascal GenerateREQT (short ud, unsigned short v);
void _far _pascal GenerateREQF (short ud, unsigned short v);

#ifdef __cplusplus
}
#endif

/***************************************************************************/
/*    HANDY CONSTANTS FOR USE BY APPLICATION PROGRAMS ...                  */
/***************************************************************************/

/* GPIB Commands                                           */

#define UNL  0x3f  /* GPIB unlisten command                */
#define UNT  0x5f  /* GPIB untalk command                  */
#define GTL  0x01  /* GPIB go to local                     */
#define SDC  0x04  /* GPIB selected device clear           */
#define PPC  0x05  /* GPIB parallel poll configure         */
#define GET  0x08  /* GPIB group execute trigger           */
#define TCT  0x09  /* GPIB take control                    */
#define LLO  0x11  /* GPIB local lock out                  */
#define DCL  0x14  /* GPIB device clear                    */
#define PPU  0x15  /* GPIB parallel poll unconfigure       */
#define SPE  0x18  /* GPIB serial poll enable              */
#define SPD  0x19  /* GPIB serial poll disable             */
#define PPE  0x60  /* GPIB parallel poll enable            */
#define PPD  0x70  /* GPIB parallel poll disable           */

/* GPIB status bit vector :                                */
/*       global variable ibsta and wait mask               */

#define ERR     (1<<15) /* Error detected                  */
#define TIMO    (1<<14) /* Timeout                         */
#define END     (1<<13) /* EOI or EOS detected             */
#define SRQI    (1<<12) /* SRQ detected by CIC             */
#define RQS     (1<<11) /* Device needs service            */
#define SPOLL   (1<<10) /* Board has been serially polled  */
#define EVENT   (1<<9)  /* An event has occured            */
#define CMPL    (1<<8)  /* I/O completed                   */
#define LOK     (1<<7)  /* Local lockout state             */
#define REM     (1<<6)  /* Remote state                    */
#define CIC     (1<<5)  /* Controller-in-Charge            */
#define ATN     (1<<4)  /* Attention asserted              */
#define TACS    (1<<3)  /* Talker active                   */
#define LACS    (1<<2)  /* Listener active                 */
#define DTAS    (1<<1)  /* Device trigger state            */
#define DCAS    (1<<0)  /* Device clear state              */

/* Error messages returned in global variable iberr        */

#define EDVR  0  /* DOS error                              */
#define ECIC  1  /* Function requires GPIB board to be CIC */
#define ENOL  2  /* Write function detected no Listeners   */
#define EADR  3  /* Interface board not addressed correctly*/
#define EARG  4  /* Invalid argument to function call      */
#define ESAC  5  /* Function requires GPIB board to be SAC */
#define EABO  6  /* I/O operation aborted                  */
#define ENEB  7  /* Non-existent interface board           */
#define EDMA  8  /* Error with allocating DMA buffer       */
#define EOIP 10  /* I/O operation started before previous  */
		 /* operation completed                    */
#define ECAP 11  /* No capability for intended operation   */
#define EFSO 12  /* File system operation error            */
#define EBUS 14  /* Command error during device call       */
#define ESTB 15  /* Serial poll status byte lost           */
#define ESRQ 16  /* SRQ remains asserted                   */
#define ETAB 20  /* The return buffer is full.             */

/* EOS mode bits                                           */

#define BIN  (1<<12) /* Eight bit compare                  */
#define XEOS (1<<11) /* Send END with EOS byte		   */
#define REOS (1<<10) /* Terminate read on EOS              */

/* Timeout values and meanings                             */

#define TNONE    0   /* Infinite timeout (disabled)        */
#define T10us    1   /* Timeout of 10 us (ideal)           */
#define T30us    2   /* Timeout of 30 us (ideal)           */
#define T100us   3   /* Timeout of 100 us (ideal)          */
#define T300us   4   /* Timeout of 300 us (ideal)          */
#define T1ms     5   /* Timeout of 1 ms (ideal)            */
#define T3ms     6   /* Timeout of 3 ms (ideal)            */
#define T10ms    7   /* Timeout of 10 ms (ideal)           */
#define T30ms    8   /* Timeout of 30 ms (ideal)           */
#define T100ms   9   /* Timeout of 100 ms (ideal)          */
#define T300ms  10   /* Timeout of 300 ms (ideal)          */
#define T1s     11   /* Timeout of 1 s (ideal)             */
#define T3s     12   /* Timeout of 3 s (ideal)             */
#define T10s    13   /* Timeout of 10 s (ideal)            */
#define T30s    14   /* Timeout of 30 s (ideal)            */
#define T100s   15   /* Timeout of 100 s (ideal)           */
#define T300s   15   /* Timeout of 300 s (ideal)           */
#define T1000s  15   /* Timeout of 1000 s (ideal)          */

/*  IBLN Constants                                         */

#define NO_SAD   0
#define ALL_SAD -1

/*  IBEVENT Constants                                      */
#define  EventDTAS   1
#define  EventDCAS   2


/*
 *    GotoMultAddr (VXI) flags
 */
#define  MultAddrPrimary      0x00
#define  MultAddrSecondary    0x01

#define  MultAddrListen       0x00
#define  MultAddrTalk         0x01
#define  MultAddrSerialPoll   0x81


/*  The following constants are used for the second parameter of the 
 *  ibconfig function.  They are the "option" selection codes.
 */
#define  IbcPAD        0x0001      /* Primary Address                      */
#define  IbcSAD        0x0002      /* Secondary Address                    */
#define  IbcTMO        0x0003      /* Timeout Value                        */
#define  IbcEOT        0x0004      /* Send EOI with last data byte?        */
#define  IbcPPC        0x0005      /* Parallel Poll Configure              */
#define  IbcREADDR     0x0006      /* Repeat Addressing                    */
#define  IbcAUTOPOLL   0x0007      /* Disable Auto Serial Polling          */
#define  IbcCICPROT    0x0008      /* Use the CIC Protocol?                */
#define  IbcIRQ        0x0009      /* Use PIO for I/O                      */
#define  IbcSC         0x000A      /* Board is System Controller?          */
#define  IbcSRE        0x000B      /* Assert SRE on device calls?          */
#define  IbcEOSrd      0x000C      /* Terminate reads on EOS               */
#define  IbcEOSwrt     0x000D      /* Send EOI with EOS character          */
#define  IbcEOScmp     0x000E      /* Use 7 or 8-bit EOS compare           */
#define  IbcEOSchar    0x000F      /* The EOS character.                   */
#define  IbcPP2        0x0010      /* Use Parallel Poll Mode 2.            */
#define  IbcTIMING     0x0011      /* NORMAL, HIGH, or VERY_HIGH timming.  */
#define  IbcDMA        0x0012      /* Use DMA for I/O                      */
#define  IbcReadAdjust 0x0013      /* Swap bytes during an ibrd.           */
#define  IbcWriteAdjust 0x014      /* Swap bytes during an ibwrt.          */
#define  IbcEventQueue 0x0015      /* Enable/disable the event queue.           */
#define  IbcSPollBit   0x0016      /* Enable/disable the visibility of SPOLL.   */
#define  IbcSendLLO    0x0017      /* Enable/disable the sending of LLO.      */
#define  IbcSPollTime  0x0018      /* Set the timeout value for serial polls. */
#define  IbcPPollTime  0x0019      /* Set the parallel poll length period.    */
#define  IbcNoEndBitOnEOS 0x001A   /* Remove EOS from END bit of IBSTA.       */
 
/*  488.2 CONSTANTS  */

/* These are the values are used by the Send 488.2 command. */
#define  NULLend (unsigned short) 0x00  /* Do nothing at the end of a transfer.*/
#define  NLend   (unsigned short) 0x01  /* Send NL with EOI after a transfer.  */
#define  DABend  (unsigned short) 0x02  /* Send EOI with the last DAB.         */


/* This value is used by the 488.2 Receive command.
 */
#define  STOPend     (unsigned int)0x0100


/*
 *  This macro can be used to easily create an entry in address list
 *  that is required by many of the 488.2 functions.  An address list is
 *  just an array of unsigned shorts.  The primary address goes in the
 *  lower 8-bits and the secondary address goes in the upper 8-bits.
 */
#define  MakeAddr(pad, sad)   ((unsigned short)(((unsigned char)(pad)) | (((unsigned char)(sad))<<8)))


/*
 *  The following two macros are used to "break apart" an address list
 *  entry.  They take an unsigned integer and return either the primary
 *  or secondary address stored in the integer.
 */
#define  GetPAD(val)    ((unsigned char)(val))
#define  GetSAD(val)    ((unsigned char)((val) >> 8))


/*
 *  The following values are used by the iblines function.  The unsigned
 *  integer returned by iblines contains:
 *       The lower byte will contain a "monitor" bit mask.  If a bit is
 *          set (1) in this mask, then the corresponding line can be
 *          monitiored by the driver.  If the bit is clear (0),
 *          then the line cannot be monitored.
 *       The upper byte will contain the status of the bus lines.
 *          Each bit corresponds to a certain bus line, and has
 *          a correcponding "monitor" bit in the lower byte.
 *
 */
#define  ValidEOI   (unsigned short)0x0080
#define  ValidATN   (unsigned short)0x0040
#define  ValidSRQ   (unsigned short)0x0020
#define  ValidREN   (unsigned short)0x0010
#define  ValidIFC   (unsigned short)0x0008
#define  ValidNRFD  (unsigned short)0x0004
#define  ValidNDAC  (unsigned short)0x0002
#define  ValidDAV   (unsigned short)0x0001
#define  BusEOI     (unsigned short)0x8000
#define  BusATN     (unsigned short)0x4000
#define  BusSRQ     (unsigned short)0x2000
#define  BusREN     (unsigned short)0x1000
#define  BusIFC     (unsigned short)0x0800
#define  BusNRFD    (unsigned short)0x0400
#define  BusNDAC    (unsigned short)0x0200
#define  BusDAV     (unsigned short)0x0100

/*
 *  This value is used to terminate an address list.  It should be
 *  assigned to the last entry.
 */
#define  NOADDR    0xFFFF

