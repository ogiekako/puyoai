#ifndef WII_SERIAL_KEY_SENDER_H_
#define WII_SERIAL_KEY_SENDER_H_

#include <termios.h>
#include <string>
#include <thread>

#include "base/base.h"
#include "base/blocking_queue.h"
#include "wii/key_sender.h"

class SerialKeySender : public KeySender {
public:
    explicit SerialKeySender(const std::string& deviceName);
    ~SerialKeySender() override;

    void start() override;
    void stop() override;

    void sendWait(int ms) override;
    void sendKeySet(const KeySet&, bool forceSend) override;
    void sendKeySetSeq(const KeySetSeq&) override;

private:
    void sendKeySetInternal(const KeySet&);

    void runLoop();

    int fd_;
    struct termios original_;
    KeySet lastSent_;

    std::thread th_;
    std::atomic<bool> should_stop_;
    base::InfiniteBlockingQueue<unsigned char> queue_;
};

#endif
