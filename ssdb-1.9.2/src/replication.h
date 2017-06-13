//
// Created by zts on 17-4-27.
//

#ifndef SSDB_REPLICATION_H
#define SSDB_REPLICATION_H


#include <include.h>
#include <string>
#include <sstream>
#include <util/strings.h>
#include <util/thread.h>
#include <net/redis/redis_client.h>
#include <common/context.hpp>
#include <net/worker.h>
#include <thread>
#include <future>

const uint64_t MAX_PACKAGE_SIZE = 8 * 1024 * 1024;
const uint64_t MIN_PACKAGE_SIZE = 1024 * 1024;

//#define REPLIC_NO_COMPRESS TRUE

void *ssdb_sync(void *arg);
void *ssdb_sync2(void *arg);

int replic_decode_len(const char *data, int *offset, uint64_t *lenptr);
std::string replic_save_len(uint64_t len);



class CompressResult {
public:
    Buffer* in = nullptr;
    Buffer* out = nullptr;
    size_t comprlen = 0;
};



class ReplicationByIterator : public ReplicationJob {
public:
    int64_t ts;

    HostAndPort hnp;

    bool heartbeat = false;
    bool compress = true;

    volatile bool quit = false;

    ReplicationByIterator(const Context &ctx, const HostAndPort &hnp, Link *link, bool compress, bool heartbeat) :
            hnp(hnp), heartbeat(heartbeat), compress(compress) {
        ts = time_ms();

        ReplicationJob::ctx = ctx;
        ReplicationJob::upstream = link;
    }

    ~ReplicationByIterator() override = default;
    void reportError();
    int process() override;
};

class ReplicationByIterator2 : public ReplicationByIterator {

public:
    ReplicationByIterator2(const Context &ctx, const HostAndPort &hnp, Link *link, bool compress,
                                                   bool heartbeat) : ReplicationByIterator(ctx, hnp, link, compress,
                                                                                           heartbeat) {}

    ~ReplicationByIterator2() override = default;
    int process() override;

    std::future<CompressResult> bg;

};


#endif //SSDB_REPLICATION_H
