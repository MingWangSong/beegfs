#pragma once

#include <net/message/MirroredMessage.h>
#include <common/net/message/storage/attribs/SetFileStateMsg.h>
#include <common/net/message/storage/attribs/SetFileStateRespMsg.h>

class SetFileStateMsgEx : public MirroredMessage<SetFileStateMsg, FileIDLock>
{
   public:
      typedef ErrorCodeResponseState<SetFileStateRespMsg, NETMSGTYPE_SetFileState> ResponseState;

      virtual bool processIncoming(ResponseContext& ctx) override;

      FileIDLock lock(EntryLockStore& store) override;

      bool isMirrored() override { return getEntryInfo()->getIsBuddyMirrored(); }

   private:
      std::unique_ptr<MirroredMessageResponseState> executeLocally(ResponseContext& ctx,
         bool isSecondary) override;

      void forwardToSecondary(ResponseContext& ctx) override;

      FhgfsOpsErr processSecondaryResponse(NetMessage& resp) override
      {
         return (FhgfsOpsErr) static_cast<SetFileStateRespMsg&>(resp).getValue();
      }

      const char* mirrorLogContext() const override { return "SetFileStateMsgEx/forward"; }
};