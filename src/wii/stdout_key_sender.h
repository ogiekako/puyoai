#ifndef WII_STDOUT_KEY_SENDER_H_
#define WII_STDOUT_KEY_SENDER_H_

#include "base/base.h"
#include "core/key_set.h"
#include "wii/key_sender.h"

class StdoutKeySender : public KeySender {
public:
    virtual ~StdoutKeySender() {}

    void start() override {}
    void stop() override {}

    void sendWait(int ms) override;
    void sendKeySet(const KeySet&, bool forceSend) override;
    void sendKeySetSeq(const KeySetSeq&) override;
};

#endif
