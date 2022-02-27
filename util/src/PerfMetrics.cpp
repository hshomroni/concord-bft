//
// Created by hshomroni on 22/02/2022.
//

#include "PerfMetrics.hpp"

template <typename T>
PerfMetric<T>::PerfMetric(Component& component, string name, uint64_t num_map_entries_for_reset, bool isThreadSafe)
    : num_map_entries_for_reset_(num_map_entries_for_reset),
      is_thread_safe_(isThreadSafe),
      name_(name),
      avg_{component.RegisterGauge(name + "Avg", 0)},
      variance_{component.RegisterGauge(name + "Variance", 0)} {}

template <typename T>
void PerfMetric<T>::addStartTimeStamp(const T& key) {
  if (is_thread_safe_) {
    lock_guard<mutex> lock(mutex_);
    addStartTimeStampUnSafe(key);
  } else {
    addStartTimeStampUnSafe(key);
  }
}

template <typename T>
void PerfMetric<T>::addStartTimeStampUnSafe(const T& key) {
  if (entries_.count(key) == 0) {
    entries_[key] = getMonotonicTime();
  }
}

template <typename T>
void PerfMetric<T>::deleteSingleEntry(const T& key) {
  if (is_thread_safe_) {
    lock_guard<mutex> lock(mutex_);
    deleteSingleEntryUnSafe(key);
  } else {
    deleteSingleEntryUnSafe(key);
  }
}

template <typename T>
void PerfMetric<T>::deleteSingleEntryUnSafe(const T& key) {
  if (entries_.count(key) != 0) {
    entries_.erase(key);
  }
}

template <typename T>
void PerfMetric<T>::finishMeasurement(const T& key) {
  if (is_thread_safe_) {
    lock_guard<mutex> lock(mutex_);
    finishMeasurementUnSafe(key);
  } else {
    finishMeasurementUnSafe(key);
  }
}

template <typename T>
void PerfMetric<T>::finishMeasurementUnSafe(const T& key) {
  if (entries_.count(key) != 0) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(getMonotonicTime() - entries_[key]).count();

    avg_and_variance_.add(static_cast<double>(duration));

    avg_.Get().Set(static_cast<uint64_t>(avg_and_variance_.avg()));
    variance_.Get().Set(static_cast<uint64_t>(avg_and_variance_.var()));
    entries_.erase(key);
  }

  if (entries_.size() > num_map_entries_for_reset_) {
    reset();
  }
}

template <typename T>
void PerfMetric<T>::reset() {
  if (is_thread_safe_) {
    lock_guard<mutex> lock(mutex_);
    entries_.clear();
  } else {
    entries_.clear();
  }
}