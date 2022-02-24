// Concord
//
// Copyright (c) 2019-2022 VMware, Inc. All Rights Reserved.
//
// This product is licensed to you under the Apache 2.0 license (the "License").  You may not use this product except in
// compliance with the Apache 2.0 License.
//
// This product may include a number of subcomponents with separate copyright notices and license terms. Your use of
// these subcomponents is subject to the terms and conditions of the sub-component's license, as noted in the LICENSE
// file.

#pragma once

#include <unordered_map>
#include <string>
#include <chrono>
#include <variant>
#include <mutex>

#include "RollingAvgAndVar.hpp"
#include "Metrics.hpp"
#include "Logger.hpp"

#define getMonotonicTime std::chrono::steady_clock::now

using namespace std;
using concordMetrics::GaugeHandle;
using concordMetrics::AtomicGaugeHandle;
using concordMetrics::Component;

class PerfMetric {
 public:
  PerfMetric(Component& component, string name, uint64_t num_map_entries_for_reset, bool isThreadSafe);

  void addStartTimeStamp(bool isPrimary, string key);

  void finishMeasurement(bool isPrimary, string key);

  void reset();

  // void printMe() {
  //  LOG_FATAL(KEY_EX_LOG,
  //           "Hanan Perf Metric " << name_ << " info: num entries=" << entries_.size() << ", avg=" <<
  //           avg_.Get().Get());
  // }

 private:
  void addStartTimeStampUnSafe(string key);
  void finishMeasurementUnSafe(string key);

  mutex mutex_;
  uint64_t num_map_entries_for_reset_;
  bool is_thread_safe_;
  string name_;
  bftEngine::impl::RollingAvgAndVar avg_and_variance_;
  unordered_map<string, chrono::time_point<std::chrono::steady_clock>> entries_;
  GaugeHandle avg_;
  GaugeHandle variance_;
};