#ifndef CONFIG_H_
#define CONFIG_H_

#include <common/Common.h>
#include <common/toolkit/list/StrCpyListIter.h>
#include <common/toolkit/list/UInt16ListIter.h>
#include <common/toolkit/tree/StrCpyMap.h>
#include <common/toolkit/tree/StrCpyMapIter.h>
#include <common/toolkit/StringTk.h>
#include <app/config/MountConfig.h>


struct Config;
typedef struct Config Config;

enum FileCacheType;
typedef enum FileCacheType FileCacheType;

enum InodeIDStyle;
typedef enum InodeIDStyle InodeIDStyle;

enum RDMAKeyType;
typedef enum RDMAKeyType RDMAKeyType;

enum CheckCapabilities;
typedef enum CheckCapabilities CheckCapabilities;

enum EventLogMask
{
   EventLogMask_NONE = 0,
   EventLogMask_FLUSH = 1,
   EventLogMask_TRUNC = 2,
   EventLogMask_SETATTR = 4,
   EventLogMask_CLOSE = 8,
   EventLogMask_LINK_OP = 16,
   EventLogMask_OPEN_READ = 32,
   EventLogMask_OPEN_WRITE = 64,
   EventLogMask_OPEN_READ_WRITE = 128
};


extern __must_check bool Config_init(Config* this, MountConfig* mountConfig);
extern Config* Config_construct(MountConfig* mountConfig);
extern void Config_uninit(Config* this);
extern void Config_destruct(Config* this);

extern bool _Config_initConfig(Config* this, MountConfig* mountConfig);
extern StrCpyMapIter _Config_eraseFromConfigMap(Config* this, StrCpyMapIter* iter);
extern void _Config_loadDefaults(Config* this);
extern bool _Config_applyConfigMap(Config* this);
extern void _Config_configMapRedefine(Config* this, char* keyStr, const char* valueStr);
extern void __Config_addLineToConfigMap(Config* this, char* line);

extern void __Config_loadFromMountConfig(Config* this, MountConfig* mountConfig);
extern bool __Config_loadFromFile(struct Config* this, const char* filename);
extern bool Config_loadStringListFile(const char* filename,
   StrCpyList* outList);
extern bool Config_loadUInt16ListFile(struct Config* this, const char* filename,
   UInt16List* outList);

extern bool __Config_initImplicitVals(Config* this);
extern void __Config_initConnNumCommRetries(Config* this);
extern void __Config_initTuneFileCacheTypeNum(Config* this);
void __Config_initSysInodeIDStyleNum(Config* this);
bool __Config_initConnAuthHash(Config* this, char* connAuthFile, uint64_t* outConnAuthHash);
void __Config_initConnRDMAKeyTypeNum(Config* this);

// conversion
const char* Config_fileCacheTypeNumToStr(FileCacheType cacheType);
const char* Config_inodeIDStyleNumToStr(InodeIDStyle inodeIDStyle);
const char* Config_eventLogMaskToStr(enum EventLogMask mask);
const char* Config_rdmaKeyTypeNumToStr(RDMAKeyType keyType);
const char* Config_checkCapabilitiesTypeToStr(CheckCapabilities checkCapabilities);

// getters & setters
static inline char* Config_getCfgFile(Config* this);
static inline int Config_getLogLevel(Config* this);
static inline bool Config_getLogClientID(Config* this);
static inline bool Config_getConnUseRDMA(Config* this);
static inline bool Config_getConnTCPFallbackEnabled(Config* this);
static inline int Config_getConnClientPort(Config* this);
static inline int Config_getConnMgmtdPort(Config* this);
static inline int Config_getConnMgmtdGrpcPort(Config* this);
static inline void Config_setConnMgmtdGrpcPort(Config* this, int port);
static inline unsigned Config_getConnMaxInternodeNum(Config* this);
static inline char* Config_getConnInterfacesFile(Config* this);
static inline char* Config_getConnInterfacesList(Config* this);
static inline char* Config_getConnRDMAInterfacesFile(Config* this);
static inline unsigned Config_getConnFallbackExpirationSecs(Config* this);
static inline unsigned Config_getConnNumCommRetries(Config* this);
static inline unsigned Config_getConnCommRetrySecs(Config* this);
static inline bool Config_getConnUnmountRetries(Config* this);
static inline int Config_getConnTCPRcvBufSize(Config* this);
static inline int Config_getConnUDPRcvBufSize(Config* this);
static inline unsigned Config_getConnRDMABufSize(Config* this);
static inline unsigned Config_getConnRDMAFragmentSize(Config* this);
static inline unsigned Config_getConnRDMABufNum(Config* this);
static inline unsigned Config_getConnRDMAMetaBufSize(Config* this);
static inline unsigned Config_getConnRDMAMetaBufNum(Config* this);
static inline unsigned Config_getConnRDMAMetaFragmentSize(Config* this);
static inline char* Config_getConnRDMAKeyType(Config* this);
static inline RDMAKeyType Config_getConnRDMAKeyTypeNum(Config* this);
static inline int Config_getConnRDMATypeOfService(Config* this);
static inline unsigned Config_getRemapConnectionFailureStatus(Config* this);
static inline void Config_setRemapConnectionFailureStatus(Config* this, unsigned status);
static inline char* Config_getConnNetFilterFile(Config* this);
static inline unsigned Config_getConnMaxConcurrentAttempts(Config* this);
static inline char* Config_getConnAuthFile(Config* this);
static inline bool Config_getConnDisableAuthentication(Config* this);
static inline uint64_t Config_getConnAuthHash(Config* this);
static inline char* Config_getConnTcpOnlyFilterFile(Config* this);
static inline char* Config_getTunePreferredMetaFile(Config* this);
static inline char* Config_getTunePreferredStorageFile(Config* this);
static inline char* Config_getTuneFileCacheType(Config* this);
static inline FileCacheType Config_getTuneFileCacheTypeNum(Config* this);
static inline int Config_getTuneFileCacheBufSize(Config* this);
static inline int Config_getTuneFileCacheBufNum(Config* this);
static inline int Config_getTunePathBufSize(Config* this);
static inline int Config_getTunePathBufNum(Config* this);
static inline int Config_getTuneMsgBufSize(Config* this);
static inline int Config_getTuneMsgBufNum(Config* this);
static inline unsigned Config_getTunePageCacheValidityMS(Config* this);
static inline unsigned Config_getTuneDirSubentryCacheValidityMS(Config* this);
static inline unsigned Config_getTuneFileSubentryCacheValidityMS(Config* this);
static inline unsigned Config_getTuneENOENTCacheValidityMS(Config* this);
static inline bool Config_getTuneRemoteFSync(Config* this);
static inline bool Config_getTuneUseGlobalFileLocks(Config* this);
static inline bool Config_getTuneRefreshOnGetAttr(Config* this);
static inline void Config_setTuneRefreshOnGetAttr(Config* this);
static inline unsigned Config_getTuneInodeBlockBits(Config* this);
static inline unsigned Config_getTuneInodeBlockSize(Config* this);
static inline bool Config_getTuneEarlyCloseResponse(Config* this);
static inline bool Config_getTuneUseGlobalAppendLocks(Config* this);
static inline bool Config_getTuneUseBufferedAppend(Config* this);
static inline unsigned Config_getTuneStatFsCacheSecs(Config* this);
static inline bool Config_getTuneCoherentBuffers(Config* this);

static inline char* Config_getSysMgmtdHost(Config* this);
static inline char* Config_getSysInodeIDStyle(Config* this);
static inline InodeIDStyle Config_getSysInodeIDStyleNum(Config* this);
static inline bool Config_getSysCacheInvalidationVersion(Config* this);
static inline bool Config_getSysCreateHardlinksAsSymlinks(Config* this);
static inline unsigned Config_getSysMountSanityCheckMS(Config* this);
static inline bool Config_getSysSyncOnClose(Config* this);
static inline bool Config_getSysSessionCheckOnClose(Config* this);
static inline bool Config_getSysSessionChecksEnabled(Config* this);
static inline unsigned Config_getSysUpdateTargetStatesSecs(Config* this);
static inline unsigned Config_getSysTargetOfflineTimeoutSecs(Config* this);
static inline bool Config_getSysXAttrsEnabled(Config* this);
static inline CheckCapabilities Config_getSysXAttrsCheckCapabilities (Config* this);
static inline bool Config_getSysACLsEnabled(Config* this);
static inline bool Config_getSysXAttrsImplicitlyEnabled(Config* this);
static inline bool Config_getSysBypassFileAccessCheckOnMeta(Config* this);

static inline bool Config_getQuotaEnabled(Config* this);
static inline char* Config_getConnMessagingTimeouts(Config* this);
static inline char* Config_getConnRDMATimeouts(Config* this);
static inline int Config_getConnRDMATimeoutConnect(Config* this);
static inline int Config_getConnRDMATimeoutCompletion(Config* this);
static inline int Config_getConnRDMATimeoutFlowSend(Config* this);
static inline int Config_getConnRDMATimeoutFlowRecv(Config* this);
static inline int Config_getConnRDMATimeoutPoll(Config* this);


enum FileCacheType
{ FILECACHETYPE_None = 0, FILECACHETYPE_Buffered = 1, FILECACHETYPE_Paged = 2,
  FILECACHETYPE_Native = 3};


#define INODEIDSTYLE_HASH32HSIEH_STR   "hash32"
#define INODEIDSTYLE_HASH64HSIEH_STR   "hash64"
#define INODEIDSTYLE_HASH32MD4_STR     "md4hash32"
#define INODEIDSTYLE_HASH64MD4_STR     "md4hash64"
#define INODEIDSTYLE_DEFAULT           INODEIDSTYLE_HASH64MD4_STR

enum InodeIDStyle
{
   INODEIDSTYLE_Hash32Hsieh     = 0,    // hsieh32
   INODEIDSTYLE_Hash32MD4,              // half-md4
   INODEIDSTYLE_Hash64HSieh,            // hsieh32
   INODEIDSTYLE_Hash64MD4               // half-md4
};
#define INODEIDSTYLE_Default  INODEIDSTYLE_Hash64MD4

#define RDMAKEYTYPE_UNSAFE_GLOBAL_STR   "global"
#define RDMAKEYTYPE_UNSAFE_DMA_STR      "dma"
#define RDMAKEYTYPE_REGISTER_STR        "register"

enum RDMAKeyType
{
   RDMAKEYTYPE_UnsafeGlobal = 0,
   RDMAKEYTYPE_UnsafeDMA,
   RDMAKEYTYPE_Register
};

#define CHECKCAPABILITIES_ALWAYS_STR    "always"
#define CHECKCAPABILITIES_CACHE_STR     "cache"
#define CHECKCAPABILITIES_NEVER_STR     "never"

enum CheckCapabilities
{
   CHECKCAPABILITIES_Always = 0,
   CHECKCAPABILITIES_Cache,
   CHECKCAPABILITIES_Never
};

struct Config
{
   // configurables
   char*          cfgFile;

   int            logLevel;
   bool           logClientID;

   int            connPortShift; // shifts all UDP and TCP ports
   int            connClientPort;
   int            connMgmtdPort;
   int            connMgmtdGrpcPort; // pulled from mgmtd and not meant to be configured by the user
   bool           connUseRDMA;
   bool           connTCPFallbackEnabled;
   unsigned       connMaxInternodeNum;
   char*          connInterfacesFile;
   char*          connInterfacesList;
   char*          connRDMAInterfacesFile;
   unsigned       connFallbackExpirationSecs;
   unsigned       connNumCommRetries; // auto-computed from connCommRetrySecs
   unsigned       connCommRetrySecs;
   bool           connUnmountRetries;
   int            connTCPRcvBufSize;
   int            connUDPRcvBufSize;
   int            connRDMABufSize;
   int            connRDMAFragmentSize;
   int            connRDMABufNum;
   int            connRDMAMetaBufSize;
   int            connRDMAMetaFragmentSize;
   int            connRDMAMetaBufNum;
   int            connRDMATypeOfService;
   char*          connRDMAKeyType;
   RDMAKeyType    connRDMAKeyTypeNum;
   char*          connNetFilterFile; // allowed IP addresses (all IPs allowed, if empty)
   unsigned       connMaxConcurrentAttempts;
   char*          connAuthFile;
   bool           connDisableAuthentication;
   uint64_t       connAuthHash; // implicitly set based on hash of connAuthFile contents
   char*          connTcpOnlyFilterFile; // allow only plain TCP (no RDMA etc) to these IPs

   char*          connMessagingTimeouts;
   int            connMsgLongTimeout;
   int            connMsgMediumTimeout;
   int            connMsgShortTimeout; // connection (response) timeouts in ms
                                       // note: be careful here, because servers not
                                       // responding for >30secs under high load is nothing
                                       // unusual, so never use connMsgShortTimeout for
                                       // IO-related operations.
   char*          connRDMATimeouts;
   int            connRDMATimeoutConnect;
   int            connRDMATimeoutCompletion;
   int            connRDMATimeoutFlowSend;
   int            connRDMATimeoutFlowRecv;
   int            connRDMATimeoutPoll;

   char*          tunePreferredMetaFile;
   char*          tunePreferredStorageFile;
   char*          tuneFileCacheType;
   FileCacheType  tuneFileCacheTypeNum; // auto-generated based on tuneFileCacheType
   int            tuneFileCacheBufSize;
   int            tuneFileCacheBufNum; // 0 means automatic setting
   unsigned       tunePageCacheValidityMS;
   unsigned       tuneDirSubentryCacheValidityMS;
   unsigned       tuneFileSubentryCacheValidityMS;
   unsigned       tuneENOENTCacheValidityMS;
   int            tunePathBufSize;
   int            tunePathBufNum;
   int            tuneMsgBufSize;
   int            tuneMsgBufNum; // 0 means automatic setting
   bool     tuneRemoteFSync;
   bool     tuneUseGlobalFileLocks; // false means local flock/fcntl locks
   bool     tuneRefreshOnGetAttr; // false means don't refresh on getattr
   unsigned       tuneInodeBlockBits; // bitshift for optimal io size seen by stat() (2^n)
   unsigned       tuneInodeBlockSize; // auto-generated based on tuneInodeBlockBits
   bool     tuneEarlyCloseResponse; // don't wait for chunk files close result
   bool     tuneUseGlobalAppendLocks; // false means local append locks
   bool     tuneUseBufferedAppend; // false disables buffering of append writes
   unsigned       tuneStatFsCacheSecs; // 0 disables caching of free space info from servers
   bool           tuneCoherentBuffers; // try to keep buffer cache and page cache coherent

   char*          sysMgmtdHost;
   char*          sysInodeIDStyle;
   InodeIDStyle   sysInodeIDStyleNum; // auto-generated based on sysInodeIDStyle
   bool     sysCacheInvalidationVersion;
   bool     sysCreateHardlinksAsSymlinks;
   unsigned       sysMountSanityCheckMS;
   bool     sysSyncOnClose;
   bool     sysSessionCheckOnClose;
   bool     sysSessionChecksEnabled;
   unsigned       sysUpdateTargetStatesSecs;
   unsigned       sysTargetOfflineTimeoutSecs;

   bool     sysXAttrsEnabled;
   CheckCapabilities   sysXAttrsCheckCapabilities;
   bool     sysACLsEnabled;
   bool     sysXAttrsImplicitlyEnabled; // True when XAttrs have not been enabled in the config file
                                        // but have been enabled by __Config_initImplicitVals
                                        // because ACLs are enabled in the config and XAs are needed
                                        // to store the ACLs.
   bool     sysBypassFileAccessCheckOnMeta; // bypass file access check on meta server

   bool     quotaEnabled;
   enum EventLogMask eventLogMask;

   /* workaround for rename of closed files on nfs */
   bool sysRenameEbusyAsXdev;


   // internals
   StrCpyMap configMap;

   // testing
   unsigned  remapConnectionFailureStatus;
};

char* Config_getCfgFile(Config* this)
{
   return this->cfgFile;
}

int Config_getLogLevel(Config* this)
{
   return this->logLevel;
}

bool Config_getLogClientID(Config* this)
{
   return this->logClientID;
}

int Config_getConnClientPort(Config* this)
{
   return this->connClientPort ? (this->connClientPort + this->connPortShift) : 0;
}

int Config_getConnMgmtdPort(Config* this)
{
   return this->connMgmtdPort ? (this->connMgmtdPort + this->connPortShift) : 0;
}

int Config_getConnMgmtdGrpcPort(Config* this)
{
   // not adding port shift here because connMgmtdGrpcPort is pulled from mgmtd and already shifted
   return this->connMgmtdGrpcPort ? this->connMgmtdGrpcPort : 0;
}

void Config_setConnMgmtdGrpcPort(Config* this, int port)
{
   this->connMgmtdGrpcPort = port;
}

bool Config_getConnUseRDMA(Config* this)
{
   return this->connUseRDMA;
}

bool Config_getConnTCPFallbackEnabled(Config* this)
{
   return this->connTCPFallbackEnabled;
}

unsigned Config_getConnMaxInternodeNum(Config* this)
{
   return this->connMaxInternodeNum;
}

char* Config_getConnInterfacesFile(Config* this)
{
   return this->connInterfacesFile;
}

char* Config_getConnInterfacesList(Config* this)
{
   return this->connInterfacesList;
}

char* Config_getConnRDMAInterfacesFile(Config* this)
{
   return this->connRDMAInterfacesFile;
}

unsigned Config_getConnFallbackExpirationSecs(Config* this)
{
   return this->connFallbackExpirationSecs;
}

unsigned Config_getConnNumCommRetries(Config* this)
{
   return this->connNumCommRetries;
}

unsigned Config_getConnCommRetrySecs(Config* this)
{
   return this->connCommRetrySecs;
}

bool Config_getConnUnmountRetries(Config* this)
{
   return this->connUnmountRetries;
}

int Config_getConnTCPRcvBufSize(Config* this)
{
   return this->connTCPRcvBufSize;
}

int Config_getConnUDPRcvBufSize(Config* this)
{
   return this->connUDPRcvBufSize;
}

unsigned Config_getConnRDMABufSize(Config* this)
{
   return (unsigned) this->connRDMABufSize;
}

unsigned Config_getConnRDMAFragmentSize(Config* this)
{
   return (unsigned) this->connRDMAFragmentSize;
}

unsigned Config_getConnRDMABufNum(Config* this)
{
   return (unsigned) this->connRDMABufNum;
}

unsigned Config_getConnRDMAMetaBufSize(Config* this)
{
   return (unsigned) this->connRDMAMetaBufSize;
}

unsigned Config_getConnRDMAMetaFragmentSize(Config* this)
{
   return (unsigned) this->connRDMAMetaFragmentSize;
}

unsigned Config_getConnRDMAMetaBufNum(Config* this)
{
   return (unsigned) this->connRDMAMetaBufNum;
}

int Config_getConnRDMATypeOfService(Config* this)
{
   return this->connRDMATypeOfService;
}

char* Config_getConnRDMAKeyType(Config* this)
{
   return this->connRDMAKeyType;
}

RDMAKeyType Config_getConnRDMAKeyTypeNum(Config* this)
{
   return this->connRDMAKeyTypeNum;
}

unsigned Config_getRemapConnectionFailureStatus(Config* this)
{
   return this->remapConnectionFailureStatus;
}

void Config_setRemapConnectionFailureStatus(Config* this, unsigned status)
{
   this->remapConnectionFailureStatus = status;
}

char* Config_getConnNetFilterFile(Config* this)
{
   return this->connNetFilterFile;
}

unsigned Config_getConnMaxConcurrentAttempts(Config* this)
{
   return this->connMaxConcurrentAttempts;
}

char* Config_getConnAuthFile(Config* this)
{
   return this->connAuthFile;
}

bool Config_getConnDisableAuthentication(Config* this)
{
   return this->connDisableAuthentication;
}

uint64_t Config_getConnAuthHash(Config* this)
{
   return this->connAuthHash;
}

char* Config_getConnTcpOnlyFilterFile(Config* this)
{
   return this->connTcpOnlyFilterFile;
}

char* Config_getTunePreferredMetaFile(Config* this)
{
   return this->tunePreferredMetaFile;
}

char* Config_getTunePreferredStorageFile(Config* this)
{
   return this->tunePreferredStorageFile;
}

char* Config_getTuneFileCacheType(Config* this)
{
   return this->tuneFileCacheType;
}

FileCacheType Config_getTuneFileCacheTypeNum(Config* this)
{
   return this->tuneFileCacheTypeNum;
}

int Config_getTuneFileCacheBufSize(Config* this)
{
   return this->tuneFileCacheBufSize;
}

int Config_getTuneFileCacheBufNum(Config* this)
{
   return this->tuneFileCacheBufNum;
}

int Config_getTunePathBufSize(Config* this)
{
   return this->tunePathBufSize;
}

int Config_getTunePathBufNum(Config* this)
{
   return this->tunePathBufNum;
}

int Config_getTuneMsgBufSize(Config* this)
{
   return this->tuneMsgBufSize;
}

int Config_getTuneMsgBufNum(Config* this)
{
   return this->tuneMsgBufNum;
}

unsigned Config_getTunePageCacheValidityMS(Config* this)
{
   return this->tunePageCacheValidityMS;
}

unsigned Config_getTuneDirSubentryCacheValidityMS(Config* this)
{
   return this->tuneDirSubentryCacheValidityMS;
}

unsigned Config_getTuneFileSubentryCacheValidityMS(Config* this)
{
   return this->tuneFileSubentryCacheValidityMS;
}

unsigned Config_getTuneENOENTCacheValidityMS(Config* this)
{
   return this->tuneENOENTCacheValidityMS;
}

bool Config_getTuneRemoteFSync(Config* this)
{
   return this->tuneRemoteFSync;
}

bool Config_getTuneUseGlobalFileLocks(Config* this)
{
   return this->tuneUseGlobalFileLocks;
}

bool Config_getTuneRefreshOnGetAttr(Config* this)
{
   return this->tuneRefreshOnGetAttr;
}

bool Config_getTuneCoherentBuffers(Config* this)
{
   return this->tuneCoherentBuffers;
}

/**
 * Special function to automatically enable TuneRefreshOnGetAttr, e.g. for NFS exports.
 *
 * Note: We do not use any locks here assuming the right value will propate to all cores rather
 *       soon.
 */
void Config_setTuneRefreshOnGetAttr(Config* this)
{
   this->tuneRefreshOnGetAttr = true;

   // do a memory barrier, so that other CPUs get the new value as soon as possible
   smp_wmb();
}


unsigned Config_getTuneInodeBlockBits(Config* this)
{
   return this->tuneInodeBlockBits;
}

unsigned Config_getTuneInodeBlockSize(Config* this)
{
   return this->tuneInodeBlockSize;
}

bool Config_getTuneEarlyCloseResponse(Config* this)
{
   return this->tuneEarlyCloseResponse;
}

bool Config_getTuneUseGlobalAppendLocks(Config* this)
{
   return this->tuneUseGlobalAppendLocks;
}

bool Config_getTuneUseBufferedAppend(Config* this)
{
   return this->tuneUseBufferedAppend;
}

unsigned Config_getTuneStatFsCacheSecs(Config* this)
{
   return this->tuneStatFsCacheSecs;
}

char* Config_getSysMgmtdHost(Config* this)
{
   return this->sysMgmtdHost;
}

char* Config_getSysInodeIDStyle(Config* this)
{
   return this->sysInodeIDStyle;
}

InodeIDStyle Config_getSysInodeIDStyleNum(Config* this)
{
   return this->sysInodeIDStyleNum;
}

bool Config_getSysCacheInvalidationVersion(Config* this)
{
   return this->sysCacheInvalidationVersion;
}

bool Config_getSysCreateHardlinksAsSymlinks(Config* this)
{
   return this->sysCreateHardlinksAsSymlinks;
}

unsigned Config_getSysMountSanityCheckMS(Config* this)
{
   return this->sysMountSanityCheckMS;
}

bool Config_getSysSyncOnClose(Config* this)
{
   return this->sysSyncOnClose;
}

bool Config_getSysSessionCheckOnClose(Config* this)
{
   return this->sysSessionCheckOnClose;
}

bool Config_getSysSessionChecksEnabled(Config* this)
{
   return this->sysSessionChecksEnabled;
}

unsigned Config_getSysUpdateTargetStatesSecs(Config* this)
{
   return this->sysUpdateTargetStatesSecs;
}

unsigned Config_getSysTargetOfflineTimeoutSecs(Config* this)
{
   return this->sysTargetOfflineTimeoutSecs;
}

bool Config_getSysXAttrsEnabled(Config* this)
{
   return this->sysXAttrsEnabled;
}

CheckCapabilities Config_getSysXAttrsCheckCapabilities(Config* this)
{
   return this->sysXAttrsCheckCapabilities;
}

bool Config_getSysACLsEnabled(Config* this)
{
   return this->sysACLsEnabled;
}

bool Config_getSysXAttrsImplicitlyEnabled(Config* this)
{
   return this->sysXAttrsImplicitlyEnabled;
}

bool Config_getSysBypassFileAccessCheckOnMeta(Config* this)
{
    return this->sysBypassFileAccessCheckOnMeta;
}

bool Config_getQuotaEnabled(Config* this)
{
   return this->quotaEnabled;
}

char* Config_getConnMessagingTimeouts(Config* this)
{
   return this->connMessagingTimeouts;
}

char* Config_getConnRDMATimeouts(Config* this)
{
   return this->connRDMATimeouts;
}

int Config_getConnRDMATimeoutConnect(Config* this)
{
   return this->connRDMATimeoutConnect;
}

int Config_getConnRDMATimeoutCompletion(Config* this)
{
   return this->connRDMATimeoutCompletion;
}

int Config_getConnRDMATimeoutFlowSend(Config* this)
{
   return this->connRDMATimeoutFlowSend;
}

int Config_getConnRDMATimeoutFlowRecv(Config* this)
{
   return this->connRDMATimeoutFlowRecv;
}

int Config_getConnRDMATimeoutPoll(Config* this)
{
   return this->connRDMATimeoutPoll;
}

#endif /*CONFIG_H_*/
