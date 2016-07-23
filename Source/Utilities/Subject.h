//
// Copyright (c) 2013 Juan Palacios juan.palacios.puyana@gmail.com
// Subject to the BSD 2-Clause License
// - see < http://opensource.org/licenses/BSD-2-Clause>
//

#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <functional>
#include <map>
#include <vector>
#include <utility> // for std::forward

template <typename Event>
class Subject
{
 public:
  Subject()=default;
  template <typename Observer>
  void registerObserver(const Event& event, Observer&& observer)
  {
    observers_[event].push_back(std::forward<Observer>(observer));
  }
  template <typename Observer>
  void registerObserver(Event&& event, Observer&& observer)
  {
    observers_[std::move(event)].push_back(std::forward<Observer>(observer));
  }

  void notify(const Event& event) const
  {
    for (const auto& obs : observers_.at(event)) obs();
  }
  // disallow copying and assigning
  Subject(const Subject&)=delete;
  Subject& operator=(const Subject&)=delete;

 private:
  std::map<Event, std::vector<std::function<void()>>> observers_;
};

#endif // SUBJECT_H_
