#include "dap.hpp"

#include <fmt/format.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
  dap::Source source;
  source.name = "Hello World";
  source.presentationHint = dap::SourcePresentationHint::normal;
  fmt::print("{}\n", json(source).dump());
}
