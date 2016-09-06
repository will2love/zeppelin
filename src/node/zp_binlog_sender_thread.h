#ifndef ZP_BINLOG_SENDER_THREAD_H
#define ZP_BINLOG_SENDER_THREAD_H

#include "zp_pb_cli.h"

#include "pink_thread.h"
#include "slice.h"
#include "status.h"

#include "env.h"
#include "slash_mutex.h"
#include "zp_data_partition.h"


class ZPBinlogSenderThread : public pink::Thread {
 public:

  ZPBinlogSenderThread(Partition *partition, slash::SequentialFile *queue, uint32_t filenum, uint64_t con_offset);

  virtual ~ZPBinlogSenderThread();

  // Get and Set
  uint64_t last_record_offset () {
    slash::RWLock l(&rwlock_, false);
    return last_record_offset_;
  }
  uint32_t filenum() {
    slash::RWLock l(&rwlock_, false);
    return filenum_;
  }
  uint64_t con_offset() {
    slash::RWLock l(&rwlock_, false);
    return con_offset_;
  }

  int trim();
  uint64_t get_next(bool &is_error);

 private:

  slash::Status Parse(std::string &scratch);
  slash::Status Consume(std::string &scratch);
  unsigned int ReadPhysicalRecord(slash::Slice *fragment);

  Partition *partition_;
  uint64_t con_offset_;
  uint32_t filenum_;

  uint64_t initial_offset_;
  uint64_t last_record_offset_;
  uint64_t end_of_buffer_offset_;

  slash::SequentialFile* queue_;
  char* const backing_store_;
  slash::Slice buffer_;

  //TODO add rwlock
  pthread_rwlock_t rwlock_;

  // TODO Cli

  virtual void* ThreadMain();
  uint32_t ParseMsgCode(std::string* scratch);
  void SendToPeers(const std::string &data);
};

#endif
