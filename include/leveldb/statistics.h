// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_STATISTICS_H_
#define STORAGE_LEVELDB_INCLUDE_STATISTICS_H_

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>

namespace leveldb {

/**
 * Keep adding ticker's here.
 * Any ticker should have a value less than TICKER_ENUM_MAX.
 * Add a new ticker by assigning it the current value of TICKER_ENUM_MAX
 * Add a string representation in TickersNameMap below.
 * And incrementing TICKER_ENUM_MAX.
 */
enum Tickers {
  BLOCK_CACHE_MISS = 0,
  BLOCK_CACHE_HIT = 1,
  BLOOM_FILTER_USEFUL = 2, // no. of times bloom filter has avoided file reads.
  /**
   * COMPACTION_KEY_DROP_* count the reasons for key drop during compaction
   * There are 3 reasons currently.
   */
  COMPACTION_KEY_DROP_NEWER_ENTRY = 3, // key was written with a newer value.
  COMPACTION_KEY_DROP_OBSOLETE = 4, // The key is obsolete.
  COMPACTION_KEY_DROP_USER = 5, // user compaction function has dropped the key.
  // Number of keys written to the database via the Put and Write call's
  NUMBER_KEYS_WRITTEN = 6,
  // Number of Keys read,
  NUMBER_KEYS_READ = 7,
  // Bytes written / read
  BYTES_WRITTEN = 8,
  BYTES_READ = 9,
  NO_FILE_CLOSES = 10,
  NO_FILE_OPENS = 11,
  NO_FILE_ERRORS = 12,
  // Time system had to wait to do LO-L1 compactions
  STALL_L0_SLOWDOWN_MICROS = 13,
  // Time system had to wait to move memtable to L1.
  STALL_MEMTABLE_COMPACTION_MICROS = 14,
  // write throttle because of too many files in L0
  STALL_L0_NUM_FILES_MICROS = 15,
  RATE_LIMIT_DELAY_MILLIS = 16,

  NO_ITERATORS = 17, // number of iterators currently open

  // Number of MultiGet calls, keys read, and bytes read
  NUMBER_MULTIGET_CALLS = 18,
  NUMBER_MULTIGET_KEYS_READ = 19,
  NUMBER_MULTIGET_BYTES_READ = 20,

  TICKER_ENUM_MAX = 21
};

const std::vector<std::pair<Tickers, std::string>> TickersNameMap = {
  std::make_pair(BLOCK_CACHE_MISS, "levledb.block.cache.miss"),
  std::make_pair(BLOCK_CACHE_HIT, "rocksdb.block.cache.hit"),
  std::make_pair(BLOOM_FILTER_USEFUL, "rocksdb.bloom.filter.useful"),
  std::make_pair(COMPACTION_KEY_DROP_NEWER_ENTRY, "rocksdb.compaction.key.drop.new"),
  std::make_pair(COMPACTION_KEY_DROP_OBSOLETE, "rocksdb.compaction.key.drop.obsolete"),
  std::make_pair(COMPACTION_KEY_DROP_USER, "rocksdb.compaction.key.drop.user"),
  std::make_pair(NUMBER_KEYS_WRITTEN, "rocksdb.number.keys.written"),
  std::make_pair(NUMBER_KEYS_READ, "rocksdb.number.keys.read"),
  std::make_pair(BYTES_WRITTEN, "rocksdb.bytes.written"),
  std::make_pair(BYTES_READ, "rocksdb.bytes.read"),
  std::make_pair(NO_FILE_CLOSES, "rocksdb.no.file.closes"),
  std::make_pair(NO_FILE_OPENS, "rocksdb.no.file.opens"),
  std::make_pair(NO_FILE_ERRORS, "rocksdb.no.file.errors"),
  std::make_pair(STALL_L0_SLOWDOWN_MICROS, "rocksdb.l0.slowdown.micros"),
  std::make_pair(STALL_MEMTABLE_COMPACTION_MICROS, "rocksdb.memtable.compaction.micros"),
  std::make_pair(STALL_L0_NUM_FILES_MICROS, "rocksdb.l0.num.files.stall.micros"),
  std::make_pair(RATE_LIMIT_DELAY_MILLIS, "rocksdb.rate.limit.dleay.millis"),
  std::make_pair(NO_ITERATORS, "rocksdb.num.iterators"),
  std::make_pair(NUMBER_MULTIGET_CALLS, "rocksdb.number.multiget.get"),
  std::make_pair(NUMBER_MULTIGET_KEYS_READ, "rocksdb.number.multiget.keys.read"),
  std::make_pair(NUMBER_MULTIGET_BYTES_READ, "rocksdb.number.multiget.bytes.read")
};

/**
 * Keep adding histogram's here.
 * Any histogram whould have value less than HISTOGRAM_ENUM_MAX
 * Add a new Histogram by assigning it the current value of HISTOGRAM_ENUM_MAX
 * Add a string representation in HistogramsNameMap below
 * And increment HISTOGRAM_ENUM_MAX
 */
enum Histograms {
  DB_GET = 0,
  DB_WRITE = 1,
  COMPACTION_TIME = 2,
  TABLE_SYNC_MICROS = 3,
  COMPACTION_OUTFILE_SYNC_MICROS = 4,
  WAL_FILE_SYNC_MICROS = 5,
  MANIFEST_FILE_SYNC_MICROS = 6,
  // TIME SPENT IN IO DURING TABLE OPEN
  TABLE_OPEN_IO_MICROS = 7,
  DB_MULTIGET = 8,
  HISTOGRAM_ENUM_MAX = 9
};

const std::vector<std::pair<Histograms, std::string>> HistogramsNameMap = {
  std::make_pair(DB_GET, "rocksdb.db.get.micros"),
  std::make_pair(DB_WRITE, "rocksdb.db.write.micros"),
  std::make_pair(COMPACTION_TIME, "rocksdb.compaction.times.micros"),
  std::make_pair(TABLE_SYNC_MICROS, "rocksdb.table.sync.micros"),
  std::make_pair(COMPACTION_OUTFILE_SYNC_MICROS, "rocksdb.compaction.outfile.sync.micros"),
  std::make_pair(WAL_FILE_SYNC_MICROS, "rocksdb.wal.file.sync.micros"),
  std::make_pair(MANIFEST_FILE_SYNC_MICROS, "rocksdb.manifest.file.sync.micros"),
  std::make_pair(TABLE_OPEN_IO_MICROS, "rocksdb.table.open.io.micros"),
  std::make_pair(DB_MULTIGET, "rocksdb.db.multiget.micros")
};

struct HistogramData {
  double median;
  double percentile95;
  double percentile99;
  double average;
  double standard_deviation;
};


class Histogram {
 public:
  // clear's the histogram
  virtual void Clear() = 0;
  virtual ~Histogram();
  // Add a value to be recorded in the histogram.
  virtual void Add(uint64_t value) = 0;

  virtual std::string ToString() const = 0;

  // Get statistics
  virtual double Median() const = 0;
  virtual double Percentile(double p) const = 0;
  virtual double Average() const = 0;
  virtual double StandardDeviation() const = 0;
  virtual void Data(HistogramData * const data) const = 0;

};

/**
 * A dumb ticker which keeps incrementing through its life time.
 * Not thread safe. Locking is currently managed by external leveldb lock
 */
class Ticker {
 public:
  Ticker() : count_(0) { }

 inline void recordTick() {
    count_++;
  }

  inline void recordTick(int count) {
    count_ += count;
  }

  inline uint64_t getCount() {
    return count_;
  }

 private:
  std::atomic_uint_fast64_t count_;
};

// Analyze the performance of a db
class Statistics {
 public:

  virtual long getTickerCount(Tickers tickerType) = 0;
  virtual void recordTick(Tickers tickerType, uint64_t count = 0) = 0;
  virtual void measureTime(Histograms histogramType, uint64_t time) = 0;

  virtual void histogramData(Histograms type, HistogramData * const data) = 0;

};

// Create a concrete DBStatistics object
std::shared_ptr<Statistics> CreateDBStatistics();

// Ease of Use functions
inline void RecordTick(std::shared_ptr<Statistics> statistics,
                       Tickers ticker,
                       uint64_t count = 1) {
  assert(HistogramsNameMap.size() == HISTOGRAM_ENUM_MAX - 1);
  assert(TickersNameMap.size() == TICKER_ENUM_MAX -1);
  if (statistics) {
    statistics->recordTick(ticker, count);
  }
}
}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_STATISTICS_H_
