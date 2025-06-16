#include <program/Program.h>
#include <storage/MetaStore.h>

#include "SessionFile.h"

bool SessionFile::operator==(const SessionFile& other) const
{
   return accessFlags == other.accessFlags
      && sessionID == other.sessionID
      && entryInfo == other.entryInfo
      && useAsyncCleanup == other.useAsyncCleanup;
}

bool SessionFile::relinkInode(MetaStore& store)
{
   // Bypass access checks during session recovery to ensure locked files in disposal directory
   // can be properly recovered after system crashes or unclean shutdowns. This preserves session
   // continuity regardless of file state locks. We may revisit this approach if anything changes.
   auto openRes = store.openFile(&entryInfo, accessFlags, /* bypassAccessCheck */ true, inode, true);

   if (openRes == FhgfsOpsErr_SUCCESS)
      return true;

   LOG(SESSIONS, ERR, "Could not relink session for inode.",
         ("inode", entryInfo.getParentEntryID() + "/" + entryInfo.getEntryID()));
   return false;
}
