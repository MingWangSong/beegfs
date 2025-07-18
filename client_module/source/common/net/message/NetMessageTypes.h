#ifndef NETMESSAGETYPES_H_
#define NETMESSAGETYPES_H_

/* This file MUST be kept in sync with the corresponding fhgfs_common file!
   See fhgfs_common/source/common/net/message/NetMessageTypes.h */

// invalid messages
#define NETMSGTYPE_Invalid                            0

// nodes messages
#define NETMSGTYPE_RemoveNode                        1013
#define NETMSGTYPE_RemoveNodeResp                    1014
#define NETMSGTYPE_GetNodes                          1017
#define NETMSGTYPE_GetNodesResp                      1018
#define NETMSGTYPE_HeartbeatRequest                  1019
#define NETMSGTYPE_Heartbeat                         1020
#define NETMSGTYPE_GetNodeCapacityPools              1021
#define NETMSGTYPE_GetNodeCapacityPoolsResp          1022
#define NETMSGTYPE_MapTargets                        1023
#define NETMSGTYPE_MapTargetsResp                    1024
#define NETMSGTYPE_GetTargetMappings                 1025
#define NETMSGTYPE_GetTargetMappingsResp             1026
#define NETMSGTYPE_UnmapTarget                       1027
#define NETMSGTYPE_UnmapTargetResp                   1028
#define NETMSGTYPE_GenericDebug                      1029
#define NETMSGTYPE_GenericDebugResp                  1030
#define NETMSGTYPE_GetClientStats                    1031
#define NETMSGTYPE_GetClientStatsResp                1032
#define NETMSGTYPE_RefreshCapacityPools              1035
#define NETMSGTYPE_StorageBenchControlMsg            1037
#define NETMSGTYPE_StorageBenchControlMsgResp        1038
#define NETMSGTYPE_RegisterNode                      1039
#define NETMSGTYPE_RegisterNodeResp                  1040
#define NETMSGTYPE_RegisterTarget                    1041
#define NETMSGTYPE_RegisterTargetResp                1042
#define NETMSGTYPE_SetMirrorBuddyGroup               1045
#define NETMSGTYPE_SetMirrorBuddyGroupResp           1046
#define NETMSGTYPE_GetMirrorBuddyGroups              1047
#define NETMSGTYPE_GetMirrorBuddyGroupsResp          1048
#define NETMSGTYPE_GetTargetStates                   1049
#define NETMSGTYPE_GetTargetStatesResp               1050
#define NETMSGTYPE_RefreshTargetStates               1051
#define NETMSGTYPE_GetStatesAndBuddyGroups           1053
#define NETMSGTYPE_GetStatesAndBuddyGroupsResp       1054
#define NETMSGTYPE_SetTargetConsistencyStates        1055
#define NETMSGTYPE_SetTargetConsistencyStatesResp    1056
#define NETMSGTYPE_ChangeTargetConsistencyStates     1057
#define NETMSGTYPE_ChangeTargetConsistencyStatesResp 1058
#define NETMSGTYPE_PublishCapacities                 1059
#define NETMSGTYPE_RemoveBuddyGroup                  1060
#define NETMSGTYPE_RemoveBuddyGroupResp              1061
#define NETMSGTYPE_GetTargetConsistencyStates        1062
#define NETMSGTYPE_GetTargetConsistencyStatesResp    1063


// storage messages
#define NETMSGTYPE_MkDir                           2001
#define NETMSGTYPE_MkDirResp                       2002
#define NETMSGTYPE_RmDir                           2003
#define NETMSGTYPE_RmDirResp                       2004
#define NETMSGTYPE_MkFile                          2005
#define NETMSGTYPE_MkFileResp                      2006
#define NETMSGTYPE_UnlinkFile                      2007
#define NETMSGTYPE_UnlinkFileResp                  2008
#define NETMSGTYPE_UnlinkLocalFile                 2011
#define NETMSGTYPE_UnlinkLocalFileResp             2012
#define NETMSGTYPE_Stat                            2015
#define NETMSGTYPE_StatResp                        2016
#define NETMSGTYPE_GetChunkFileAttribs             2017
#define NETMSGTYPE_GetChunkFileAttribsResp         2018
#define NETMSGTYPE_TruncFile                       2019
#define NETMSGTYPE_TruncFileResp                   2020
#define NETMSGTYPE_TruncLocalFile                  2021
#define NETMSGTYPE_TruncLocalFileResp              2022
#define NETMSGTYPE_Rename                          2023
#define NETMSGTYPE_RenameResp                      2024
#define NETMSGTYPE_SetAttr                         2025
#define NETMSGTYPE_SetAttrResp                     2026
#define NETMSGTYPE_ListDirFromOffset               2029
#define NETMSGTYPE_ListDirFromOffsetResp           2030
#define NETMSGTYPE_StatStoragePath                 2031
#define NETMSGTYPE_StatStoragePathResp             2032
#define NETMSGTYPE_SetLocalAttr                    2033
#define NETMSGTYPE_SetLocalAttrResp                2034
#define NETMSGTYPE_FindOwner                       2035
#define NETMSGTYPE_FindOwnerResp                   2036
#define NETMSGTYPE_MkLocalDir                      2037
#define NETMSGTYPE_MkLocalDirResp                  2038
#define NETMSGTYPE_RmLocalDir                      2039
#define NETMSGTYPE_RmLocalDirResp                  2040
#define NETMSGTYPE_MovingFileInsert                2041
#define NETMSGTYPE_MovingFileInsertResp            2042
#define NETMSGTYPE_MovingDirInsert                 2043
#define NETMSGTYPE_MovingDirInsertResp             2044
#define NETMSGTYPE_GetEntryInfo                    2045
#define NETMSGTYPE_GetEntryInfoResp                2046
#define NETMSGTYPE_SetDirPattern                   2047
#define NETMSGTYPE_SetDirPatternResp               2048
#define NETMSGTYPE_GetHighResStats                 2051
#define NETMSGTYPE_GetHighResStatsResp             2052
#define NETMSGTYPE_MkFileWithPattern               2053
#define NETMSGTYPE_MkFileWithPatternResp           2054
#define NETMSGTYPE_RefreshEntryInfo                2055
#define NETMSGTYPE_RefreshEntryInfoResp            2056
#define NETMSGTYPE_RmDirEntry                      2057
#define NETMSGTYPE_RmDirEntryResp                  2058
#define NETMSGTYPE_LookupIntent                    2059
#define NETMSGTYPE_LookupIntentResp                2060
#define NETMSGTYPE_FindLinkOwner                   2063
#define NETMSGTYPE_FindLinkOwnerResp               2064
#define NETMSGTYPE_MirrorMetadata                  2067
#define NETMSGTYPE_MirrorMetadataResp              2068
#define NETMSGTYPE_SetMetadataMirroring            2069
#define NETMSGTYPE_SetMetadataMirroringResp        2070
#define NETMSGTYPE_Hardlink                        2071
#define NETMSGTYPE_HardlinkResp                    2072
#define NETMSGTYPE_SetQuota                        2075
#define NETMSGTYPE_SetQuotaResp                    2076
#define NETMSGTYPE_SetExceededQuota                2077
#define NETMSGTYPE_SetExceededQuotaResp            2078
#define NETMSGTYPE_RequestExceededQuota            2079
#define NETMSGTYPE_RequestExceededQuotaResp        2080
#define NETMSGTYPE_UpdateDirParent                 2081
#define NETMSGTYPE_UpdateDirParentResp             2082
#define NETMSGTYPE_ResyncLocalFile                 2083
#define NETMSGTYPE_ResyncLocalFileResp             2084
#define NETMSGTYPE_StartStorageTargetResync        2085
#define NETMSGTYPE_StartStorageTargetResyncResp    2086
#define NETMSGTYPE_StorageResyncStarted            2087
#define NETMSGTYPE_StorageResyncStartedResp        2088
#define NETMSGTYPE_ListChunkDirIncremental         2089
#define NETMSGTYPE_ListChunkDirIncrementalResp     2090
#define NETMSGTYPE_RmChunkPaths                    2091
#define NETMSGTYPE_RmChunkPathsResp                2092
#define NETMSGTYPE_GetStorageResyncStats           2093
#define NETMSGTYPE_GetStorageResyncStatsResp       2094
#define NETMSGTYPE_SetLastBuddyCommOverride        2095
#define NETMSGTYPE_SetLastBuddyCommOverrideResp    2096
#define NETMSGTYPE_GetQuotaInfo                    2097
#define NETMSGTYPE_GetQuotaInfoResp                2098
#define NETMSGTYPE_SetStorageTargetInfo            2099
#define NETMSGTYPE_SetStorageTargetInfoResp        2100
#define NETMSGTYPE_ListXAttr                       2101
#define NETMSGTYPE_ListXAttrResp                   2102
#define NETMSGTYPE_GetXAttr                        2103
#define NETMSGTYPE_GetXAttrResp                    2104
#define NETMSGTYPE_RemoveXAttr                     2105
#define NETMSGTYPE_RemoveXAttrResp                 2106
#define NETMSGTYPE_SetXAttr                        2107
#define NETMSGTYPE_SetXAttrResp                    2108
#define NETMSGTYPE_GetDefaultQuota                 2109
#define NETMSGTYPE_GetDefaultQuotaResp             2110
#define NETMSGTYPE_SetDefaultQuota                 2111
#define NETMSGTYPE_SetDefaultQuotaResp             2112
#define NETMSGTYPE_ResyncSessionStore              2113
#define NETMSGTYPE_ResyncSessionStoreResp          2114
#define NETMSGTYPE_ResyncRawInodes                 2115
#define NETMSGTYPE_ResyncRawInodesResp             2116
#define NETMSGTYPE_GetMetaResyncStats              2117
#define NETMSGTYPE_GetMetaResyncStatsResp          2118
#define NETMSGTYPE_MoveFileInode                   2119
#define NETMSGTYPE_MoveFileInodeResp               2120
#define NETMSGTYPE_UnlinkLocalFileInode            2121
#define NETMSGTYPE_UnlinkLocalFileInodeResp        2122
#define NETMSGTYPE_SetFilePattern                  2123
#define NETMSGTYPE_SetFilePatternResp              2124
#define NETMSGTYPE_CpChunkPaths                    2125
#define NETMSGTYPE_CpChunkPathsResp                2126
#define NETMSGTYPE_ChunkBalance                    2127
#define NETMSGTYPE_ChunkBalanceResp                2128
#define NETMSGTYPE_StripePatternUpdate             2129
#define NETMSGTYPE_StripePatternUpdateResp         2130
#define NETMSGTYPE_SetFileState                    2131
#define NETMSGTYPE_SetFileStateResp                2132

// session messages
#define NETMSGTYPE_OpenFile                        3001
#define NETMSGTYPE_OpenFileResp                    3002
#define NETMSGTYPE_CloseFile                       3003
#define NETMSGTYPE_CloseFileResp                   3004
#define NETMSGTYPE_OpenLocalFile                   3005
#define NETMSGTYPE_OpenLocalFileResp               3006
#define NETMSGTYPE_CloseChunkFile                  3007
#define NETMSGTYPE_CloseChunkFileResp              3008
#define NETMSGTYPE_WriteLocalFile                  3009
#define NETMSGTYPE_WriteLocalFileResp              3010
#define NETMSGTYPE_FSyncLocalFile                  3013
#define NETMSGTYPE_FSyncLocalFileResp              3014
#define NETMSGTYPE_ReadLocalFileV2                 3019
#define NETMSGTYPE_RefreshSession                  3021
#define NETMSGTYPE_RefreshSessionResp              3022
#define NETMSGTYPE_LockGranted                     3023
#define NETMSGTYPE_FLockEntry                      3025
#define NETMSGTYPE_FLockEntryResp                  3026
#define NETMSGTYPE_FLockRange                      3027
#define NETMSGTYPE_FLockRangeResp                  3028
#define NETMSGTYPE_FLockAppend                     3029
#define NETMSGTYPE_FLockAppendResp                 3030
#define NETMSGTYPE_AckNotify                       3031
#define NETMSGTYPE_AckNotifyResp                   3032
#define NETMSGTYPE_BumpFileVersion                 3033
#define NETMSGTYPE_BumpFileVersionResp             3034
#define NETMSGTYPE_GetFileVersion                  3035
#define NETMSGTYPE_GetFileVersionResp              3036
#ifdef BEEGFS_NVFS
#define NETMSGTYPE_WriteLocalFileRDMA              3037
#define NETMSGTYPE_WriteLocalFileRDMAResp          3038
#define NETMSGTYPE_ReadLocalFileRDMA               3039
#define NETMSGTYPE_ReadLocalFileRDMAResp           3040
#endif

// control messages
#define NETMSGTYPE_SetChannelDirect                4001
#define NETMSGTYPE_Ack                             4003
#define NETMSGTYPE_Dummy                           4005
#define NETMSGTYPE_AuthenticateChannel             4007
#define NETMSGTYPE_GenericResponse                 4009
#define NETMSGTYPE_PeerInfo                        4011

// mon messages
#define NETMSGTYPE_GetNodesFromRootMetaNode        6001
#define NETMSGTYPE_SendNodesList                   6002
#define NETMSGTYPE_RequestMetaData                 6003
#define NETMSGTYPE_RequestStorageData              6004
#define NETMSGTYPE_RequestMetaDataResp             6005
#define NETMSGTYPE_RequestStorageDataResp          6006

// fsck messages
#define NETMSGTYPE_RetrieveDirEntries              7001
#define NETMSGTYPE_RetrieveDirEntriesResp          7002
#define NETMSGTYPE_RetrieveInodes                  7003
#define NETMSGTYPE_RetrieveInodesResp              7004
#define NETMSGTYPE_RetrieveChunks                  7005
#define NETMSGTYPE_RetrieveChunksResp              7006
#define NETMSGTYPE_RetrieveFsIDs                   7007
#define NETMSGTYPE_RetrieveFsIDsResp               7008
#define NETMSGTYPE_DeleteDirEntries                7009
#define NETMSGTYPE_DeleteDirEntriesResp            7010
#define NETMSGTYPE_CreateDefDirInodes              7011
#define NETMSGTYPE_CreateDefDirInodesResp          7012
#define NETMSGTYPE_FixInodeOwnersInDentry          7013
#define NETMSGTYPE_FixInodeOwnersInDentryResp      7014
#define NETMSGTYPE_FixInodeOwners                  7015
#define NETMSGTYPE_FixInodeOwnersResp              7016
#define NETMSGTYPE_LinkToLostAndFound              7017
#define NETMSGTYPE_LinkToLostAndFoundResp          7018
#define NETMSGTYPE_DeleteChunks                    7019
#define NETMSGTYPE_DeleteChunksResp                7020
#define NETMSGTYPE_CreateEmptyContDirs             7021
#define NETMSGTYPE_CreateEmptyContDirsResp         7022
#define NETMSGTYPE_UpdateFileAttribs               7023
#define NETMSGTYPE_UpdateFileAttribsResp           7024
#define NETMSGTYPE_UpdateDirAttribs                7025
#define NETMSGTYPE_UpdateDirAttribsResp            7026
#define NETMSGTYPE_RemoveInodes                    7027
#define NETMSGTYPE_RemoveInodesResp                7028
#define NETMSGTYPE_RecreateFsIDs                   7031
#define NETMSGTYPE_RecreateFsIDsResp               7032
#define NETMSGTYPE_RecreateDentries                7033
#define NETMSGTYPE_RecreateDentriesResp            7034
#define NETMSGTYPE_FsckModificationEvent           7035
#define NETMSGTYPE_FsckSetEventLogging             7036
#define NETMSGTYPE_FsckSetEventLoggingResp         7037
#define NETMSGTYPE_FetchFsckChunkList              7038
#define NETMSGTYPE_FetchFsckChunkListResp          7039
#define NETMSGTYPE_AdjustChunkPermissions          7040
#define NETMSGTYPE_AdjustChunkPermissionsResp      7041
#define NETMSGTYPE_MoveChunkFile                   7042
#define NETMSGTYPE_MoveChunkFileResp               7043
#define NETMSGTYPE_CheckAndRepairDupInode          7044
#define NETMSGTYPE_CheckAndRepairDupInodeResp      7045

#endif /*NETMESSAGETYPES_H_*/
