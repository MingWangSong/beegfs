#include <common/net/message/control/GenericResponseMsg.h>
#include <common/net/message/session/opening/OpenFileRespMsg.h>
#include <common/toolkit/SessionTk.h>
#include <common/storage/striping/Raid0Pattern.h>
#include <components/FileEventLogger.h>
#include <net/msghelpers/MsgHelperOpen.h>
#include <program/Program.h>
#include <session/EntryLock.h>
#include <session/SessionStore.h>
#include "OpenFileMsgEx.h"

FileIDLock OpenFileMsgEx::lock(EntryLockStore& store)
{
   return {&store, getEntryInfo()->getEntryID(), true};
}

bool OpenFileMsgEx::processIncoming(ResponseContext& ctx)
{
#ifdef BEEGFS_DEBUG
   const char* logContext = "OpenFileMsg incoming";

   EntryInfo* entryInfo = getEntryInfo();

   LOG_DEBUG(logContext, Log_SPAM, "ParentInfo: " + entryInfo->getParentEntryID() +
      " EntryID: " + entryInfo->getEntryID() + " FileName: " + entryInfo->getFileName() );

   LOG_DEBUG(logContext, Log_DEBUG,
      "BuddyMirrored: " + std::string(entryInfo->getIsBuddyMirrored() ? "Yes" : "No") +
      " Secondary: " +
      std::string(hasFlag(NetMessageHeader::Flag_BuddyMirrorSecond) ? "Yes" : "No") );
#endif // BEEGFS_DEBUG

   return BaseType::processIncoming(ctx);
}

std::unique_ptr<MirroredMessageResponseState> OpenFileMsgEx::executeLocally(ResponseContext& ctx,
   bool isSecondary)
{
   App* app = Program::getApp();

   EntryInfo* entryInfo = this->getEntryInfo();
   bool useQuota = isMsgHeaderFeatureFlagSet(OPENFILEMSG_FLAG_USE_QUOTA);
   bool bypassAccessCheck = isMsgHeaderFeatureFlagSet(OPENFILEMSG_FLAG_BYPASS_ACCESS_CHECK);

   const bool eventLoggingEnabled = !isSecondary && app->getFileEventLogger() && getFileEvent();
   MetaFileHandle inode;

   PathInfo pathInfo;
   StripePattern* pattern;
   unsigned sessionFileID;
   SessionStore* sessions = entryInfo->getIsBuddyMirrored()
      ? app->getMirroredSessions()
      : app->getSessions();

   FhgfsOpsErr openRes = MsgHelperOpen::openFile(
      entryInfo, getAccessFlags(), useQuota, bypassAccessCheck, getMsgHeaderUserID(),
      inode, isSecondary);

   if (openRes == FhgfsOpsErr_SUCCESS && shouldFixTimestamps())
      fixInodeTimestamp(*inode, fileTimestamps, entryInfo);

   // update operation counters
   updateNodeOp(ctx, MetaOpCounter_OPEN);

   if (openRes != FhgfsOpsErr_SUCCESS)
   {
      // If open() fails due to file state restrictions, emit an OPEN_BLOCKED event
      // (when event logging is enabled). The numHardlinks in event context will be 0
      // since inode is nullptr after access denial.
      if ((openRes == FhgfsOpsErr_FILEACCESS_DENIED) && eventLoggingEnabled)
      {
         FileEvent* fileEvent = const_cast<FileEvent*>(getFileEvent());
         fileEvent->type = FileEventType::OPEN_BLOCKED;

         EventContext eventCtx = makeEventContext(
            entryInfo,
            entryInfo->getParentEntryID(),
            getMsgHeaderUserID(),
            "",
            inode ? inode->getNumHardlinks() : 0,
            isSecondary
         );
         logEvent(app->getFileEventLogger(), *fileEvent, eventCtx);
      }

      // error occurred
      Raid0Pattern dummyPattern(1, UInt16Vector{});

      // generate response
      if(unlikely(openRes == FhgfsOpsErr_COMMUNICATION))
      {
         return boost::make_unique<OpenFileResponseState>();
      }
      else
      { // normal response
         return boost::make_unique<OpenFileResponseState>(openRes, std::string(), dummyPattern,
               pathInfo, 0);
      }
   }

   if (eventLoggingEnabled)
   {
      EventContext eventCtx = makeEventContext(
         entryInfo,
         entryInfo->getParentEntryID(),
         getMsgHeaderUserID(),
         "",
         inode->getNumHardlinks(),
         isSecondary
      );

      logEvent(app->getFileEventLogger(), *getFileEvent(), eventCtx);
   }

   // success => insert session
   SessionFile* sessionFile = new SessionFile(std::move(inode), getAccessFlags(), entryInfo);

   Session* session = sessions->referenceSession(getClientNumID(), true);

   pattern = sessionFile->getInode()->getStripePattern();

   if (!isSecondary)
   {
      sessionFileID = session->getFiles()->addSession(sessionFile);
      fileHandleID = SessionTk::generateFileHandleID(sessionFileID, entryInfo->getEntryID() );

      setSessionFileID(sessionFileID);
      setFileHandleID(fileHandleID.c_str());
   }
   else
   {
      fileHandleID = getFileHandleID();
      sessionFileID = getSessionFileID();

      bool addRes = session->getFiles()->addSession(sessionFile, sessionFileID);

      if (!addRes)
      {
         const char* logContext = "OpenFileMsgEx (executeLocally)";
         LogContext(logContext).log(Log_NOTICE,
            "Couldn't add sessionFile on secondary buddy; sessionID: " + getClientNumID().str()
               + "; sessionFileID: " + StringTk::uintToStr(sessionFileID));
      }
   }

   sessions->releaseSession(session);

   sessionFile->getInode()->getPathInfo(&pathInfo);

   return boost::make_unique<OpenFileResponseState>(openRes, fileHandleID, *pattern, pathInfo,
         sessionFile->getInode()->getFileVersion());
}

void OpenFileMsgEx::forwardToSecondary(ResponseContext& ctx)
{
   sendToSecondary(ctx, *this, NETMSGTYPE_OpenFileResp);
}
