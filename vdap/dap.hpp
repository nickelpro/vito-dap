#ifndef VITO_DAP_HPP
#define VITO_DAP_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <nlohmann/json.hpp>

namespace nlohmann {
template <> struct adl_serializer<std::variant<std::int64_t, std::string>> {
  static void to_json(json& j,
      const std::variant<std::int64_t, std::string>& p) {
    if(std::holds_alternative<std::int64_t>(p))
      j = json(std::get<std::int64_t>(p));
    else
      j = json(std::get<std::string>(p));
  }
  static void from_json(const json& j,
      std::variant<std::int64_t, std::string>& p) {
    if(j.is_number())
      p = j.get<std::int64_t>();
    else
      p = j.get<std::string>();
  }
};

template <>
struct adl_serializer<std::map<std::string, std::optional<std::string>>> {
  static void to_json(json& j,
      const std::map<std::string, std::optional<std::string>>& p) {
    for(const auto& [key, value] : p) {
      if(value)
        j[key] = *value;
      else
        j[key] = nullptr;
    }
  }

  static void from_json(const json& j,
      std::map<std::string, std::optional<std::string>>& p) {
    for(const auto& [key, value] : j.items()) {
      if(value.is_null())
        p[key] = {};
      else
        p[key] = value.get<std::string>();
    }
  }
};
} // namespace nlohmann

namespace dap {

using json = nlohmann::json;

// Switch to string_view if:
//   https://github.com/nlohmann/json/pull/2685
// ever merges
template <typename T>
void to_optJson(json& j, const std::string& s, const std::optional<T>& p) {
  if(p)
    j[s] = *p;
}

template <typename T>
void from_optJson(const json& j, const std::string& s, std::optional<T>& p) {
  if(auto it {j.find(s)}; it != j.end())
    p = it->get<T>();
}

struct ExceptionBreakpointsFilter {
  std::string filter;
  std::string label;
  std::optional<std::string> description;
  std::optional<bool> default_;
  std::optional<bool> supportsCondition;
  std::optional<std::string> conditionDescription;
};

void to_json(json& j, const ExceptionBreakpointsFilter& p) {
  j["filter"] = p.filter;
  j["label"] = p.label;
  to_optJson(j, "description", p.description);
  to_optJson(j, "default", p.default_);
  to_optJson(j, "supportsCondition", p.supportsCondition);
  to_optJson(j, "conditionDescription", p.conditionDescription);
}

void from_json(const json& j, ExceptionBreakpointsFilter& p) {
  j.at("filter").get_to(p.filter);
  j.at("label").get_to(p.label);
  from_optJson(j, "description", p.description);
  from_optJson(j, "default", p.default_);
  from_optJson(j, "supportsCondition", p.supportsCondition);
  from_optJson(j, "conditionDescription", p.conditionDescription);
}

struct ExceptionFilterOptions {
  std::string filterId;
  std::optional<std::string> condition;
};

void to_json(json& j, const ExceptionFilterOptions& p) {
  j["filterId"] = p.filterId;
  to_optJson(j, "condition", p.condition);
}

void from_json(const json& j, ExceptionFilterOptions& p) {
  j.at("filterId").get_to(p.filterId);
  from_optJson(j, "condition", p.condition);
}

struct ExceptionPathSegment {
  std::optional<bool> negate;
  std::vector<std::string> name;
};

void to_json(json& j, const ExceptionPathSegment& p) {
  j["name"] = p.name;
  to_optJson(j, "negate", p.negate);
}

void from_json(const json& j, ExceptionPathSegment& p) {
  j.at("name").get_to(p.name);
  from_optJson(j, "negate", p.negate);
}

enum struct ExceptionBreakMode {
  never,
  always,
  unhandled,
  userUnhandled,
};

void to_json(json& j, const ExceptionBreakMode& p) {
  switch(p) {
    case ExceptionBreakMode::never:
      j = "never";
      break;
    case ExceptionBreakMode::always:
      j = "always";
      break;
    case ExceptionBreakMode::unhandled:
      j = "unhandled";
      break;
    case ExceptionBreakMode::userUnhandled:
      j = "userUnhandled";
      break;
    default:
      throw std::runtime_error {"Unknown ExceptionBreakMode"};
  };
}

void from_json(const json& j, ExceptionBreakMode& p) {
  std::string s {j.get<std::string>()};
  if(s == "never")
    p = ExceptionBreakMode::never;
  else if(s == "always")
    p = ExceptionBreakMode::always;
  else if(s == "unhandled")
    p = ExceptionBreakMode::unhandled;
  else if(s == "userUnhandled")
    p = ExceptionBreakMode::userUnhandled;
  else
    throw std::runtime_error {"Unknown ExceptionBreakMode"};
}


struct ExceptionOptions {
  std::optional<std::vector<ExceptionPathSegment>> path;
  ExceptionBreakMode breakMode;
};

void to_json(json& j, const ExceptionOptions& p) {
  j["breakMode"] = p.breakMode;
  to_optJson(j, "path", p.path);
}

void from_json(const json& j, ExceptionOptions& p) {
  j.at("breakMode").get_to(p.breakMode);
  from_optJson(j, "path", p.path);
}

struct ExceptionDetails {
  std::optional<std::string> message;
  std::optional<std::string> typeName;
  std::optional<std::string> fullTypeName;
  std::optional<std::string> evaluateName;
  std::optional<std::string> stackTrace;
  std::optional<std::vector<ExceptionDetails>> innerExceptions;
};

void to_json(json& j, const ExceptionDetails& p) {
  to_optJson(j, "message", p.message);
  to_optJson(j, "typeName", p.typeName);
  to_optJson(j, "fullTypeName", p.fullTypeName);
  to_optJson(j, "evaluateName", p.evaluateName);
  to_optJson(j, "stackTrace", p.stackTrace);
  to_optJson(j, "innerExceptions", p.innerExceptions);
}

void from_json(const json& j, ExceptionDetails& p) {
  from_optJson(j, "message", p.message);
  from_optJson(j, "typeName", p.typeName);
  from_optJson(j, "fullTypeName", p.fullTypeName);
  from_optJson(j, "evaluateName", p.evaluateName);
  from_optJson(j, "stackTrace", p.stackTrace);
  from_optJson(j, "innerExceptions", p.innerExceptions);
}

enum struct ColumnDescriptorType {
  string,
  number,
  boolean,
  unixTimestampUTC,
};

void to_json(json& j, const ColumnDescriptorType& p) {
  switch(p) {
    case ColumnDescriptorType::string:
      j = "string";
      break;
    case ColumnDescriptorType::number:
      j = "number";
      break;
    case ColumnDescriptorType::boolean:
      j = "boolean";
      break;
    case ColumnDescriptorType::unixTimestampUTC:
      j = "unixTimestampUTC";
      break;
    default:
      throw std::runtime_error {"Unknown ColumnDescriptorType"};
  };
}

void from_json(const json& j, ColumnDescriptorType& p) {
  std::string s {j.get<std::string>()};
  if(s == "string")
    p = ColumnDescriptorType::string;
  else if(s == "number")
    p = ColumnDescriptorType::number;
  else if(s == "boolean")
    p = ColumnDescriptorType::boolean;
  else if(s == "unixTimestampUTC")
    p = ColumnDescriptorType::unixTimestampUTC;
  else
    throw std::runtime_error {"Unknown ColumnDescriptorType"};
}

struct ColumnDescriptor {
  std::string attributeName;
  std::string label;
  std::optional<std::string> format;
  std::optional<ColumnDescriptorType> type;
  std::optional<std::int64_t> width;
};

void to_json(json& j, const ColumnDescriptor& p) {
  j["attributeName"] = p.attributeName;
  j["label"] = p.label;
  to_optJson(j, "format", p.format);
  to_optJson(j, "type", p.type);
  to_optJson(j, "width", p.width);
}

void from_json(const json& j, ColumnDescriptor& p) {
  j.at("attributeName").get_to(p.attributeName);
  j.at("label").get_to(p.label);
  from_optJson(j, "format", p.format);
  from_optJson(j, "type", p.type);
  from_optJson(j, "width", p.width);
}

enum struct CheckSumAlgorithm {
  MD5,
  SHA1,
  SHA256,
  timestamp,
};

void to_json(json& j, const CheckSumAlgorithm& p) {
  switch(p) {
    case CheckSumAlgorithm::MD5:
      j = "MD5";
      break;
    case CheckSumAlgorithm::SHA1:
      j = "SHA1";
      break;
    case CheckSumAlgorithm::SHA256:
      j = "SHA256";
      break;
    case CheckSumAlgorithm::timestamp:
      j = "timestamp";
      break;
    default:
      throw std::runtime_error {"Unknown CheckSumAlgorithm"};
  };
}

void from_json(const json& j, CheckSumAlgorithm& p) {
  std::string s {j.get<std::string>()};
  if(s == "MD5")
    p = CheckSumAlgorithm::MD5;
  else if(s == "SHA1")
    p = CheckSumAlgorithm::SHA1;
  else if(s == "SHA256")
    p = CheckSumAlgorithm::SHA256;
  else if(s == "timestamp")
    p = CheckSumAlgorithm::timestamp;
  else
    throw std::runtime_error {"Unknown CheckSumAlgorithm"};
}

struct Checksum {
  CheckSumAlgorithm algorithm;
  std::string checksum;
};

void to_json(json& j, const Checksum& p) {
  j["algorithm"] = p.algorithm;
  j["checksum"] = p.checksum;
}

void from_json(const json& j, Checksum& p) {
  j.at("algorithm").get_to(p.algorithm);
  j.at("checksum").get_to(p.checksum);
}

struct Capabilities {
  std::optional<bool> supportsConfigurationDoneRequest;
  std::optional<bool> supportsFunctionBreakpoints;
  std::optional<bool> supportsHitConditionalBreakpoints;
  std::optional<bool> supportsEvaluateForHovers;
  std::optional<std::vector<ExceptionBreakpointsFilter>>
      exceptionBreakpointFilters;
  std::optional<bool> supportsStepBack;
  std::optional<bool> supportsSetVariables;
  std::optional<bool> supportsRestartFrame;
  std::optional<bool> supportsGotoTargetsRequest;
  std::optional<bool> supportsStepInTargetsRequest;
  std::optional<bool> supportsCompletionsRequest;
  std::optional<std::vector<std::string>> completionTriggerCharacters;
  std::optional<bool> supportsModulesRequest;
  std::optional<std::vector<ColumnDescriptor>> additionalModuleColumns;
  std::optional<std::vector<CheckSumAlgorithm>> supportedChecksumAlgorithms;
  std::optional<bool> supportsRestartRequest;
  std::optional<bool> supportsExceptionOptions;
  std::optional<bool> supportsValueFormattingOptions;
  std::optional<bool> supportsExceptionInfoRequest;
  std::optional<bool> supportTerminateDebugee;
  std::optional<bool> supportSuspendDebugee;
  std::optional<bool> supportsDelayedStackTraceLoading;
  std::optional<bool> supportsLoadedSourcesRequest;
  std::optional<bool> supportsLogPoints;
  std::optional<bool> supportsTerminateThreadsRequest;
  std::optional<bool> supportsSetExpression;
  std::optional<bool> supportsTerminateRequest;
  std::optional<bool> supportsDataBreakpoints;
  std::optional<bool> supportsReadMemoryRequest;
  std::optional<bool> supportsWriteMemoryRequest;
  std::optional<bool> supportsDisassembleRequest;
  std::optional<bool> supportsCancelRequest;
  std::optional<bool> supportsBreakpointLocationRequest;
  std::optional<bool> supportsClipboardContext;
  std::optional<bool> supportsSteppingGranularity;
  std::optional<bool> supportsInstructionBreakpoints;
  std::optional<bool> supportsExceptionFilterOptions;
  std::optional<bool> supportsSingleThreadExecutionRequests;
};

void to_json(json& j, const Capabilities& p) {
  to_optJson(j, "supportsConfigurationDoneRequest",
      p.supportsConfigurationDoneRequest);
  to_optJson(j, "supportsFunctionBreakpoints", p.supportsFunctionBreakpoints);
  to_optJson(j, "supportsHitConditionalBreakpoints",
      p.supportsHitConditionalBreakpoints);
  to_optJson(j, "supportsEvaluateForHovers", p.supportsEvaluateForHovers);
  to_optJson(j, "exceptionBreakpointFilters", p.exceptionBreakpointFilters);
  to_optJson(j, "supportsStepBack", p.supportsStepBack);
  to_optJson(j, "supportsSetVariables", p.supportsSetVariables);
  to_optJson(j, "supportsRestartFrame", p.supportsRestartFrame);
  to_optJson(j, "supportsGotoTargetsRequest", p.supportsGotoTargetsRequest);
  to_optJson(j, "supportsStepInTargetsRequest", p.supportsStepInTargetsRequest);
  to_optJson(j, "supportsCompletionsRequest", p.supportsCompletionsRequest);
  to_optJson(j, "completionTriggerCharacters", p.completionTriggerCharacters);
  to_optJson(j, "supportsModulesRequest", p.supportsModulesRequest);
  to_optJson(j, "additionalModuleColumns", p.additionalModuleColumns);
  to_optJson(j, "supportedChecksumAlgorithms", p.supportedChecksumAlgorithms);
  to_optJson(j, "supportsRestartRequest", p.supportsRestartRequest);
  to_optJson(j, "supportsExceptionOptions", p.supportsExceptionOptions);
  to_optJson(j, "supportsValueFormattingOptions",
      p.supportsValueFormattingOptions);
  to_optJson(j, "supportsExceptionInfoRequest", p.supportsExceptionInfoRequest);
  to_optJson(j, "supportTerminateDebugee", p.supportTerminateDebugee);
  to_optJson(j, "supportSuspendDebugee", p.supportSuspendDebugee);
  to_optJson(j, "supportsDelayedStackTraceLoading",
      p.supportsDelayedStackTraceLoading);
  to_optJson(j, "supportsLoadedSourcesRequest", p.supportsLoadedSourcesRequest);
  to_optJson(j, "supportsLogPoints", p.supportsLogPoints);
  to_optJson(j, "supportsTerminateThreadsRequest",
      p.supportsTerminateThreadsRequest);
  to_optJson(j, "supportsSetExpression", p.supportsSetExpression);
  to_optJson(j, "supportsTerminateRequest", p.supportsTerminateRequest);
  to_optJson(j, "supportsDataBreakpoints", p.supportsDataBreakpoints);
  to_optJson(j, "supportsReadMemoryRequest", p.supportsReadMemoryRequest);
  to_optJson(j, "supportsWriteMemoryRequest", p.supportsWriteMemoryRequest);
  to_optJson(j, "supportsDisassembleRequest", p.supportsDisassembleRequest);
  to_optJson(j, "supportsCancelRequest", p.supportsCancelRequest);
  to_optJson(j, "supportsBreakpointLocationRequest",
      p.supportsBreakpointLocationRequest);
  to_optJson(j, "supportsClipboardContext", p.supportsClipboardContext);
  to_optJson(j, "supportsSteppingGranularity", p.supportsSteppingGranularity);
  to_optJson(j, "supportsInstructionBreakpoints",
      p.supportsInstructionBreakpoints);
  to_optJson(j, "supportsExceptionFilterOptions",
      p.supportsExceptionFilterOptions);
  to_optJson(j, "supportsSingleThreadExecutionRequests",
      p.supportsSingleThreadExecutionRequests);
}

void from_json(const json& j, Capabilities& p) {
  from_optJson(j, "supportsConfigurationDoneRequest",
      p.supportsConfigurationDoneRequest);
  from_optJson(j, "supportsFunctionBreakpoints", p.supportsFunctionBreakpoints);
  from_optJson(j, "supportsHitConditionalBreakpoints",
      p.supportsHitConditionalBreakpoints);
  from_optJson(j, "supportsEvaluateForHovers", p.supportsEvaluateForHovers);
  from_optJson(j, "exceptionBreakpointFilters", p.exceptionBreakpointFilters);
  from_optJson(j, "supportsStepBack", p.supportsStepBack);
  from_optJson(j, "supportsSetVariables", p.supportsSetVariables);
  from_optJson(j, "supportsRestartFrame", p.supportsRestartFrame);
  from_optJson(j, "supportsGotoTargetsRequest", p.supportsGotoTargetsRequest);
  from_optJson(j, "supportsStepInTargetsRequest",
      p.supportsStepInTargetsRequest);
  from_optJson(j, "supportsCompletionsRequest", p.supportsCompletionsRequest);
  from_optJson(j, "completionTriggerCharacters", p.completionTriggerCharacters);
  from_optJson(j, "supportsModulesRequest", p.supportsModulesRequest);
  from_optJson(j, "additionalModuleColumns", p.additionalModuleColumns);
  from_optJson(j, "supportedChecksumAlgorithms", p.supportedChecksumAlgorithms);
  from_optJson(j, "supportsRestartRequest", p.supportsRestartRequest);
  from_optJson(j, "supportsExceptionOptions", p.supportsExceptionOptions);
  from_optJson(j, "supportsValueFormattingOptions",
      p.supportsValueFormattingOptions);
  from_optJson(j, "supportsExceptionInfoRequest",
      p.supportsExceptionInfoRequest);
  from_optJson(j, "supportTerminateDebugee", p.supportTerminateDebugee);
  from_optJson(j, "supportSuspendDebugee", p.supportSuspendDebugee);
  from_optJson(j, "supportsDelayedStackTraceLoading",
      p.supportsDelayedStackTraceLoading);
  from_optJson(j, "supportsLoadedSourcesRequest",
      p.supportsLoadedSourcesRequest);
  from_optJson(j, "supportsLogPoints", p.supportsLogPoints);
  from_optJson(j, "supportsTerminateThreadsRequest",
      p.supportsTerminateThreadsRequest);
  from_optJson(j, "supportsSetExpression", p.supportsSetExpression);
  from_optJson(j, "supportsTerminateRequest", p.supportsTerminateRequest);
  from_optJson(j, "supportsDataBreakpoints", p.supportsDataBreakpoints);
  from_optJson(j, "supportsReadMemoryRequest", p.supportsReadMemoryRequest);
  from_optJson(j, "supportsWriteMemoryRequest", p.supportsWriteMemoryRequest);
  from_optJson(j, "supportsDisassembleRequest", p.supportsDisassembleRequest);
  from_optJson(j, "supportsCancelRequest", p.supportsCancelRequest);
  from_optJson(j, "supportsBreakpointLocationRequest",
      p.supportsBreakpointLocationRequest);
  from_optJson(j, "supportsClipboardContext", p.supportsClipboardContext);
  from_optJson(j, "supportsSteppingGranularity", p.supportsSteppingGranularity);
  from_optJson(j, "supportsInstructionBreakpoints",
      p.supportsInstructionBreakpoints);
  from_optJson(j, "supportsExceptionFilterOptions",
      p.supportsExceptionFilterOptions);
  from_optJson(j, "supportsSingleThreadExecutionRequests",
      p.supportsSingleThreadExecutionRequests);
}

enum struct SourcePresentationHint {
  normal,
  emphasize,
  deemphasize,
};

void to_json(json& j, const SourcePresentationHint& p) {
  switch(p) {
    case SourcePresentationHint::normal:
      j = "normal";
      break;
    case SourcePresentationHint::emphasize:
      j = "emphasize";
      break;
    case SourcePresentationHint::deemphasize:
      j = "deemphasize";
      break;
    default:
      throw std::runtime_error {"Unknown SourcePresentationHint"};
  };
}

void from_json(const json& j, SourcePresentationHint& p) {
  std::string s {j.get<std::string>()};
  if(s == "normal")
    p = SourcePresentationHint::normal;
  else if(s == "emphasize")
    p = SourcePresentationHint::emphasize;
  else if(s == "deemphasize")
    p = SourcePresentationHint::deemphasize;
  else
    throw std::runtime_error {"Unknown SourcePresentationHint"};
}

struct Source {
  std::optional<std::string> name;
  std::optional<std::string> path;
  std::optional<std::int64_t> sourceReference;
  std::optional<SourcePresentationHint> presentationHint;
  std::optional<std::string> origin;
  std::optional<std::vector<Source>> sources;
  std::optional<json> adapterData;
  std::optional<std::vector<Checksum>> checksums;
};

void to_json(json& j, const Source& p) {
  to_optJson(j, "name", p.name);
  to_optJson(j, "path", p.path);
  to_optJson(j, "sourceReference", p.sourceReference);
  to_optJson(j, "presentationHint", p.presentationHint);
  to_optJson(j, "origin", p.origin);
  to_optJson(j, "sources", p.sources);
  to_optJson(j, "adapterData", p.adapterData);
  to_optJson(j, "checksums", p.checksums);
}

void from_json(const json& j, Source& p) {
  from_optJson(j, "name", p.name);
  from_optJson(j, "path", p.path);
  from_optJson(j, "sourceReference", p.sourceReference);
  from_optJson(j, "presentationHint", p.presentationHint);
  from_optJson(j, "origin", p.origin);
  from_optJson(j, "sources", p.sources);
  from_optJson(j, "adapterData", p.adapterData);
  from_optJson(j, "checksums", p.checksums);
}

struct InstructionBreakpoint {
  std::string instructionReference;
  std::optional<std::int64_t> offset;
  std::optional<std::string> condition;
  std::optional<std::string> hitCondition;
};

void to_json(json& j, const InstructionBreakpoint& p) {
  j["instructionReference"] = p.instructionReference;
  to_optJson(j, "offset", p.offset);
  to_optJson(j, "condition", p.condition);
  to_optJson(j, "hitCondition", p.hitCondition);
}

void from_json(const json& j, InstructionBreakpoint& p) {
  j.at("instructionReference").get_to(p.instructionReference);
  from_optJson(j, "offset", p.offset);
  from_optJson(j, "condition", p.condition);
  from_optJson(j, "hitCondition", p.hitCondition);
}

enum struct DataBreakpointAccessType {
  read,
  write,
  readWrite,
};

void to_json(json& j, const DataBreakpointAccessType& p) {
  switch(p) {
    case DataBreakpointAccessType::read:
      j = "read";
      break;
    case DataBreakpointAccessType::write:
      j = "write";
      break;
    case DataBreakpointAccessType::readWrite:
      j = "readWrite";
      break;
    default:
      throw std::runtime_error {"Unknown DataBreakpointAccessType"};
  };
}

void from_json(const json& j, DataBreakpointAccessType& p) {
  std::string s {j.get<std::string>()};
  if(s == "read")
    p = DataBreakpointAccessType::read;
  else if(s == "write")
    p = DataBreakpointAccessType::write;
  else if(s == "readWrite")
    p = DataBreakpointAccessType::readWrite;
  else
    throw std::runtime_error {"Unknown DataBreakpointAccessType"};
}

struct DataBreakpoint {
  std::string dataId;
  std::optional<DataBreakpointAccessType> accessType;
  std::optional<std::string> condition;
  std::optional<std::string> hitCondition;
};

void to_json(json& j, const DataBreakpoint& p) {
  j["dataId"] = p.dataId;
  to_optJson(j, "accessType", p.accessType);
  to_optJson(j, "condition", p.condition);
  to_optJson(j, "hitCondition", p.hitCondition);
}

void from_json(const json& j, DataBreakpoint& p) {
  j.at("dataId").get_to(p.dataId);
  from_optJson(j, "accessType", p.accessType);
  from_optJson(j, "condition", p.condition);
  from_optJson(j, "hitCondition", p.hitCondition);
}

struct FunctionBreakpoint {
  std::string name;
  std::optional<std::string> condition;
  std::optional<std::string> hitCondition;
};

void to_json(json& j, const FunctionBreakpoint& p) {
  j["name"] = p.name;
  to_optJson(j, "condition", p.condition);
  to_optJson(j, "hitCondition", p.hitCondition);
}

void from_json(const json& j, FunctionBreakpoint& p) {
  j.at("name").get_to(p.name);
  from_optJson(j, "condition", p.condition);
  from_optJson(j, "hitCondition", p.hitCondition);
}

struct Breakpoint {
  std::optional<std::int64_t> id;
  bool verified;
  std::optional<std::string> message;
  std::optional<Source> source;
  std::optional<std::int64_t> line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
  std::optional<std::string> instructionReference;
  std::optional<std::int64_t> number;
};

void to_json(json& j, const Breakpoint& p) {
  j["verified"] = p.verified;
  to_optJson(j, "id", p.id);
  to_optJson(j, "message", p.message);
  to_optJson(j, "source", p.source);
  to_optJson(j, "line", p.line);
  to_optJson(j, "column", p.column);
  to_optJson(j, "endLine", p.endLine);
  to_optJson(j, "endColumn", p.endColumn);
  to_optJson(j, "instructionReference", p.instructionReference);
  to_optJson(j, "number", p.number);
}

void from_json(const json& j, Breakpoint& p) {
  j.at("verified").get_to(p.verified);
  from_optJson(j, "id", p.id);
  from_optJson(j, "message", p.message);
  from_optJson(j, "source", p.source);
  from_optJson(j, "line", p.line);
  from_optJson(j, "column", p.column);
  from_optJson(j, "endLine", p.endLine);
  from_optJson(j, "endColumn", p.endColumn);
  from_optJson(j, "instructionReference", p.instructionReference);
  from_optJson(j, "number", p.number);
}

struct BreakpointLocation {
  std::int64_t line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
};

void to_json(json& j, const BreakpointLocation& p) {
  j["line"] = p.line;
  to_optJson(j, "column", p.column);
  to_optJson(j, "endLine", p.endLine);
  to_optJson(j, "endColumn", p.endColumn);
}

void from_json(const json& j, BreakpointLocation& p) {
  j.at("line").get_to(p.line);
  from_optJson(j, "column", p.column);
  from_optJson(j, "endLine", p.endLine);
  from_optJson(j, "endColumn", p.endColumn);
}

enum struct SteppingGranularity {
  statement,
  line,
  instruction,
};

void to_json(json& j, const SteppingGranularity& p) {
  switch(p) {
    case SteppingGranularity::statement:
      j = "statement";
      break;
    case SteppingGranularity::line:
      j = "line";
      break;
    case SteppingGranularity::instruction:
      j = "instruction";
      break;
    default:
      throw std::runtime_error {"Unknown SteppingGranularity"};
  };
}

void from_json(const json& j, SteppingGranularity& p) {
  std::string s {j.get<std::string>()};
  if(s == "statement")
    p = SteppingGranularity::statement;
  else if(s == "line")
    p = SteppingGranularity::line;
  else if(s == "instruction")
    p = SteppingGranularity::instruction;
  else
    throw std::runtime_error {"Unknown SteppingGranularity"};
}

struct Module {
  std::variant<std::int64_t, std::string> id;
  std::string name;
  std::optional<std::string> path;
  std::optional<bool> isOptimized;
  std::optional<bool> isUserCode;
  std::optional<std::string> version;
  std::optional<std::string> symbolStatus;
  std::optional<std::string> symbolFilePath;
  std::optional<std::string> dateTimeStamp;
  std::optional<std::string> addressRange;
};

void to_json(json& j, const Module& p) {
  j["id"] = p.id;
  j["name"] = p.name;
  to_optJson(j, "path", p.path);
  to_optJson(j, "isOptimized", p.isOptimized);
  to_optJson(j, "isUserCode", p.isUserCode);
  to_optJson(j, "version", p.version);
  to_optJson(j, "symbolStatus", p.symbolStatus);
  to_optJson(j, "symbolFilePath", p.symbolFilePath);
  to_optJson(j, "dateTimeStamp", p.dateTimeStamp);
  to_optJson(j, "addressRange", p.addressRange);
}

void from_json(const json& j, Module& p) {
  j.at("id").get_to(p.id);
  j.at("name").get_to(p.name);
  from_optJson(j, "path", p.path);
  from_optJson(j, "isOptimized", p.isOptimized);
  from_optJson(j, "isUserCode", p.isUserCode);
  from_optJson(j, "version", p.version);
  from_optJson(j, "symbolStatus", p.symbolStatus);
  from_optJson(j, "symbolFilePath", p.symbolFilePath);
  from_optJson(j, "dateTimeStamp", p.dateTimeStamp);
  from_optJson(j, "addressRange", p.addressRange);
}

struct StackFrameFormat {
  std::optional<bool> parameters;
  std::optional<bool> parameterTypes;
  std::optional<bool> parameterNames;
  std::optional<bool> parameterValues;
  std::optional<bool> line;
  std::optional<bool> module;
  std::optional<bool> includeAll;
};

void to_json(json& j, const StackFrameFormat& p) {
  to_optJson(j, "parameters", p.parameters);
  to_optJson(j, "parameterTypes", p.parameterTypes);
  to_optJson(j, "parameterNames", p.parameterNames);
  to_optJson(j, "parameterValues", p.parameterValues);
  to_optJson(j, "line", p.line);
  to_optJson(j, "module", p.module);
  to_optJson(j, "includeAll", p.includeAll);
}

void from_json(const json& j, StackFrameFormat& p) {
  from_optJson(j, "parameters", p.parameters);
  from_optJson(j, "parameterTypes", p.parameterTypes);
  from_optJson(j, "parameterNames", p.parameterNames);
  from_optJson(j, "parameterValues", p.parameterValues);
  from_optJson(j, "line", p.line);
  from_optJson(j, "module", p.module);
  from_optJson(j, "includeAll", p.includeAll);
}

enum struct StackFramePresentationHint {
  normal,
  label,
  subtle,
};

void to_json(json& j, const StackFramePresentationHint& p) {
  switch(p) {
    case StackFramePresentationHint::normal:
      j = "normal";
      break;
    case StackFramePresentationHint::label:
      j = "label";
      break;
    case StackFramePresentationHint::subtle:
      j = "subtle";
      break;
    default:
      throw std::runtime_error {"Unknown StackFramePresentationHint"};
  };
}

void from_json(const json& j, StackFramePresentationHint& p) {
  std::string s {j.get<std::string>()};
  if(s == "normal")
    p = StackFramePresentationHint::normal;
  else if(s == "label")
    p = StackFramePresentationHint::label;
  else if(s == "subtle")
    p = StackFramePresentationHint::subtle;
  else
    throw std::runtime_error {"Unknown StackFramePresentationHint"};
}

struct StackFrame {
  std::int64_t id;
  std::string name;
  Source source;
  std::int64_t line;
  std::int64_t column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
  std::optional<bool> canRestart;
  std::optional<std::string> instructionPointerReference;
  std::optional<std::variant<std::int64_t, std::string>> moduleId;
  std::optional<StackFramePresentationHint> presentationHint;
};

void to_json(json& j, const StackFrame& p) {
  j["id"] = p.id;
  j["name"] = p.name;
  j["source"] = p.source;
  j["line"] = p.line;
  j["column"] = p.column;
  to_optJson(j, "endLine", p.endLine);
  to_optJson(j, "endColumn", p.endColumn);
  to_optJson(j, "canRestart", p.canRestart);
  to_optJson(j, "instructionPointerReference", p.instructionPointerReference);
  to_optJson(j, "moduleId", p.moduleId);
  to_optJson(j, "presentationHint", p.presentationHint);
}

void from_json(const json& j, StackFrame& p) {
  j.at("id").get_to(p.id);
  j.at("name").get_to(p.name);
  j.at("source").get_to(p.source);
  j.at("line").get_to(p.line);
  j.at("column").get_to(p.column);
  from_optJson(j, "endLine", p.endLine);
  from_optJson(j, "endColumn", p.endColumn);
  from_optJson(j, "canRestart", p.canRestart);
  from_optJson(j, "instructionPointerReference", p.instructionPointerReference);
  from_optJson(j, "moduleId", p.moduleId);
  from_optJson(j, "presentationHint", p.presentationHint);
}

struct Scope {
  std::string name;
  std::string presentationHint;
  std::int64_t variablesReference;
  std::optional<std::int64_t> namedVariables;
  std::optional<std::int64_t> indexedVariables;
  bool expensive;
  std::optional<Source> source;
  std::optional<std::int64_t> line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
};

void to_json(json& j, const Scope& p) {
  j["name"] = p.name;
  j["presentationHint"] = p.presentationHint;
  j["variablesReference"] = p.variablesReference;
  to_optJson(j, "namedVariables", p.namedVariables);
  to_optJson(j, "indexedVariables", p.indexedVariables);
  j["expensive"] = p.expensive;
  to_optJson(j, "source", p.source);
  to_optJson(j, "line", p.line);
  to_optJson(j, "column", p.column);
  to_optJson(j, "endLine", p.endLine);
  to_optJson(j, "endColumn", p.endColumn);
}

void from_json(const json& j, Scope& p) {
  j.at("name").get_to(p.name);
  j.at("presentationHint").get_to(p.presentationHint);
  j.at("variablesReference").get_to(p.variablesReference);
  from_optJson(j, "namedVariables", p.namedVariables);
  from_optJson(j, "indexedVariables", p.indexedVariables);
  j.at("expensive").get_to(p.expensive);
  from_optJson(j, "source", p.source);
  from_optJson(j, "line", p.line);
  from_optJson(j, "column", p.column);
  from_optJson(j, "endLine", p.endLine);
  from_optJson(j, "endColumn", p.endColumn);
}

struct ValueFormat {
  std::optional<bool> hex;
};

void to_json(json& j, const ValueFormat& p) {
  to_optJson(j, "hex", p.hex);
}

void from_json(const json& j, ValueFormat& p) {
  from_optJson(j, "hex", p.hex);
}

struct VariablePresentationHint {
  std::string kind;
  std::optional<std::vector<std::string>> attributes;
  std::optional<std::string> visibility;
  std::optional<bool> lazy;
};

void to_json(json& j, const VariablePresentationHint& p) {
  j["kind"] = p.kind;
  to_optJson(j, "attributes", p.attributes);
  to_optJson(j, "visibility", p.visibility);
  to_optJson(j, "lazy", p.lazy);
}

void from_json(const json& j, VariablePresentationHint& p) {
  j.at("kind").get_to(p.kind);
  from_optJson(j, "attributes", p.attributes);
  from_optJson(j, "visibility", p.visibility);
  from_optJson(j, "lazy", p.lazy);
}

struct Variable {
  std::string name;
  std::string value;
  std::optional<std::string> type;
  std::optional<VariablePresentationHint> presentationHint;
  std::optional<std::string> evaluateName;
  std::int64_t variablesReference;
  std::optional<std::int64_t> namedVariables;
  std::optional<std::int64_t> indexedVariables;
  std::optional<std::string> memoryReference;
};

void to_json(json& j, const Variable& p) {
  j["name"] = p.name;
  j["value"] = p.value;
  to_optJson(j, "type", p.type);
  to_optJson(j, "presentationHint", p.presentationHint);
  to_optJson(j, "evaluateName", p.evaluateName);
  j["variablesReference"] = p.variablesReference;
  to_optJson(j, "namedVariables", p.namedVariables);
  to_optJson(j, "indexedVariables", p.indexedVariables);
  to_optJson(j, "memoryReference", p.memoryReference);
}

void from_json(const json& j, Variable& p) {
  j.at("name").get_to(p.name);
  j.at("value").get_to(p.value);
  from_optJson(j, "type", p.type);
  from_optJson(j, "presentationHint", p.presentationHint);
  from_optJson(j, "evaluateName", p.evaluateName);
  j.at("variablesReference").get_to(p.variablesReference);
  from_optJson(j, "namedVariables", p.namedVariables);
  from_optJson(j, "indexedVariables", p.indexedVariables);
  from_optJson(j, "memoryReference", p.memoryReference);
}

struct Thread {
  int64_t id;
  std::string name;
};

void to_json(json& j, const Thread& p) {
  j["id"] = p.id;
  j["name"] = p.name;
}

void from_json(const json& j, Thread& p) {
  j.at("id").get_to(p.id);
  j.at("name").get_to(p.name);
}

struct StepInTarget {
  int64_t id;
  std::string label;
};

void to_json(json& j, const StepInTarget& p) {
  j["id"] = p.id;
  j["label"] = p.label;
}

void from_json(const json& j, StepInTarget& p) {
  j.at("id").get_to(p.id);
  j.at("label").get_to(p.label);
}

struct GotoTarget {
  int64_t id;
  std::string label;
  std::int64_t line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
  std::optional<std::string> instructionPointerReference;
};

void to_json(json& j, const GotoTarget& p) {
  j["id"] = p.id;
  j["label"] = p.label;
  j["line"] = p.line;
  to_optJson(j, "column", p.column);
  to_optJson(j, "endLine", p.endLine);
  to_optJson(j, "endColumn", p.endColumn);
  to_optJson(j, "instructionPointerReference", p.instructionPointerReference);
}

void from_json(const json& j, GotoTarget& p) {
  j.at("id").get_to(p.id);
  j.at("label").get_to(p.label);
  j.at("line").get_to(p.line);
  from_optJson(j, "column", p.column);
  from_optJson(j, "endLine", p.endLine);
  from_optJson(j, "endColumn", p.endColumn);
  from_optJson(j, "instructionPointerReference", p.instructionPointerReference);
}

enum struct CompletionItemType {
  method,
  function,
  constructor,
  field,
  variable,
  class_,
  interface,
  module,
  property,
  unit,
  value,
  enum_,
  keyword,
  snippet,
  text,
  color,
  file,
  reference,
  customColor,
};

void to_json(json& j, const CompletionItemType& p) {
  switch(p) {
    case CompletionItemType::method:
      j = "method";
      break;
    case CompletionItemType::function:
      j = "function";
      break;
    case CompletionItemType::constructor:
      j = "constructor";
      break;
    case CompletionItemType::field:
      j = "field";
      break;
    case CompletionItemType::variable:
      j = "variable";
      break;
    case CompletionItemType::class_:
      j = "class";
      break;
    case CompletionItemType::interface:
      j = "interface";
      break;
    case CompletionItemType::module:
      j = "module";
      break;
    case CompletionItemType::property:
      j = "property";
      break;
    case CompletionItemType::unit:
      j = "unit";
      break;
    case CompletionItemType::value:
      j = "value";
      break;
    case CompletionItemType::enum_:
      j = "enum";
      break;
    case CompletionItemType::keyword:
      j = "keyword";
      break;
    case CompletionItemType::snippet:
      j = "snippet";
      break;
    case CompletionItemType::text:
      j = "text";
      break;
    case CompletionItemType::color:
      j = "color";
      break;
    case CompletionItemType::file:
      j = "file";
      break;
    case CompletionItemType::reference:
      j = "reference";
      break;
    case CompletionItemType::customColor:
      j = "customColor";
      break;
    default:
      throw std::runtime_error {"Unknown CompletionItemType"};
  };
}

void from_json(const json& j, CompletionItemType& p) {
  std::string s {j.get<std::string>()};
  if(j == "method")
    p = CompletionItemType::method;
  else if(j == "function")
    p = CompletionItemType::function;
  else if(j == "constructor")
    p = CompletionItemType::constructor;
  else if(j == "field")
    p = CompletionItemType::field;
  else if(j == "variable")
    p = CompletionItemType::variable;
  else if(j == "class")
    p = CompletionItemType::class_;
  else if(j == "interface")
    p = CompletionItemType::interface;
  else if(j == "module")
    p = CompletionItemType::module;
  else if(j == "property")
    p = CompletionItemType::property;
  else if(j == "unit")
    p = CompletionItemType::unit;
  else if(j == "value")
    p = CompletionItemType::value;
  else if(j == "enum")
    p = CompletionItemType::enum_;
  else if(j == "keyword")
    p = CompletionItemType::keyword;
  else if(j == "snippet")
    p = CompletionItemType::snippet;
  else if(j == "text")
    p = CompletionItemType::text;
  else if(j == "color")
    p = CompletionItemType::color;
  else if(j == "file")
    p = CompletionItemType::file;
  else if(j == "reference")
    p = CompletionItemType::reference;
  else if(j == "customColor")
    p = CompletionItemType::customColor;
  else
    throw std::runtime_error {"Unknown CompletionItemType"};
}

struct CompletionItem {
  std::string label;
  std::optional<std::string> text;
  std::optional<std::string> sortText;
  std::optional<std::string> detail;
  std::optional<CompletionItemType> type;
  std::optional<std::int64_t> start;
  std::optional<std::int64_t> length;
  std::optional<std::int64_t> selectionStart;
  std::optional<std::int64_t> selectionLength;
};

void to_json(json& j, const CompletionItem& p) {
  j["label"] = p.label;
  to_optJson(j, "text", p.text);
  to_optJson(j, "sortText", p.sortText);
  to_optJson(j, "detail", p.detail);
  to_optJson(j, "type", p.type);
  to_optJson(j, "start", p.start);
  to_optJson(j, "length", p.length);
  to_optJson(j, "selectionStart", p.selectionStart);
  to_optJson(j, "selectionLength", p.selectionLength);
}

void from_json(const json& j, CompletionItem& p) {
  j.at("label").get_to(p.label);
  from_optJson(j, "text", p.text);
  from_optJson(j, "sortText", p.sortText);
  from_optJson(j, "detail", p.detail);
  from_optJson(j, "type", p.type);
  from_optJson(j, "start", p.start);
  from_optJson(j, "length", p.length);
  from_optJson(j, "selectionStart", p.selectionStart);
  from_optJson(j, "selectionLength", p.selectionLength);
}

struct DisassembledInstruction {
  std::string address;
  std::optional<std::string> instructionBytes;
  std::string instruction;
  std::optional<std::string> symbol;
  std::optional<Source> location;
  std::optional<std::int64_t> line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
};

void to_json(json& j, const DisassembledInstruction& p) {
  j["address"] = p.address;
  to_optJson(j, "instructionBytes", p.instructionBytes);
  j["instruction"] = p.instruction;
  to_optJson(j, "symbol", p.symbol);
  to_optJson(j, "location", p.location);
  to_optJson(j, "line", p.line);
  to_optJson(j, "column", p.column);
  to_optJson(j, "endLine", p.endLine);
  to_optJson(j, "endColumn", p.endColumn);
}

void from_json(const json& j, DisassembledInstruction& p) {
  j.at("address").get_to(p.address);
  from_optJson(j, "instructionBytes", p.instructionBytes);
  j.at("instruction").get_to(p.instruction);
  from_optJson(j, "symbol", p.symbol);
  from_optJson(j, "location", p.location);
  from_optJson(j, "line", p.line);
  from_optJson(j, "column", p.column);
  from_optJson(j, "endLine", p.endLine);
  from_optJson(j, "endColumn", p.endColumn);
}

enum struct MessageType {
  request,
  response,
  event,
};

void to_json(json& j, const MessageType& p) {
  switch(p) {
    case MessageType::request:
      j = "request";
      break;
    case MessageType::response:
      j = "response";
      break;
    case MessageType::event:
      j = "event";
      break;
    default:
      throw std::runtime_error {"Unknown MessageType"};
  };
}

void from_json(const json& j, MessageType& p) {
  std::string s {j.get<std::string>()};
  if(s == "request")
    p = MessageType::request;
  else if(s == "response")
    p = MessageType::response;
  else if(s == "event")
    p = MessageType::event;
  else
    throw std::runtime_error {"Unknown MessageType"};
}


struct ProtocolMessage {
  std::int64_t seq;
  MessageType type;
};

void to_json(json& j, const ProtocolMessage& p) {
  j["seq"] = p.seq;
  j["type"] = p.type;
}

void from_json(const json& j, ProtocolMessage& p) {
  j.at("seq").get_to(p.seq);
  j.at("type").get_to(p.type);
}

enum struct CommandType {
  cancel,
  runInTerminal,
  initialize,
  configurationDone,
  launch,
  attach,
  restart,
  disconnect,
  terminate,
  breakpointLocations,
  setBreakpoints,
  setFunctionBreakpoints,
  setExceptionBreakpoints,
  dataBreakpointInfo,
  setDataBreakpoints,
  setInstructionBreakpoints,
  continue_,
  next,
  stepIn,
  stepOut,
  stepBack,
  reverseContinue,
  restartFrame,
  goto_,
  pause,
  stackTrace,
  scopes,
  variables,
  setVariable,
  source,
  threads,
  terminateThreads,
  modules,
  loadedSources,
  evaluate,
  setExpression,
  stepInTargets,
  gotoTargets,
  completions,
  exceptionInfo,
  readMemory,
  writeMemory,
  disassemble,
};

void to_json(json& j, const CommandType& p) {
  switch(p) {
    case CommandType::cancel:
      j = "cancel";
      break;
    case CommandType::runInTerminal:
      j = "runInTerminal";
      break;
    case CommandType::initialize:
      j = "initialize";
      break;
    case CommandType::configurationDone:
      j = "configurationDone";
      break;
    case CommandType::launch:
      j = "launch";
      break;
    case CommandType::attach:
      j = "attach";
      break;
    case CommandType::restart:
      j = "restart";
      break;
    case CommandType::disconnect:
      j = "disconnect";
      break;
    case CommandType::terminate:
      j = "terminate";
      break;
    case CommandType::breakpointLocations:
      j = "breakpointLocations";
      break;
    case CommandType::setBreakpoints:
      j = "setBreakpoints";
      break;
    case CommandType::setFunctionBreakpoints:
      j = "setFunctionBreakpoints";
      break;
    case CommandType::continue_:
      j = "continue";
      break;
    case CommandType::next:
      j = "next";
      break;
    case CommandType::stepIn:
      j = "stepIn";
      break;
    case CommandType::stepOut:
      j = "stepOut";
      break;
    case CommandType::stepBack:
      j = "stepBack";
      break;
    case CommandType::reverseContinue:
      j = "reverseContinue";
      break;
    case CommandType::restartFrame:
      j = "restartFrame";
      break;
    case CommandType::goto_:
      j = "goto";
      break;
    case CommandType::pause:
      j = "pause";
      break;
    case CommandType::stackTrace:
      j = "stackTrace";
      break;
    case CommandType::scopes:
      j = "scopes";
      break;
    case CommandType::variables:
      j = "variables";
      break;
    case CommandType::setVariable:
      j = "setVariable";
      break;
    case CommandType::source:
      j = "source";
      break;
    case CommandType::threads:
      j = "threads";
      break;
    case CommandType::terminateThreads:
      j = "terminateThreads";
      break;
    case CommandType::modules:
      j = "modules";
      break;
    case CommandType::loadedSources:
      j = "loadedSources";
      break;
    case CommandType::evaluate:
      j = "evaluate";
      break;
    case CommandType::setExpression:
      j = "setExpression";
      break;
    case CommandType::stepInTargets:
      j = "stepInTargets";
      break;
    case CommandType::gotoTargets:
      j = "gotoTargets";
      break;
    case CommandType::completions:
      j = "completions";
      break;
    case CommandType::exceptionInfo:
      j = "exceptionInfo";
      break;
    case CommandType::readMemory:
      j = "readMemory";
      break;
    case CommandType::writeMemory:
      j = "writeMemory";
      break;
    case CommandType::disassemble:
      j = "disassemble";
      break;
    default:
      throw std::runtime_error {"Unknown CommandType"};
  };
}

void from_json(const json& j, CommandType& p) {
  std::string s {j.get<std::string>()};
  if(j == "cancel")
    p = CommandType::cancel;
  else if(j == "runInTerminal")
    p = CommandType::runInTerminal;
  else if(j == "initialize")
    p = CommandType::initialize;
  else if(j == "configurationDone")
    p = CommandType::configurationDone;
  else if(j == "launch")
    p = CommandType::launch;
  else if(j == "attach")
    p = CommandType::attach;
  else if(j == "restart")
    p = CommandType::restart;
  else if(j == "disconnect")
    p = CommandType::disconnect;
  else if(j == "terminate")
    p = CommandType::terminate;
  else if(j == "breakpointLocations")
    p = CommandType::breakpointLocations;
  else if(j == "setBreakpoints")
    p = CommandType::setBreakpoints;
  else if(j == "setFunctionBreakpoints")
    p = CommandType::setFunctionBreakpoints;
  else if(j == "continue")
    p = CommandType::continue_;
  else if(j == "next")
    p = CommandType::next;
  else if(j == "stepIn")
    p = CommandType::stepIn;
  else if(j == "stepOut")
    p = CommandType::stepOut;
  else if(j == "stepBack")
    p = CommandType::stepBack;
  else if(j == "reverseContinue")
    p = CommandType::reverseContinue;
  else if(j == "restartFrame")
    p = CommandType::restartFrame;
  else if(j == "goto")
    p = CommandType::goto_;
  else if(j == "pause")
    p = CommandType::pause;
  else if(j == "stackTrace")
    p = CommandType::stackTrace;
  else if(j == "scopes")
    p = CommandType::scopes;
  else if(j == "variables")
    p = CommandType::variables;
  else if(j == "setVariable")
    p = CommandType::setVariable;
  else if(j == "source")
    p = CommandType::source;
  else if(j == "threads")
    p = CommandType::threads;
  else if(j == "terminateThreads")
    p = CommandType::terminateThreads;
  else if(j == "modules")
    p = CommandType::modules;
  else if(j == "loadedSources")
    p = CommandType::loadedSources;
  else if(j == "evaluate")
    p = CommandType::evaluate;
  else if(j == "setExpression")
    p = CommandType::setExpression;
  else if(j == "stepInTargets")
    p = CommandType::stepInTargets;
  else if(j == "gotoTargets")
    p = CommandType::gotoTargets;
  else if(j == "completions")
    p = CommandType::completions;
  else if(j == "exceptionInfo")
    p = CommandType::exceptionInfo;
  else if(j == "readMemory")
    p = CommandType::readMemory;
  else if(j == "writeMemory")
    p = CommandType::writeMemory;
  else if(j == "disassemble")
    p = CommandType::disassemble;
  else
    throw std::runtime_error {"Unknown CommandType"};
}

struct Request : ProtocolMessage {
  CommandType command;
};

void to_json(json& j, const Request& p) {
  j = static_cast<ProtocolMessage>(p);
  j["command"] = p.command;
}

void from_json(const json& j, Request& p) {
  j.get_to(static_cast<ProtocolMessage&>(p));
  j.at("command").get_to(p.command);
}

enum struct EventType {
  initialized,
  stopped,
  continued,
  exited,
  thread,
  output,
  breakpoint,
  module,
  loadedSource,
  process,
  capabilities,
  progressStart,
  progressUpdate,
  progresSend,
  invalidated,
  memory,
};

void to_json(json& j, const EventType& p) {
  switch(p) {
    case EventType::initialized:
      j = "initialized";
      break;
    case EventType::stopped:
      j = "stopped";
      break;
    case EventType::continued:
      j = "continued";
      break;
    case EventType::exited:
      j = "exited";
      break;
    case EventType::thread:
      j = "thread";
      break;
    case EventType::output:
      j = "output";
      break;
    case EventType::breakpoint:
      j = "breakpoint";
      break;
    case EventType::module:
      j = "module";
      break;
    case EventType::loadedSource:
      j = "loadedSource";
      break;
    case EventType::process:
      j = "process";
      break;
    case EventType::capabilities:
      j = "capabilities";
      break;
    case EventType::progressStart:
      j = "progressStart";
      break;
    case EventType::progressUpdate:
      j = "progressUpdate";
      break;
    case EventType::progresSend:
      j = "progresSend";
      break;
    case EventType::invalidated:
      j = "invalidated";
      break;
    case EventType::memory:
      j = "memory";
      break;
    default:
      throw std::runtime_error {"Unknown EventType"};
  };
}

void from_json(const json& j, EventType& p) {
  std::string s {j.get<std::string>()};
  if(j == "initialized")
    p = EventType::initialized;
  else if(j == "stopped")
    p = EventType::stopped;
  else if(j == "continued")
    p = EventType::continued;
  else if(j == "exited")
    p = EventType::exited;
  else if(j == "thread")
    p = EventType::thread;
  else if(j == "output")
    p = EventType::output;
  else if(j == "breakpoint")
    p = EventType::breakpoint;
  else if(j == "module")
    p = EventType::module;
  else if(j == "loadedSource")
    p = EventType::loadedSource;
  else if(j == "process")
    p = EventType::process;
  else if(j == "capabilities")
    p = EventType::capabilities;
  else if(j == "progressStart")
    p = EventType::progressStart;
  else if(j == "progressUpdate")
    p = EventType::progressUpdate;
  else if(j == "progresSend")
    p = EventType::progresSend;
  else if(j == "invalidated")
    p = EventType::invalidated;
  else if(j == "memory")
    p = EventType::memory;
  else
    throw std::runtime_error {"Unknown EventType"};
}

struct Event : ProtocolMessage {
  EventType event;
};

void to_json(json& j, const Event& p) {
  j = static_cast<ProtocolMessage>(p);
  j["event"] = p.event;
}

void from_json(const json& j, Event& p) {
  j.get_to(static_cast<ProtocolMessage&>(p));
  j.at("event").get_to(p.event);
}

struct Response : ProtocolMessage {
  std::int64_t request_seq;
  bool success;
  CommandType command;
  std::optional<std::string> message;
};

void to_json(json& j, const Response& p) {
  j = static_cast<ProtocolMessage>(p);
  j["request_seq"] = p.request_seq;
  j["success"] = p.success;
  j["command"] = p.command;
  to_optJson(j, "message", p.message);
}

void from_json(const json& j, Response& p) {
  j.get_to(static_cast<ProtocolMessage&>(p));
  j.at("request_seq").get_to(p.request_seq);
  j.at("success").get_to(p.success);
  j.at("command").get_to(p.command);
  from_optJson(j, "message", p.message);
}

struct ErrorResponse : Response {
  std::optional<std::string> error;
};

void to_json(json& j, const ErrorResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  to_optJson(body, "error", p.error);
}

void from_json(const json& j, ErrorResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  from_optJson(body, "error", p.error);
}

struct CancelRequest : Request {
  std::optional<std::int64_t> requestId;
  std::optional<std::string> progressId;
};

void to_json(json& j, const CancelRequest& p) {
  j = static_cast<Request>(p);
  if(p.progressId || p.progressId) {
    json& arguments {j["arguments"]};
    to_optJson(arguments, "requestId", p.requestId);
    to_optJson(arguments, "progressId", p.progressId);
  }
}

void from_json(const json& j, CancelRequest& p) {
  j.get_to(static_cast<Request&>(p));
  if(auto it {j.find("arguments")}; it != j.end()) {
    from_optJson(*it, "requestId", p.requestId);
    from_optJson(*it, "progressId", p.progressId);
  }
}

using CancelResponse = Response;

using InitializedEvent = Event;

struct StoppedEvent : Event {
  std::string reason;
  std::optional<std::string> description;
  std::optional<std::int64_t> threadId;
  std::optional<bool> preserveFocusHint;
  std::optional<std::string> text;
  std::optional<bool> allThreadsStopped;
  std::optional<std::vector<std::int64_t>> hitBreakpointIds;
};

void to_json(json& j, const StoppedEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["reason"] = p.reason;
  to_optJson(body, "description", p.description);
  to_optJson(body, "threadId", p.threadId);
  to_optJson(body, "preserveFocusHint", p.preserveFocusHint);
  to_optJson(body, "text", p.text);
  to_optJson(body, "allThreadsStopped", p.allThreadsStopped);
  to_optJson(body, "hitBreakpointIds", p.hitBreakpointIds);
}

void from_json(const json& j, StoppedEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("reason").get_to(p.reason);
  from_optJson(body, "description", p.description);
  from_optJson(body, "threadId", p.threadId);
  from_optJson(body, "preserveFocusHint", p.preserveFocusHint);
  from_optJson(body, "text", p.text);
  from_optJson(body, "allThreadsStopped", p.allThreadsStopped);
  from_optJson(body, "hitBreakpointIds", p.hitBreakpointIds);
}

struct ContinuedEvent : Event {
  std::int64_t threadId;
  std::optional<bool> allThreadsContinued;
};

void to_json(json& j, const ContinuedEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["threadId"] = p.threadId;
  to_optJson(body, "allThreadsContinued", p.allThreadsContinued);
}

void from_json(const json& j, ContinuedEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("threadId").get_to(p.threadId);
  from_optJson(body, "allThreadsContinued", p.allThreadsContinued);
}

struct ExitedEvent : Event {
  std::int64_t exitCode;
};

void to_json(json& j, const ExitedEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["exitCode"] = p.exitCode;
}

void from_json(const json& j, ExitedEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("exitCode").get_to(p.exitCode);
}

struct TerminatedEvent : Event {
  std::optional<json> restart;
};

void to_json(json& j, const TerminatedEvent& p) {
  j = static_cast<Event>(p);
  if(p.restart)
    j["body"]["restart"] = *p.restart;
}

void from_json(const json& j, TerminatedEvent& p) {
  j.get_to(static_cast<Event&>(p));
  if(auto it {j.find("body")}; it != j.end())
    from_optJson(*it, "restart", p.restart);
}

struct ThreadEvent : Event {
  std::string reason;
  std::int64_t threadId;
};

void to_json(json& j, const ThreadEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["reason"] = p.reason;
  body["threadId"] = p.threadId;
}

void from_json(const json& j, ThreadEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("reason").get_to(p.reason);
  body.at("threadId").get_to(p.threadId);
}

enum struct OutputGroup {
  start,
  startCollapsed,
  end,
};

void to_json(json& j, const OutputGroup& p) {
  switch(p) {
    case OutputGroup::start:
      j = "start";
      break;
    case OutputGroup::startCollapsed:
      j = "startCollapsed";
      break;
    case OutputGroup::end:
      j = "end";
      break;
    default:
      throw std::runtime_error {"Unknown OutputGroup"};
  };
}

void from_json(const json& j, OutputGroup& p) {
  std::string s {j.get<std::string>()};
  if(s == "start")
    p = OutputGroup::start;
  else if(s == "startCollapsed")
    p = OutputGroup::startCollapsed;
  else if(s == "end")
    p = OutputGroup::end;
  else
    throw std::runtime_error {"Unknown OutputGroup"};
}


struct OutputEvent : Event {
  std::optional<std::string> category;
  std::string output;
  std::optional<OutputGroup> group;
  std::optional<std::int64_t> variablesReference;
  std::optional<Source> source;
  std::optional<std::int64_t> line;
  std::optional<std::int64_t> column;
  std::optional<json> data;
};

void to_json(json& j, const OutputEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  to_optJson(body, "category", p.category);
  body["output"] = p.output;
  to_optJson(body, "group", p.group);
  to_optJson(body, "variablesReference", p.variablesReference);
  to_optJson(body, "source", p.source);
  to_optJson(body, "line", p.line);
  to_optJson(body, "column", p.column);
  to_optJson(body, "data", p.data);
}

void from_json(const json& j, OutputEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  from_optJson(body, "category", p.category);
  body.at("output").get_to(p.output);
  from_optJson(body, "group", p.group);
  from_optJson(body, "variablesReference", p.variablesReference);
  from_optJson(body, "source", p.source);
  from_optJson(body, "line", p.line);
  from_optJson(body, "column", p.column);
  from_optJson(body, "data", p.data);
}

struct BreakpointEvent : Event {
  std::string reason;
  Breakpoint breakpoint;
};

void to_json(json& j, const BreakpointEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["reason"] = p.reason;
  body["breakpoint"] = p.breakpoint;
}

void from_json(const json& j, BreakpointEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("reason").get_to(p.reason);
  body.at("breakpoint").get_to(p.breakpoint);
}

enum struct ModuleReason {
  new_,
  changed,
  removed,
};

void to_json(json& j, const ModuleReason& p) {
  switch(p) {
    case ModuleReason::new_:
      j = "new";
      break;
    case ModuleReason::changed:
      j = "changed";
      break;
    case ModuleReason::removed:
      j = "removed";
      break;
    default:
      throw std::runtime_error {"Unknown ModuleReason"};
  };
}

void from_json(const json& j, ModuleReason& p) {
  std::string s {j.get<std::string>()};
  if(s == "new")
    p = ModuleReason::new_;
  else if(s == "changed")
    p = ModuleReason::changed;
  else if(s == "removed")
    p = ModuleReason::removed;
  else
    throw std::runtime_error {"Unknown ModuleReason"};
}

struct ModuleEvent : Event {
  ModuleReason reason;
  Module module;
};

void to_json(json& j, const ModuleEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["reason"] = p.reason;
  body["module"] = p.module;
}

void from_json(const json& j, ModuleEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("reason").get_to(p.reason);
  body.at("module").get_to(p.module);
}

using LoadedSourceReason = ModuleReason;

struct LoadedSourceEvent : Event {
  LoadedSourceReason reason;
  Source source;
};

void to_json(json& j, const LoadedSourceEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["reason"] = p.reason;
  body["source"] = p.source;
}

void from_json(const json& j, LoadedSourceEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("reason").get_to(p.reason);
  body.at("source").get_to(p.source);
}

enum struct StartMethod {
  launch,
  attach,
  attachForSuspendedLaunch,
};

void to_json(json& j, const StartMethod& p) {
  switch(p) {
    case StartMethod::launch:
      j = "launch";
      break;
    case StartMethod::attach:
      j = "attach";
      break;
    case StartMethod::attachForSuspendedLaunch:
      j = "attachForSuspendedLaunch";
      break;
    default:
      throw std::runtime_error {"Unknown StartMethod"};
  };
}

void from_json(const json& j, StartMethod& p) {
  std::string s {j.get<std::string>()};
  if(s == "launch")
    p = StartMethod::launch;
  else if(s == "attach")
    p = StartMethod::attach;
  else if(s == "attachForSuspendedLaunch")
    p = StartMethod::attachForSuspendedLaunch;
  else
    throw std::runtime_error {"Unknown StartMethod"};
}

struct ProcessEvent : Event {
  std::string name;
  std::optional<std::int64_t> systemProcessId;
  std::optional<bool> isLocalProcess;
  std::optional<StartMethod> startMethod;
  std::optional<std::int64_t> pointerSize;
};

void to_json(json& j, const ProcessEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["name"] = p.name;
  to_optJson(body, "systemProcessId", p.systemProcessId);
  to_optJson(body, "isLocalProcess", p.isLocalProcess);
  to_optJson(body, "startMethod", p.startMethod);
  to_optJson(body, "pointerSize", p.pointerSize);
}

void from_json(const json& j, ProcessEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("name").get_to(p.name);
  from_optJson(body, "systemProcessId", p.systemProcessId);
  from_optJson(body, "isLocalProcess", p.isLocalProcess);
  from_optJson(body, "startMethod", p.startMethod);
  from_optJson(body, "pointerSize", p.pointerSize);
}

struct CapabilitiesEvent : Event {
  Capabilities capabilities;
};

void to_json(json& j, const CapabilitiesEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["capabilities"] = p.capabilities;
}

void from_json(const json& j, CapabilitiesEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("capabilities").get_to(p.capabilities);
}

struct ProgressStartEvent : Event {
  std::string progressId;
  std::string title;
  std::optional<std::int64_t> requestId;
  std::optional<bool> cancellable;
  std::optional<std::string> message;
  std::optional<std::int64_t> percentage;
};

void to_json(json& j, const ProgressStartEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["progressId"] = p.progressId;
  body["title"] = p.title;
  to_optJson(body, "requestId", p.requestId);
  to_optJson(body, "cancellable", p.cancellable);
  to_optJson(body, "message", p.message);
  to_optJson(body, "percentage", p.percentage);
}

void from_json(const json& j, ProgressStartEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("progressId").get_to(p.progressId);
  body.at("title").get_to(p.title);
  from_optJson(body, "requestId", p.requestId);
  from_optJson(body, "cancellable", p.cancellable);
  from_optJson(body, "message", p.message);
  from_optJson(body, "percentage", p.percentage);
}

struct ProgressUpdateEvent : Event {
  std::string progressId;
  std::optional<std::string> message;
  std::optional<std::int64_t> percentage;
};

void to_json(json& j, const ProgressUpdateEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["progressId"] = p.progressId;
  to_optJson(body, "message", p.message);
  to_optJson(body, "percentage", p.percentage);
}

void from_json(const json& j, ProgressUpdateEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("progressId").get_to(p.progressId);
  from_optJson(body, "message", p.message);
  from_optJson(body, "percentage", p.percentage);
}

struct ProgressEndEvent : Event {
  std::string progressId;
  std::optional<std::string> message;
};

void to_json(json& j, const ProgressEndEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["progressId"] = p.progressId;
  to_optJson(body, "message", p.message);
}

void from_json(const json& j, ProgressEndEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("progressId").get_to(p.progressId);
  from_optJson(body, "message", p.message);
}

struct InvalidatedEvent : Event {
  std::optional<std::string> areas;
  std::optional<std::int64_t> threadId;
  std::optional<std::int64_t> stackFrameId;
};

void to_json(json& j, const InvalidatedEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  to_optJson(body, "areas", p.areas);
  to_optJson(body, "threadId", p.threadId);
  to_optJson(body, "stackFrameId", p.stackFrameId);
}

void from_json(const json& j, InvalidatedEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  from_optJson(body, "areas", p.areas);
  from_optJson(body, "threadId", p.threadId);
  from_optJson(body, "stackFrameId", p.stackFrameId);
}

struct MemoryEvent : Event {
  std::string memoryReference;
  std::int64_t offset;
  std::int64_t count;
};

void to_json(json& j, const MemoryEvent& p) {
  j = static_cast<Event>(p);
  json& body {j["body"]};
  body["memoryReference"] = p.memoryReference;
  body["offset"] = p.offset;
  body["count"] = p.count;
}

void from_json(const json& j, MemoryEvent& p) {
  j.get_to(static_cast<Event&>(p));
  const json& body {j.at("body")};
  body.at("memoryReference").get_to(p.memoryReference);
  body.at("offset").get_to(p.offset);
  body.at("count").get_to(p.count);
}

enum struct RunInTerminalKind {
  integrated,
  external,
};

void to_json(json& j, const RunInTerminalKind& p) {
  switch(p) {
    case RunInTerminalKind::integrated:
      j = "integrated";
      break;
    case RunInTerminalKind::external:
      j = "external";
      break;
    default:
      throw std::runtime_error {"Unknown RunInTerminalKind"};
  };
}

void from_json(const json& j, RunInTerminalKind& p) {
  std::string s {j.get<std::string>()};
  if(s == "integrated")
    p = RunInTerminalKind::integrated;
  else if(s == "external")
    p = RunInTerminalKind::external;
  else
    throw std::runtime_error {"Unknown RunInTerminalKind"};
}

struct RunInTerminalRequest : Request {
  std::optional<RunInTerminalKind> kind;
  std::optional<std::string> title;
  std::string cwd;
  std::vector<std::string> args;
  std::optional<std::map<std::string, std::optional<std::string>>> env;
};

void to_json(json& j, const RunInTerminalRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "kind", p.kind);
  to_optJson(arguments, "title", p.title);
  arguments["cwd"] = p.cwd;
  arguments["args"] = p.args;
  to_optJson(arguments, "env", p.env);
}

void from_json(const json& j, RunInTerminalRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "kind", p.kind);
  from_optJson(arguments, "title", p.title);
  arguments.at("cwd").get_to(p.cwd);
  arguments.at("args").get_to(p.args);
  from_optJson(arguments, "env", p.env);
}

struct RunInTerminalResponse : Response {
  std::optional<std::int64_t> processId;
  std::optional<std::int64_t> shellProcessId;
};

void to_json(json& j, const RunInTerminalResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  to_optJson(body, "processId", p.processId);
  to_optJson(body, "shellProcessId", p.shellProcessId);
}

void from_json(const json& j, RunInTerminalResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  from_optJson(body, "processId", p.processId);
  from_optJson(body, "shellProcessId", p.shellProcessId);
}

struct InitializeRequest : Request {
  std::optional<std::string> clientId;
  std::optional<std::string> clientName;
  std::string adapterId;
  std::optional<std::string> locale;
  std::optional<bool> lineStartAt1;
  std::optional<bool> columnStartAt1;
  std::optional<bool> supportsVariableType;
  std::optional<bool> supportsVariablePaging;
  std::optional<bool> supportsRunInTerminalRequest;
  std::optional<bool> supportsMemoryReferences;
  std::optional<bool> supportsProgressReporting;
  std::optional<bool> supportsInvalidatedEvent;
  std::optional<bool> supportsMemoryEvent;
};

void to_json(json& j, const InitializeRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "clientId", p.clientId);
  to_optJson(arguments, "clientName", p.clientName);
  arguments["adapterId"] = p.adapterId;
  to_optJson(arguments, "locale", p.locale);
  to_optJson(arguments, "lineStartAt1", p.lineStartAt1);
  to_optJson(arguments, "columnStartAt1", p.columnStartAt1);
  to_optJson(arguments, "supportsVariableType", p.supportsVariableType);
  to_optJson(arguments, "supportsVariablePaging", p.supportsVariablePaging);
  to_optJson(arguments, "supportsRunInTerminalRequest",
      p.supportsRunInTerminalRequest);
  to_optJson(arguments, "supportsMemoryReferences", p.supportsMemoryReferences);
  to_optJson(arguments, "supportsProgressReporting",
      p.supportsProgressReporting);
  to_optJson(arguments, "supportsInvalidatedEvent", p.supportsInvalidatedEvent);
  to_optJson(arguments, "supportsMemoryEvent", p.supportsMemoryEvent);
}

void from_json(const json& j, InitializeRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "clientId", p.clientId);
  from_optJson(arguments, "clientName", p.clientName);
  arguments.at("adapterId").get_to(p.adapterId);
  from_optJson(arguments, "locale", p.locale);
  from_optJson(arguments, "lineStartAt1", p.lineStartAt1);
  from_optJson(arguments, "columnStartAt1", p.columnStartAt1);
  from_optJson(arguments, "supportsVariableType", p.supportsVariableType);
  from_optJson(arguments, "supportsVariablePaging", p.supportsVariablePaging);
  from_optJson(arguments, "supportsRunInTerminalRequest",
      p.supportsRunInTerminalRequest);
  from_optJson(arguments, "supportsMemoryReferences",
      p.supportsMemoryReferences);
  from_optJson(arguments, "supportsProgressReporting",
      p.supportsProgressReporting);
  from_optJson(arguments, "supportsInvalidatedEvent",
      p.supportsInvalidatedEvent);
  from_optJson(arguments, "supportsMemoryEvent", p.supportsMemoryEvent);
}

struct InitializeResponse : Response {
  std::optional<Capabilities> body;
};

void to_json(json& j, const InitializeResponse& p) {
  j = static_cast<Response>(p);
  to_optJson(j, "body", p.body);
}

void from_json(const json& j, InitializeResponse& p) {
  j.get_to(static_cast<Response&>(p));
  from_optJson(j, "body", p.body);
}

using ConfigurationDoneRequest = Request;

using ConfigurationDoneResponse = Response;

struct LaunchRequest : Request {
  std::optional<bool> noDebug;
  std::optional<json> __restart;
};

void to_json(json& j, const LaunchRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "noDebug", p.noDebug);
  to_optJson(arguments, "__restart", p.__restart);
}

void from_json(const json& j, LaunchRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "noDebug", p.noDebug);
  from_optJson(arguments, "__restart", p.__restart);
}

using LaunchResponse = Response;

struct AttachRequest : Request {
  std::optional<json> __restart;
};

void to_json(json& j, const AttachRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "__restart", p.__restart);
}

void from_json(const json& j, AttachRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "__restart", p.__restart);
}

using AttachResponse = Response;

struct RestartRequest : Request {
  std::optional<bool> noDebug;
  std::optional<json> __restart;
};

void to_json(json& j, const RestartRequest& p) {
  j = static_cast<Request>(p);
  if(p.noDebug || p.__restart) {
    json& arguments {j["arguments"]};
    to_optJson(arguments, "noDebug", p.noDebug);
    to_optJson(arguments, "__restart", p.__restart);
  }
}

void from_json(const json& j, RestartRequest& p) {
  j.get_to(static_cast<Request&>(p));
  if(auto it {j.find("arguments")}; it != j.end()) {
    from_optJson(*it, "noDebug", p.noDebug);
    from_optJson(*it, "__restart", p.__restart);
  }
}

using RestartResponse = Response;

struct DisconnectRequest : Request {
  std::optional<bool> restart;
  std::optional<bool> terminateDebuggee;
  std::optional<bool> suspendDebuggee;
};

void to_json(json& j, const DisconnectRequest& p) {
  j = static_cast<Request>(p);
  if(p.restart || p.terminateDebuggee || p.suspendDebuggee) {
    json& arguments {j["arguments"]};
    to_optJson(arguments, "restart", p.restart);
    to_optJson(arguments, "terminateDebuggee", p.terminateDebuggee);
    to_optJson(arguments, "suspendDebuggee", p.suspendDebuggee);
  }
}

void from_json(const json& j, DisconnectRequest& p) {
  j.get_to(static_cast<Request&>(p));
  if(auto it {j.find("arguments")}; it != j.end()) {
    from_optJson(*it, "restart", p.restart);
    from_optJson(*it, "terminateDebuggee", p.terminateDebuggee);
    from_optJson(*it, "suspendDebuggee", p.suspendDebuggee);
  }
}

using DisconnectResponse = Response;

struct TerminateRequest : Request {
  std::optional<bool> restart;
};

void to_json(json& j, const TerminateRequest& p) {
  j = static_cast<Request>(p);
  if(p.restart)
    j["arguments"]["restart"] = *p.restart;
}

void from_json(const json& j, TerminateRequest& p) {
  j.get_to(static_cast<Request&>(p));
  if(auto it {j.find("arguments")}; it != j.end())
    from_optJson(*it, "restart", p.restart);
}

using TerminateResponse = Response;

struct BreakpointLocationsRequest : Request {
  Source source;
  std::int64_t line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
};

void to_json(json& j, const BreakpointLocationsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["source"] = p.source;
  arguments["line"] = p.line;
  to_optJson(arguments, "column", p.column);
  to_optJson(arguments, "endLine", p.endLine);
  to_optJson(arguments, "endColumn", p.endColumn);
}

void from_json(const json& j, BreakpointLocationsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("source").get_to(p.source);
  arguments.at("line").get_to(p.line);
  from_optJson(arguments, "column", p.column);
  from_optJson(arguments, "endLine", p.endLine);
  from_optJson(arguments, "endColumn", p.endColumn);
}

struct BreakpointLocationsResponse : Response {
  std::vector<BreakpointLocation> breakpoints;
};

void to_json(json& j, const BreakpointLocationsResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["breakpoints"] = p.breakpoints;
}

void from_json(const json& j, BreakpointLocationsResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("breakpoints").get_to(p.breakpoints);
}

struct SetFunctionBreakpointsRequest : Request {
  std::vector<FunctionBreakpoint> breakpoints;
};

void to_json(json& j, const SetFunctionBreakpointsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["breakpoints"] = p.breakpoints;
}

void from_json(const json& j, SetFunctionBreakpointsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("breakpoints").get_to(p.breakpoints);
}

struct SetFunctionBreakpointsResponse : Response {
  std::vector<Breakpoint> breakpoints;
};

void to_json(json& j, const SetFunctionBreakpointsResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["breakpoints"] = p.breakpoints;
}

void from_json(const json& j, SetFunctionBreakpointsResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("breakpoints").get_to(p.breakpoints);
}

struct SetExceptionBreakpointsRequest : Request {
  std::vector<std::string> filters;
  std::optional<std::vector<ExceptionFilterOptions>> filterOptions;
  std::optional<std::vector<ExceptionOptions>> exceptionOptions;
};

void to_json(json& j, const SetExceptionBreakpointsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["filters"] = p.filters;
  to_optJson(arguments, "filterOptions", p.filterOptions);
  to_optJson(arguments, "exceptionOptions", p.exceptionOptions);
}

void from_json(const json& j, SetExceptionBreakpointsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("filters").get_to(p.filters);
  from_optJson(arguments, "filterOptions", p.filterOptions);
  from_optJson(arguments, "exceptionOptions", p.exceptionOptions);
}

struct SetExceptionBreakpointsResponse : Response {
  std::optional<std::vector<Breakpoint>> breakpoints;
};

void to_json(json& j, const SetExceptionBreakpointsResponse& p) {
  j = static_cast<Response>(p);
  if(p.breakpoints)
    j["body"]["breakpoints"] = *p.breakpoints;
}

void from_json(const json& j, SetExceptionBreakpointsResponse& p) {
  j.get_to(static_cast<Response&>(p));
  if(auto it {j.find("body")}; it != j.end())
    from_optJson(*it, "breakpoints", p.breakpoints);
}

struct DataBreakpointInfoRequest : Request {
  std::optional<std::int64_t> variablesReference;
  std::string name;
};

void to_json(json& j, const DataBreakpointInfoRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "variablesReference", p.variablesReference);
  arguments["name"] = p.name;
}

void from_json(const json& j, DataBreakpointInfoRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "variablesReference", p.variablesReference);
  arguments.at("name").get_to(p.name);
}

struct DataBreakpointInfoResponse : Response {
  std::optional<std::string> dataId;
  std::string description;
  std::optional<std::vector<DataBreakpointAccessType>> accessTypes;
  std::optional<bool> canPersist;
};

void to_json(json& j, const DataBreakpointInfoResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  if(p.dataId)
    body["dataId"] = *p.dataId;
  else
    body["dataId"] = nullptr;
  body["description"] = p.description;
  to_optJson(body, "accessTypes", p.accessTypes);
  to_optJson(body, "canPersist", p.canPersist);
}

void from_json(const json& j, DataBreakpointInfoResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  if(auto dataId {body.at("dataId")}; !dataId.is_null())
    p.dataId = dataId;
  body.at("description").get_to(p.description);
  from_optJson(body, "accessTypes", p.accessTypes);
  from_optJson(body, "canPersist", p.canPersist);
}

struct SetDataBreakpointsRequest : Request {
  std::vector<DataBreakpoint> breakpoints;
};

void to_json(json& j, const SetDataBreakpointsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["breakpoints"] = p.breakpoints;
}

void from_json(const json& j, SetDataBreakpointsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("breakpoints").get_to(p.breakpoints);
}

using SetDataBreakpointsResponse = SetFunctionBreakpointsResponse;

struct SetInstructionBreakpointsRequest : Request {
  std::vector<InstructionBreakpoint> breakpoints;
};

void to_json(json& j, const SetInstructionBreakpointsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["breakpoints"] = p.breakpoints;
}

void from_json(const json& j, SetInstructionBreakpointsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("breakpoints").get_to(p.breakpoints);
}

using SetInstructionBreakpointsResponse = SetFunctionBreakpointsResponse;

struct ContinueRequest : Request {
  std::int64_t threadId;
  std::optional<bool> singleThread;
};

void to_json(json& j, const ContinueRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["threadId"] = p.threadId;
  to_optJson(arguments, "singleThread", p.singleThread);
}

void from_json(const json& j, ContinueRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("threadId").get_to(p.threadId);
  from_optJson(arguments, "singleThread", p.singleThread);
}

struct ContinueResponse : Response {
  std::optional<bool> allThreadsContinued;
};

void to_json(json& j, const ContinueResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  to_optJson(body, "allThreadsContinued", p.allThreadsContinued);
}

void from_json(const json& j, ContinueResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  from_optJson(body, "allThreadsContinued", p.allThreadsContinued);
}

struct NextRequest : Request {
  std::int64_t threadId;
  std::optional<bool> singleThread;
  std::optional<SteppingGranularity> granularity;
};

void to_json(json& j, const NextRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["threadId"] = p.threadId;
  to_optJson(arguments, "singleThread", p.singleThread);
  to_optJson(arguments, "granularity", p.granularity);
}

void from_json(const json& j, NextRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("threadId").get_to(p.threadId);
  from_optJson(arguments, "singleThread", p.singleThread);
  from_optJson(arguments, "granularity", p.granularity);
}

using NextResponse = Response;

struct StepInRequest : Request {
  int64_t threadId;
  std::optional<bool> singleThread;
  std::optional<std::int64_t> targetId;
  std::optional<SteppingGranularity> granularity;
};

void to_json(json& j, const StepInRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["threadId"] = p.threadId;
  to_optJson(arguments, "singleThread", p.singleThread);
  to_optJson(arguments, "targetId", p.targetId);
  to_optJson(arguments, "granularity", p.granularity);
}

void from_json(const json& j, StepInRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("threadId").get_to(p.threadId);
  from_optJson(arguments, "singleThread", p.singleThread);
  from_optJson(arguments, "targetId", p.targetId);
  from_optJson(arguments, "granularity", p.granularity);
}

using StepInResponse = Response;

using StepOutRequest = NextRequest;

using StepOutResponse = Response;

using StepBackRequest = NextRequest;

using StepBackResponse = Response;

using ReverseContinueRequest = ContinueRequest;

using ReverseContinueResponse = Response;

struct RestartFrameRequest : Request {
  std::int64_t frameId;
};

void to_json(json& j, const RestartFrameRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["frameId"] = p.frameId;
}

void from_json(const json& j, RestartFrameRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("frameId").get_to(p.frameId);
}

using RestartFrameResponse = Response;

struct GotoRequest : Request {
  std::int64_t threadId;
  std::int64_t targetId;
};

void to_json(json& j, const GotoRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["threadId"] = p.threadId;
  arguments["targetId"] = p.targetId;
}

void from_json(const json& j, GotoRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("threadId").get_to(p.threadId);
  arguments.at("targetId").get_to(p.targetId);
}

using GotoResponse = Response;

struct PauseRequest : Request {
  std::int64_t threadId;
};

void to_json(json& j, const PauseRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["threadId"] = p.threadId;
}

void from_json(const json& j, PauseRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("threadId").get_to(p.threadId);
}

using PauseResponse = Response;

struct StackTraceRequest : Request {
  std::int64_t threadId;
  std::optional<std::int64_t> startFrame;
  std::optional<std::int64_t> levels;
  std::optional<StackFrameFormat> format;
};

void to_json(json& j, const StackTraceRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["threadId"] = p.threadId;
  to_optJson(arguments, "startFrame", p.startFrame);
  to_optJson(arguments, "levels", p.levels);
  to_optJson(arguments, "format", p.format);
}

void from_json(const json& j, StackTraceRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("threadId").get_to(p.threadId);
  from_optJson(arguments, "startFrame", p.startFrame);
  from_optJson(arguments, "levels", p.levels);
  from_optJson(arguments, "format", p.format);
}


struct StackTraceResponse : Response {
  std::vector<StackFrame> stackFrames;
  std::optional<std::int64_t> totalFrames;
};

void to_json(json& j, const StackTraceResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["stackFrames"] = p.stackFrames;
  to_optJson(body, "totalFrames", p.totalFrames);
}

void from_json(const json& j, StackTraceResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("stackFrames").get_to(p.stackFrames);
  from_optJson(body, "totalFrames", p.totalFrames);
}

using ScopesRequest = RestartFrameRequest;

struct ScopesResponse : Response {
  std::vector<Scope> scopes;
};

void to_json(json& j, const ScopesResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["scopes"] = p.scopes;
}

void from_json(const json& j, ScopesResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("scopes").get_to(p.scopes);
}

enum struct VariablesFilter {
  indexed,
  named,
};

void to_json(json& j, const VariablesFilter& p) {
  switch(p) {
    case VariablesFilter::indexed:
      j = "indexed";
      break;
    case VariablesFilter::named:
      j = "named";
      break;
    default:
      throw std::runtime_error {"Unknown VariablesFilter"};
  };
}

void from_json(const json& j, VariablesFilter& p) {
  std::string s {j.get<std::string>()};
  if(s == "indexed")
    p = VariablesFilter::indexed;
  else if(s == "named")
    p = VariablesFilter::named;
  else
    throw std::runtime_error {"Unknown VariablesFilter"};
}


struct VariablesRequest : Request {
  std::int64_t variablesReference;
  std::optional<VariablesFilter> filter;
  std::optional<std::int64_t> start;
  std::optional<std::int64_t> count;
  std::optional<ValueFormat> format;
};

void to_json(json& j, const VariablesRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["variablesReference"] = p.variablesReference;
  to_optJson(arguments, "filter", p.filter);
  to_optJson(arguments, "start", p.start);
  to_optJson(arguments, "count", p.count);
  to_optJson(arguments, "format", p.format);
}

void from_json(const json& j, VariablesRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("variablesReference").get_to(p.variablesReference);
  from_optJson(arguments, "filter", p.filter);
  from_optJson(arguments, "start", p.start);
  from_optJson(arguments, "count", p.count);
  from_optJson(arguments, "format", p.format);
}

struct VariablesResponse : Response {
  std::vector<Variable> variables;
};

void to_json(json& j, const VariablesResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["variables"] = p.variables;
}

void from_json(const json& j, VariablesResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("variables").get_to(p.variables);
}

struct SetVariableRequest : Request {
  std::int64_t variablesReference;
  std::string name;
  std::string value;
  std::optional<ValueFormat> format;
};

void to_json(json& j, const SetVariableRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["variablesReference"] = p.variablesReference;
  arguments["name"] = p.name;
  arguments["value"] = p.value;
  to_optJson(arguments, "format", p.format);
}

void from_json(const json& j, SetVariableRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("variablesReference").get_to(p.variablesReference);
  arguments.at("name").get_to(p.name);
  arguments.at("value").get_to(p.value);
  from_optJson(arguments, "format", p.format);
}

struct SetVariableResponse : Response {
  std::string value;
  std::optional<std::string> type;
  std::optional<std::int64_t> variablesReference;
  std::optional<std::int64_t> namedVariables;
  std::optional<std::int64_t> indexedVariables;
};

void to_json(json& j, const SetVariableResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["value"] = p.value;
  to_optJson(body, "type", p.type);
  to_optJson(body, "variablesReference", p.variablesReference);
  to_optJson(body, "namedVariables", p.namedVariables);
  to_optJson(body, "indexedVariables", p.indexedVariables);
}

void from_json(const json& j, SetVariableResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("value").get_to(p.value);
  from_optJson(body, "type", p.type);
  from_optJson(body, "variablesReference", p.variablesReference);
  from_optJson(body, "namedVariables", p.namedVariables);
  from_optJson(body, "indexedVariables", p.indexedVariables);
}

struct SourceRequest : Request {
  std::optional<Source> source;
  std::int64_t sourceReference;
};

void to_json(json& j, const SourceRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "source", p.source);
  arguments["sourceReference"] = p.sourceReference;
}

void from_json(const json& j, SourceRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "source", p.source);
  arguments.at("sourceReference").get_to(p.sourceReference);
}

struct SourceResponse : Response {
  std::string content;
  std::optional<std::string> mimeType;
};

void to_json(json& j, const SourceResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["content"] = p.content;
  to_optJson(body, "mimeType", p.mimeType);
}

void from_json(const json& j, SourceResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("content").get_to(p.content);
  from_optJson(body, "mimeType", p.mimeType);
}

using ThreadsRequest = Request;

struct ThreadsResponse : Response {
  std::vector<Thread> threads;
};

void to_json(json& j, const ThreadsResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["threads"] = p.threads;
}

void from_json(const json& j, ThreadsResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("threads").get_to(p.threads);
}

struct TerminateThreadsRequest : Request {
  std::optional<std::vector<std::int64_t>> threadIds;
};

void to_json(json& j, const TerminateThreadsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "threadIds", p.threadIds);
}

void from_json(const json& j, TerminateThreadsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "threadIds", p.threadIds);
}

using TerminateThreadsResponse = Response;

struct ModulesRequest : Request {
  std::optional<std::int64_t> startModule;
  std::optional<std::int64_t> moduleCount;
};

void to_json(json& j, const ModulesRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "startModule", p.startModule);
  to_optJson(arguments, "moduleCount", p.moduleCount);
}

void from_json(const json& j, ModulesRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "startModule", p.startModule);
  from_optJson(arguments, "moduleCount", p.moduleCount);
}

struct ModulesResponse : Response {
  std::vector<Module> modules;
  std::optional<std::int64_t> totalModules;
};

void to_json(json& j, const ModulesResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["modules"] = p.modules;
  to_optJson(body, "totalModules", p.totalModules);
}

void from_json(const json& j, ModulesResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("modules").get_to(p.modules);
  from_optJson(body, "totalModules", p.totalModules);
}

using LoadedSourcesRequest = Request;

struct LoadedSourcesResponse : Response {
  std::vector<Source> sources;
};

void to_json(json& j, const LoadedSourcesResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["sources"] = p.sources;
}

void from_json(const json& j, LoadedSourcesResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("sources").get_to(p.sources);
}

struct EvaluateRequest : Request {
  std::string expression;
  std::optional<std::int64_t> frameId;
  std::optional<std::string> context;
  std::optional<ValueFormat> format;
};

void to_json(json& j, const EvaluateRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["expression"] = p.expression;
  to_optJson(arguments, "frameId", p.frameId);
  to_optJson(arguments, "context", p.context);
  to_optJson(arguments, "format", p.format);
}

void from_json(const json& j, EvaluateRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("expression").get_to(p.expression);
  from_optJson(arguments, "frameId", p.frameId);
  from_optJson(arguments, "context", p.context);
  from_optJson(arguments, "format", p.format);
}

struct EvaluateResponse : Response {
  std::string result;
  std::optional<std::string> type;
  std::optional<VariablePresentationHint> presentationHint;
  std::int64_t variablesReference;
  std::optional<std::int64_t> namedVariables;
  std::optional<std::int64_t> indexedVariables;
  std::optional<std::string> memoryReference;
};

void to_json(json& j, const EvaluateResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["result"] = p.result;
  to_optJson(body, "type", p.type);
  to_optJson(body, "presentationHint", p.presentationHint);
  body["variablesReference"] = p.variablesReference;
  to_optJson(body, "namedVariables", p.namedVariables);
  to_optJson(body, "indexedVariables", p.indexedVariables);
  to_optJson(body, "memoryReference", p.memoryReference);
}

void from_json(const json& j, EvaluateResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("result").get_to(p.result);
  from_optJson(body, "type", p.type);
  from_optJson(body, "presentationHint", p.presentationHint);
  body.at("variablesReference").get_to(p.variablesReference);
  from_optJson(body, "namedVariables", p.namedVariables);
  from_optJson(body, "indexedVariables", p.indexedVariables);
  from_optJson(body, "memoryReference", p.memoryReference);
}

struct SetExpressionRequest : Request {
  std::string expression;
  std::string value;
  std::optional<std::int64_t> frameId;
  std::optional<ValueFormat> format;
};

void to_json(json& j, const SetExpressionRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["expression"] = p.expression;
  arguments["value"] = p.value;
  to_optJson(arguments, "frameId", p.frameId);
  to_optJson(arguments, "format", p.format);
}

void from_json(const json& j, SetExpressionRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("expression").get_to(p.expression);
  arguments.at("value").get_to(p.value);
  from_optJson(arguments, "frameId", p.frameId);
  from_optJson(arguments, "format", p.format);
}

struct SetExpressionResponse : Response {
  std::string value;
  std::optional<std::string> type;
  std::optional<VariablePresentationHint> presentationHint;
  std::optional<std::int64_t> variablesReference;
  std::optional<std::int64_t> namedVariables;
  std::optional<std::int64_t> indexedVariables;
};

void to_json(json& j, const SetExpressionResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["value"] = p.value;
  to_optJson(body, "type", p.type);
  to_optJson(body, "presentationHint", p.presentationHint);
  to_optJson(body, "variablesReference", p.variablesReference);
  to_optJson(body, "namedVariables", p.namedVariables);
  to_optJson(body, "indexedVariables", p.indexedVariables);
}

void from_json(const json& j, SetExpressionResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("value").get_to(p.value);
  from_optJson(body, "type", p.type);
  from_optJson(body, "presentationHint", p.presentationHint);
  from_optJson(body, "variablesReference", p.variablesReference);
  from_optJson(body, "namedVariables", p.namedVariables);
  from_optJson(body, "indexedVariables", p.indexedVariables);
}

struct StepInTargetsRequest : Request {
  std::int64_t frameId;
};

void to_json(json& j, const StepInTargetsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["frameId"] = p.frameId;
}

void from_json(const json& j, StepInTargetsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("frameId").get_to(p.frameId);
}

struct StepInTargetsResponse : Response {
  std::vector<StepInTarget> targets;
};

void to_json(json& j, const StepInTargetsResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["targets"] = p.targets;
}

void from_json(const json& j, StepInTargetsResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("targets").get_to(p.targets);
}

struct GotoTargetsRequest : Request {
  Source source;
  std::int64_t line;
  std::optional<std::int64_t> column;
};

void to_json(json& j, const GotoTargetsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["source"] = p.source;
  arguments["line"] = p.line;
  to_optJson(arguments, "column", p.column);
}

void from_json(const json& j, GotoTargetsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("source").get_to(p.source);
  arguments.at("line").get_to(p.line);
  from_optJson(arguments, "column", p.column);
}

struct GotoTargetsResponse : Response {
  std::vector<GotoTarget> targets;
};

void to_json(json& j, const GotoTargetsResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["targets"] = p.targets;
}

void from_json(const json& j, GotoTargetsResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("targets").get_to(p.targets);
}

struct CompletionsRequest : Request {
  std::optional<std::int64_t> frameId;
  std::string text;
  std::int64_t column;
  std::optional<std::int64_t> line;
};

void to_json(json& j, const CompletionsRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  to_optJson(arguments, "frameId", p.frameId);
  arguments["text"] = p.text;
  arguments["column"] = p.column;
  to_optJson(arguments, "line", p.line);
}

void from_json(const json& j, CompletionsRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  from_optJson(arguments, "frameId", p.frameId);
  arguments.at("text").get_to(p.text);
  arguments.at("column").get_to(p.column);
  from_optJson(arguments, "line", p.line);
}

struct CompletionsResponse : Response {
  std::vector<CompletionItem> targets;
};

void to_json(json& j, const CompletionsResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["targets"] = p.targets;
}

void from_json(const json& j, CompletionsResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("targets").get_to(p.targets);
}

using ExceptionInfoRequest = PauseRequest;

struct ExceptionInfoResponse : Response {
  std::string exceptionId;
  std::optional<std::string> description;
  ExceptionBreakMode breakMode;
  std::optional<ExceptionDetails> details;
};

void to_json(json& j, const ExceptionInfoResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["exceptionId"] = p.exceptionId;
  to_optJson(body, "description", p.description);
  body["breakMode"] = p.breakMode;
  to_optJson(body, "details", p.details);
}

void from_json(const json& j, ExceptionInfoResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("exceptionId").get_to(p.exceptionId);
  from_optJson(body, "description", p.description);
  body.at("breakMode").get_to(p.breakMode);
  from_optJson(body, "details", p.details);
}

struct ReadMemoryRequest : Request {
  std::string memoryReference;
  std::optional<std::int64_t> offset;
  std::int64_t count;
};

void to_json(json& j, const ReadMemoryRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["memoryReference"] = p.memoryReference;
  to_optJson(arguments, "offset", p.offset);
  arguments["count"] = p.count;
}

void from_json(const json& j, ReadMemoryRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("memoryReference").get_to(p.memoryReference);
  from_optJson(arguments, "offset", p.offset);
  arguments.at("count").get_to(p.count);
}

struct ReadMemoryResponse : Response {
  std::string address;
  std::optional<std::int64_t> unreadableBytes;
  std::optional<std::string> data;
};

void to_json(json& j, const ReadMemoryResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["address"] = p.address;
  to_optJson(body, "unreadableBytes", p.unreadableBytes);
  to_optJson(body, "data", p.data);
}

void from_json(const json& j, ReadMemoryResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("address").get_to(p.address);
  from_optJson(body, "unreadableBytes", p.unreadableBytes);
  from_optJson(body, "data", p.data);
}

struct WriteMemoryRequest : Request {
  std::string memoryReference;
  std::optional<std::int64_t> offset;
  std::optional<bool> allowPartial;
  std::string data;
};

void to_json(json& j, const WriteMemoryRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["memoryReference"] = p.memoryReference;
  to_optJson(arguments, "offset", p.offset);
  to_optJson(arguments, "allowPartial", p.allowPartial);
  arguments["data"] = p.data;
}

void from_json(const json& j, WriteMemoryRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("memoryReference").get_to(p.memoryReference);
  from_optJson(arguments, "offset", p.offset);
  from_optJson(arguments, "allowPartial", p.allowPartial);
  arguments.at("data").get_to(p.data);
}

struct WriteMemoryResponse : Response {
  std::optional<std::int64_t> offset;
  std::optional<std::int64_t> bytesWritten;
};

void to_json(json& j, const WriteMemoryResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  to_optJson(body, "offset", p.offset);
  to_optJson(body, "bytesWritten", p.bytesWritten);
}

void from_json(const json& j, WriteMemoryResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  from_optJson(body, "offset", p.offset);
  from_optJson(body, "bytesWritten", p.bytesWritten);
}

struct DisassembleRequest : Request {
  std::string memoryReference;
  std::optional<std::int64_t> offset;
  std::optional<std::int64_t> instructionOffset;
  std::int64_t instructionCount;
  std::optional<bool> resolveSymbols;
};

void to_json(json& j, const DisassembleRequest& p) {
  j = static_cast<Request>(p);
  json& arguments {j["arguments"]};
  arguments["memoryReference"] = p.memoryReference;
  to_optJson(arguments, "offset", p.offset);
  to_optJson(arguments, "instructionOffset", p.instructionOffset);
  arguments["instructionCount"] = p.instructionCount;
  to_optJson(arguments, "resolveSymbols", p.resolveSymbols);
}

void from_json(const json& j, DisassembleRequest& p) {
  j.get_to(static_cast<Request&>(p));
  const json& arguments {j.at("arguments")};
  arguments.at("memoryReference").get_to(p.memoryReference);
  from_optJson(arguments, "offset", p.offset);
  from_optJson(arguments, "instructionOffset", p.instructionOffset);
  arguments.at("instructionCount").get_to(p.instructionCount);
  from_optJson(arguments, "resolveSymbols", p.resolveSymbols);
}

struct DisassembleResponse : Response {
  std::vector<DisassembledInstruction> instructions;
};

void to_json(json& j, const DisassembleResponse& p) {
  j = static_cast<Response>(p);
  json& body {j["body"]};
  body["instructions"] = p.instructions;
}

void from_json(const json& j, DisassembleResponse& p) {
  j.get_to(static_cast<Response&>(p));
  const json& body {j.at("body")};
  body.at("instructions").get_to(p.instructions);
}

} // namespace dap

#endif // VITO_DAP_HPP
