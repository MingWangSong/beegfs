#pragma once

#include <common/net/message/NetMessage.h>
#include <common/storage/EntryInfo.h>

class SetFileStateMsg : public MirroredMessageBase<SetFileStateMsg>
{
   public:
      /**
       * @param entryInfo just a reference, so do not free it as long as you use this object!
       * @param state the new state (accessFlags+dataState) of the file
       */
      SetFileStateMsg(EntryInfo* entryInfo, uint8_t state) :
         BaseType(NETMSGTYPE_SetFileState)
      {
         this->entryInfoPtr = entryInfo;
         this->state = state;
      }

      SetFileStateMsg() : BaseType(NETMSGTYPE_SetFileState) {}

      template<typename This, typename Ctx>
      static void serialize(This obj, Ctx& ctx)
      {
         ctx
            % serdes::backedPtr(obj->entryInfoPtr, obj->entryInfo)
            % obj->state;
      }

   private:
      // for serialization
      EntryInfo* entryInfoPtr;
      uint8_t state;

      // for deserialization
      EntryInfo entryInfo;

   public:
      EntryInfo* getEntryInfo()
      {
         return &this->entryInfo;
      }

      uint8_t getFileState()
      {
         return this->state;
      }

      bool supportsMirroring() const { return true; }
};