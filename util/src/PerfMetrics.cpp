//
// Created by hshomroni on 22/02/2022.
//

#include "PerfMetrics.hpp"

PerfMetric::PerfMetric(Component& component, string name, uint64_t num_map_entries_for_reset)
    : num_map_entries_for_reset_(num_map_entries_for_reset),
      name_(name),
      avg_{component.RegisterGauge(name + "Avg", 0)},
      variance_{component.RegisterGauge(name + "Variance", 0)} {}

void PerfMetric::addStartTimeStamp(bool isPrimary, string key) {
  if (!isPrimary) return;

  if (entries_.count(key) == 0) entries_[key] = getMonotonicTime();
}

void PerfMetric::finishMeasurement(bool isPrimary, string key) {
  if (!isPrimary) return;

  if (entries_.count(key) != 0) {
    auto end2EndPreExeDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(getMonotonicTime() - entries_[key]).count();

    avg_and_variance_.add(static_cast<double>(end2EndPreExeDuration));

    avg_.Get().Set((uint64_t)avg_and_variance_.avg());
    variance_.Get().Set((uint64_t)avg_and_variance_.var());
    entries_.erase(key);
  }

  if (entries_.size() > num_map_entries_for_reset_) {
    reset();
  }
}

void PerfMetric::reset() {
  entries_.clear();
  avg_and_variance_.reset();
}