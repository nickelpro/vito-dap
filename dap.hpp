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
  unknown,
  request,
  response,
  event,
};

struct ProtocolMessage {
  std::int64_t seq;
  MessageType type;
};

enum struct CommandType {
  unknown,
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

struct Request : ProtocolMessage {
  CommandType command;
};

enum struct EventType {
  unknown,
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

struct Event : ProtocolMessage {
  EventType event;
};

struct Response : ProtocolMessage {
  std::int64_t request_seq;
  bool success;
  CommandType command;
  std::optional<std::string> message;
};

struct ErrorResponse : Response {
  std::optional<std::string> error;
};

struct CancelArguments {
  std::optional<std::int64_t> requestId;
  std::optional<std::string> progressId;
};

struct CancelRequest : Request {
  std::optional<CancelArguments> arguments;
};

struct CancelResponse : Response {};

struct InitializedEvent : Event {};

struct StoppedEvent : Event {
  std::string reason;
  std::optional<std::string> description;
  std::optional<std::int64_t> threadId;
  std::optional<bool> preserveFocusHint;
  std::optional<std::string> text;
  std::optional<bool> allThreadsStopped;
  std::optional<std::vector<std::int64_t>> hitBreakpointIds;
};

struct ContinuedEvent : Event {
  std::int64_t threadId;
  std::optional<bool> allThreadsContinued;
};

struct ExitedEvent : Event {};

struct TerminatedEvent : Event {
  std::optional<json> restart;
};

struct ThreadEvent : Event {
  std::string reason;
  std::int64_t threadId;
};

enum struct OutputGroup {
  unknown,
  start,
  startCollapsed,
  end,
};

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

struct BreakpointEvent : Event {
  std::string reason;
  Breakpoint breakpoint;
};

enum struct ModuleReason {
  unknown,
  new_,
  changed,
  removed,
};

struct ModuleEvent : Event {
  ModuleReason reason;
  Module module;
};

struct LoadedSourceEvent : Event {
  ModuleReason reason;
  Source source;
};

enum struct StartMethod {
  unknown,
  launch,
  attach,
  attachForSuspendedLaunch,
};

struct ProcessEvent : Event {
  std::string name;
  std::optional<std::int64_t> systemProcessId;
  std::optional<bool> isLocalProcess;
  std::optional<StartMethod> startMethod;
  std::optional<std::int64_t> pointerSize;
};

struct CapabilitiesEvent : Event {
  Capabilities capabilities;
};

struct ProgressStartEvent : Event {
  std::string progressId;
  std::string title;
  std::optional<std::int64_t> requestId;
  std::optional<bool> cancellable;
  std::optional<std::string> message;
  std::optional<std::int64_t> percentage;
};

struct ProgressUpdateEvent : Event {
  std::string progressId;
  std::optional<std::string> message;
  std::optional<std::int64_t> percentage;
};

struct ProgressEndEvent : Event {
  std::string progressId;
  std::optional<std::string> message;
};

struct InvalidatedEvent : Event {
  std::optional<std::string> areas;
  std::optional<std::int64_t> threadId;
  std::optional<std::int64_t> stackFrameId;
};

struct MemoryEvent : Event {
  std::string memoryReference;
  std::int64_t offset;
  std::int64_t count;
};

enum struct RunInTerminalKind {
  unknown,
  integrated,
  external,
};

struct RunInTerminalRequest : Request {
  std::optional<RunInTerminalKind> kind;
  std::optional<std::string> title;
  std::string cwd;
  std::vector<std::string> args;
  std::optional<std::map<std::string, std::optional<std::string>>> env;
};

struct RunInTerminalResponse : Response {
  std::optional<std::int64_t> processId;
  std::optional<std::int64_t> shellProcessId;
};

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

struct InitializeResponse : Response {
  Capabilities body;
};

struct ConfigurationDoneRequest : Request {};

struct ConfigurationDoneResponse : Response {};

struct LaunchRequest : Request {
  std::optional<bool> noDebug;
  std::optional<json> __restart;
};

struct LaunchResponse : Response {};

struct AttachRequest : Request {
  std::optional<json> __restart;
};

struct AttachResponse : Response {};

struct RestartRequest : Request {
  std::optional<bool> noDebug;
  std::optional<json> __restart;
};

struct RestartResponse : Response {};

struct DisconnectRequest : Request {
  std::optional<bool> restart;
  std::optional<bool> terminateDebuggee;
  std::optional<bool> suspendDebuggee;
};

struct DisconnectResponse : Response {};

struct TerminateRequest : Request {
  std::optional<bool> restart;
};

struct TerminateResponse : Response {};

struct BreakpointLocationsRequest : Request {
  Source source;
  std::int64_t line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
};

struct BreakpointLocationsRespone : Response {
  std::vector<BreakpointLocation> breakpoints;
};

struct SetFunctionBreakpointsRequest : Request {
  std::vector<FunctionBreakpoint> breakpoints;
};

struct SetFunctionBreakpointsResponse : Response {
  std::vector<Breakpoint> breakpoints;
};

struct SetExceptionBreakpointsRequest : Request {
  std::vector<std::string> filters;
  std::optional<std::vector<ExceptionFilterOptions>> filterOptions;
  std::optional<std::vector<ExceptionOptions>> exceptionOptions;
};

struct SetExceptionBreakpointsResponse : Response {
  std::vector<Breakpoint> breakpoints;
};

struct DataBreakpointInfoRequest : Request {
  std::optional<std::int64_t> variablesReference;
  std::string name;
};

struct DataBreakpointInfoResponse : Response {
  std::variant<std::string, nullptr_t> dataId;
  std::string description;
  std::optional<std::vector<DataBreakpointAccessType>> accessTypes;
  std::optional<bool> canPersist;
};

struct SetDataBreakpointsRequest : Request {
  std::vector<DataBreakpoint> breakpoints;
};

struct SetDataBreakpointsResponse : Response {
  std::vector<Breakpoint> breakpoints;
};

struct SetInstructionBreakpointsRequest : Request {
  std::vector<InstructionBreakpoint> breakpoints;
};

struct SetInstructionBreakpointsResponse : Response {
  std::vector<Breakpoint> breakpoints;
};

struct ContinueRequest : Request {
  std::int64_t threadId;
  std::optional<bool> singleThread;
};

struct ContinueResponse : Response {
  std::optional<bool> allThreadsContinued;
};

struct NextRequest : Request {
  std::int64_t threadId;
  std::optional<bool> singleThread;
  std::optional<SteppingGranularity> granularity;
};

struct NextResponse : Response {};

struct StepInRequest : Request {
  int64_t threadId;
  std::optional<bool> singleThread;
  std::optional<std::int64_t> targetId;
  std::optional<SteppingGranularity> granularity;
};

struct StepInResponse : Response {};

struct StepOutRequest : Request {
  std::int64_t threadId;
  std::optional<bool> singleThread;
  std::optional<SteppingGranularity> granularity;
};

struct StepOutResponse : Response {};

struct StepBackRequest : Request {
  std::int64_t threadId;
  std::optional<bool> singleThread;
  std::optional<bool> granularity;
};

struct StepBackResponse : Response {};

struct ReverseContinueRequest : Request {
  std::int64_t threadId;
  std::optional<bool> singleThread;
};

struct ReverseContinueResponse : Response {};

struct RestartFrameRequest : Request {
  std::int64_t frameId;
};

struct RestartFrameResponse : Response {};

struct GotoRequest : Request {
  std::int64_t threadId;
  std::int64_t targetId;
};

struct GotoResponse : Response {};

struct PauseRequest : Request {
  std::int64_t threadId;
};

struct PauseResponse : Response {};

struct StackTraceRequest : Request {
  std::int64_t threadId;
  std::optional<std::int64_t> startFrame;
  std::optional<std::int64_t> levels;
  std::optional<StackFrameFormat> format;
};

struct StackTraceResponse : Response {
  std::vector<StackFrame> stackFrames;
  std::optional<std::int64_t> totalFrames;
};

struct ScopesRequest : Request {
  std::int64_t frameId;
};

struct ScopesResponse : Response {
  std::vector<Scope> scopes;
};

enum struct VariablesFilter {
  unknown,
  indexed,
  named,
};

struct VariablesRequest : Request {
  std::int64_t variablesReference;
  std::optional<VariablesFilter> filter;
  std::optional<std::int64_t> start;
  std::optional<std::int64_t> count;
  std::optional<ValueFormat> format;
};

struct VariablesResponse : Response {
  std::vector<Variable> variables;
};

struct SetVariableRequest : Request {
  std::int64_t variablesReference;
  std::string name;
  std::string value;
  std::optional<ValueFormat> format;
};

struct SetVariableResponse : Response {
  std::string value;
  std::optional<std::string> type;
  std::optional<std::int64_t> variablesReference;
  std::optional<std::int64_t> namedVariables;
  std::optional<std::int64_t> indexedVariables;
};

struct SourceRequest : Request {
  std::optional<Source> source;
  std::int64_t sourceReference;
};

struct SourceResponse : Response {
  std::string content;
  std::optional<std::string> mimeType;
};

struct ThreadsRequest : Request {};

struct ThreadsResponse : Response {
  std::vector<Thread> threads;
};

struct ModulesRequest : Request {
  std::optional<std::int64_t> startModule;
  std::optional<std::int64_t> moduleCount;
};

struct ModulesResponse : Response {
  std::vector<Module> modules;
  std::optional<std::int64_t> totalModules;
};

struct LoadedSourcesRequest : Request {};

struct LoadedSourcesResponse : Response {
  std::vector<Source> sources;
};

struct EvaluateRequest : Request {
  std::string expression;
  std::optional<std::int64_t> frameId;
  std::optional<std::string> context;
  std::optional<ValueFormat> format;
};

struct EvaluateResponse : Response {
  std::string result;
  std::optional<std::string> type;
  std::optional<VariablePresentationHint> presentationHint;
  std::int64_t variablesReference;
  std::optional<std::int64_t> namedVariables;
  std::optional<std::int64_t> indexedVariables;
  std::optional<std::string> memoryReference;
};

struct SetExpressionRequest : Request {
  std::string expression;
  std::string value;
  std::optional<std::int64_t> frameId;
  std::optional<ValueFormat> format;
};

struct SetExpressionResponse : Response {
  std::string value;
  std::optional<std::string> type;
  std::optional<VariablePresentationHint> presentationHint;
  std::optional<std::int64_t> variablesReference;
  std::optional<std::int64_t> namedVariables;
  std::optional<std::int64_t> indexedVariables;
};

struct StepInTargetsRequest : Request {
  std::int64_t frameId;
};

struct StepInTargetsResponse : Response {
  std::vector<StepInTarget> targets;
};

struct GotoTargetsRequest : Request {
  Source source;
  std::int64_t line;
  std::optional<std::int64_t> column;
};

struct GotoTargetsResponse : Response {
  std::vector<GotoTarget> targets;
};

struct CompletionsRequest : Request {
  std::optional<std::int64_t> frameId;
  std::string text;
  std::int64_t column;
  std::optional<std::int64_t> line;
};

struct CompletionsResponse : Response {
  std::vector<CompletionItem> targets;
};

struct ExceptionInfoRequest : Request {
  std::int64_t threadId;
};

struct ExceptionInfoResponse : Response {
  std::string exceptionId;
  std::optional<std::string> description;
  ExceptionBreakMode breakMode;
  std::optional<ExceptionDetails> details;
};

struct ReadMemoryRequest : Request {
  std::string memoryReference;
  std::optional<std::int64_t> offset;
  std::int64_t count;
};

struct ReadMemoryResponse : Response {
  std::string address;
  std::optional<std::int64_t> unreadableBytes;
  std::optional<std::string> data;
};

struct WriteMemoryRequest : Request {
  std::string memoryReference;
  std::optional<std::int64_t> offset;
  std::optional<bool> allowPartial;
  std::string data;
};

struct WriteMemoryResponse : Response {
  std::optional<std::int64_t> offset;
  std::optional<std::int64_t> bytesWritten;
};

struct DisassembleRequest : Request {
  std::string memoryReference;
  std::optional<std::int64_t> offset;
  std::optional<std::int64_t> instructionOffset;
  std::int64_t instructionCount;
  std::optional<bool> resolveSymbols;
};

struct DisassembleResponse : Response {
  std::vector<DisassembledInstruction> instructions;
};

} // namespace dap

#endif // VITO_DAP_HPP
