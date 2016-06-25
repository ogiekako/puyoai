#ifndef WII_NULL_KEY_SENDER_H_
#define WII_NULL_KEY_SENDER_H_

#include "base/base.h"
#include "core/key_set.h"
#include "wii/key_sender.h"

class NullKeySender : public KeySender {
public:
    ~NullKeySender() override {}

    void start() override {}
    void stop() override {}

    void sendWait(int /*ms*/) override {}
    void sendKeySet(const KeySet&, bool /*forceSend*/) override {}
    void sendKeySetSeq(const KeySetSeq&) override {}
};

#endif
