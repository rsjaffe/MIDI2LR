#include "Utilities.h"

std::string RSJ::trim(const std::string& str, const std::string& what) {
  auto front = str.find_first_not_of(what);
  if (front == std::string::npos)
    return std::string();
  auto back = str.find_last_not_of(what);
  return str.substr(front, back - front + 1);
}

std::string RSJ::ltrim(const std::string& str, const std::string& what) {
  auto front = str.find_first_not_of(what);
  if (front == std::string::npos)
    return std::string();
  return str.substr(front);
}

std::string RSJ::rtrim(const std::string& str, const std::string& what) {
  auto back = str.find_last_not_of(what);
  if (back == std::string::npos)
    return std::string();
  return str.substr(0, back + 1);
}
