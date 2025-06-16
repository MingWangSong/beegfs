#include <session/EntryLock.h>
#include "SetFileStateMsgEx.h"

bool SetFileStateMsgEx::processIncoming(ResponseContext& ctx)
{
   #ifdef BEEGFS_DEBUG
      const char* logContext = "SetFileStateMsgEx incoming";

      EntryInfo* entryInfo = this->getEntryInfo();
      LOG_DEBUG(logContext, 5, "EntryID: " + entryInfo->getEntryID() +
         "; FileName: " + entryInfo->getFileName() +
         "; EntryType: " + StringTk::intToStr(entryInfo->getEntryType()) +
         "; isBuddyMirrored: " + StringTk::intToStr(entryInfo->getIsBuddyMirrored()) +
         "; file state (accessFlags+dataState): " + StringTk::intToStr(this->getFileState()));
   #endif

   BaseType::processIncoming(ctx);
   return true;
}

FileIDLock SetFileStateMsgEx::lock(EntryLockStore& store)
{
   return {&store, getEntryInfo()->getEntryID(), true};
}

std::unique_ptr<MirroredMessageResponseState> SetFileStateMsgEx::executeLocally(
   ResponseContext& ctx, bool isSecondary)
{
   const char* logContext = "Set File State";
   FhgfsOpsErr res = FhgfsOpsErr_INTERNAL;

   MetaStore* metaStore = Program::getApp()->getMetaStore();
   res = metaStore->setFileState(getEntryInfo(), FileState(getFileState()));

   if (res != FhgfsOpsErr_SUCCESS)
   {
      LogContext(logContext).log(Log_DEBUG, "Setting file state failed. EntryID: " +
         getEntryInfo()->getEntryID());
      return boost::make_unique<ResponseState>(res);
   }

   return boost::make_unique<ResponseState>(res);
}

void SetFileStateMsgEx::forwardToSecondary(ResponseContext& ctx)
{
   sendToSecondary(ctx, *this, NETMSGTYPE_SetFileStateResp);
}