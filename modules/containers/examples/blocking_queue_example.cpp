//=================================================================================================
// Copyright (C) 2023-2024 EOLO Contributors
//=================================================================================================

#include <cstdlib>
#include <string>

#include <fmt/core.h>

#include "eolo/containers/blocking_queue.h"

class String {
public:
  String() {
    fmt::println("String: default constructor");
  }

  ~String() {
    fmt::println("String: destructor");
  }

  explicit String(std::string str) : str_(std::move(str)) {
    fmt::println("String: string constructor");
  }

  String(const String& other) : str_(other.str_) {
    fmt::println("String: copy constructor");
  }

  String(String&& other) noexcept : str_(std::move(other.str_)) {
    fmt::println("String: move constructor");
  }

  auto operator=(const String& other) -> String& {
    if (&other != this) {
      str_ = other.str_;
    }
    fmt::println("String: copy assignment operator");
    return *this;
  }

  auto operator=(String&& other) noexcept -> String& {
    if (&other != this) {
      str_ = std::move(other.str_);
    }
    fmt::println("String: move assignment operator");
    return *this;
  }

private:
  std::string str_;
};

/// Shows difference between tryPush and tryEmplace methods of BlockingQueue class.
/// The same logic applies to force* methods.
void pushVsEmplace() {
  // push methods require extra moving (or copying if move semantics is not supported).
  {
    fmt::print("=== Use push to add new element into the queue\n");
    eolo::containers::BlockingQueue<std::pair<String, String>> queue{ {} };
    if (!queue.tryPush({ String{ "1" }, String{ "2" } })) {
      return;
    }
  }

  // Using emplace helps to reduce the number a move constructor is called.
  {
    fmt::println("=== Use emplace to add new element into the queue");
    eolo::containers::BlockingQueue<std::pair<String, String>> queue{ {} };
    if (!queue.tryEmplace(String{ "1" }, String{ "2" })) {
      return;
    }
  }
}

auto main() -> int {
  try {
    pushVsEmplace();
  } catch (const std::exception& ex) {
    fmt::println("{}", ex.what());
    return 1;
  }

  return 0;
}
