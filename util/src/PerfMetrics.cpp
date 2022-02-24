//
// Created by hshomroni on 22/02/2022.
//

#include "PerfMetrics.hpp"

PerfMetric::PerfMetric(Component& component, string name, uint64_t num_map_entries_for_reset, bool isThreadSafe)
    : num_map_entries_for_reset_(num_map_entries_for_reset),
      is_thread_safe_(isThreadSafe),
      name_(name),
      avg_{component.RegisterGauge(name + "Avg", 0)},
      variance_{component.RegisterGauge(name + "Variance", 0)} {
  LOG_FATAL(KEY_EX_LOG, "new Hanan Perf Metric" << name_);
}

void PerfMetric::addStartTimeStamp(bool isPrimary, string key) {
  if (!isPrimary) return;
  LOG_FATAL(KEY_EX_LOG,
            "Hanan Perf Metric adding start TS " << name_ << " info: num entries=" << entries_.size()
                                                 << ", avg=" << avg_.Get().Get());
  if (is_thread_safe_) {
    lock_guard<mutex> lock(mutex_);
    addStartTimeStampUnSafe(key);
  } else {
    addStartTimeStampUnSafe(key);
  }
  LOG_FATAL(KEY_EX_LOG,
            "Hanan Perf Metric added start TS " << name_ << " info: num entries=" << entries_.size()
                                                << ", avg=" << avg_.Get().Get());
}

void PerfMetric::addStartTimeStampUnSafe(string key) {
  if (entries_.count(key) == 0) entries_[key] = getMonotonicTime();
}

void PerfMetric::finishMeasurement(bool isPrimary, string key) {
  if (!isPrimary) return;

  if (is_thread_safe_) {
    lock_guard<mutex> lock(mutex_);
    finishMeasurementUnSafe(key);
  } else {
    finishMeasurementUnSafe(key);
  }
}

void PerfMetric::finishMeasurementUnSafe(string key) {
  LOG_FATAL(KEY_EX_LOG,
            "Hanan Perf Metric finishing measure " << name_ << " info: num entries=" << entries_.size()
                                                   << ", avg=" << avg_.Get().Get());
  if (entries_.count(key) != 0) {
    auto end2EndPreExeDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(getMonotonicTime() - entries_[key]).count();

    avg_and_variance_.add(static_cast<double>(end2EndPreExeDuration));

    avg_.Get().Set((uint64_t)avg_and_variance_.avg());
    variance_.Get().Set((uint64_t)avg_and_variance_.var());
    entries_.erase(key);
  }
  LOG_FATAL(KEY_EX_LOG,
            "Hanan Perf Metric Finished measure " << name_ << " info: num entries=" << entries_.size()
                                                  << ", avg=" << avg_.Get().Get());

  if (entries_.size() > num_map_entries_for_reset_) {
    reset();
  }
}

void PerfMetric::reset() {
  entries_.clear();
  avg_and_variance_.reset();
}