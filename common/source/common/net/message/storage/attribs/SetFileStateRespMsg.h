#pragma once

#include <common/net/message/SimpleIntMsg.h>
#include <common/storage/StorageErrors.h>

class SetFileStateRespMsg : public SimpleIntMsg
{
   public:
      SetFileStateRespMsg(int result) : SimpleIntMsg(NETMSGTYPE_SetFileStateResp, result)
      {
      }

      /**
       * For deserialization only
       */
      SetFileStateRespMsg() : SimpleIntMsg(NETMSGTYPE_SetFileStateResp)
      {
      }

      FhgfsOpsErr getResult() { return static_cast<FhgfsOpsErr>(getValue()); }
};