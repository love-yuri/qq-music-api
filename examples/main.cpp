#include "utils/json_utils.hpp"

import curl;

struct Person {
  int age = 25;
  std::string name = "John";
  double height = 5.9;
};

int main() {
  const std::string input = R"({"age":21,"name":"Bob","height":6.1})";
  const auto person = read_json<Person>(input);
  yinfo << person.name << " is " << person.age << " years old";
  return 0;
}