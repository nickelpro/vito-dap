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
  j = json {{"filter", p.filter}, {"label", p.label}};
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
  j = json {{"filterId", p.filterId}};
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
  j = json {{"name", p.name}};
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
      j = json("never");
      break;
    case ExceptionBreakMode::always:
      j = json("always");
      break;
    case ExceptionBreakMode::unhandled:
      j = json("unhandled");
      break;
    case ExceptionBreakMode::userUnhandled:
      j = json("userUnhandled");
      break;
    default:
      throw std::runtime_error {"Unknown ExceptionBreakMode"};
  };
}

void from_json(const json& j, ExceptionBreakMode& p) {
  std::string breakMode {j.get<std::string>()};
  if(breakMode == "never")
    p = ExceptionBreakMode::never;
  else if(breakMode == "always")
    p = ExceptionBreakMode::always;
  else if(breakMode == "unhandled")
    p = ExceptionBreakMode::unhandled;
  else if(breakMode == "userUnhandled")
    p = ExceptionBreakMode::userUnhandled;
  else
    throw std::runtime_error {"Unknown ExceptionBreakMode"};
}


struct ExceptionOptions {
  std::optional<std::vector<ExceptionPathSegment>> path;
  ExceptionBreakMode breakMode;
};

void to_json(json& j, const ExceptionOptions& p) {
  j = json {{"breakMode", p.breakMode}};
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
  unknown,
  string,
  number,
  boolean,
  unixTimestampUTC,
};

struct ColumnDescriptor {
  std::string attributeName;
  std::string label;
  std::optional<std::string> format;
  std::optional<ColumnDescriptorType> type;
  std::optional<std::int64_t> width;
};

enum struct CheckSumAlgorithm {
  unknown,
  MD5,
  SHA1,
  SHA256,
  timestamp,
};

struct Checksum {
  CheckSumAlgorithm algorithm;
  std::string checksum;
};

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

enum struct SourcePresentationHint {
  unknown,
  normal,
  emphasize,
  deemphasize,
};

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

struct InstructionBreakpoint {
  std::string instructionReference;
  std::optional<std::int64_t> offset;
  std::optional<std::string> condition;
  std::optional<std::string> hitConditiion;
};

enum struct DataBreakPointAccessType {
  unknown,
  read,
  write,
  readWrite,
};

struct DataBreakpoint {
  std::string dataId;
  std::optional<DataBreakPointAccessType> accessType;
  std::optional<std::string> condition;
  std::optional<std::string> hitCondition;
};

struct FunctionBreakpoint {
  std::string name;
  std::optional<std::string> condition;
  std::optional<std::string> hitCondition;
};

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

struct BreakpointLocation {
  std::int64_t line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
};

enum struct SteppingGranularity {
  unknown,
  statement,
  line,
  instruction,
};

struct Module {
  std::variant<std::uint64_t, std::string> id;
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

struct StackFrameFormat {
  std::optional<bool> parameters;
  std::optional<bool> parameterTypes;
  std::optional<bool> parameterNames;
  std::optional<bool> parameterValues;
  std::optional<bool> line;
  std::optional<bool> module;
  std::optional<bool> includeAll;
};

enum struct StackFramePresentationHint {
  unknown,
  normal,
  label,
  subtle,
};

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

struct ValueFormat {
  std::optional<bool> hex;
};

struct VariablePresentationHint {
  std::string kind;
  std::optional<std::vector<std::string>> attributes;
  std::optional<std::string> visibility;
  std::optional<bool> lazy;
};

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

struct Thread {
  int64_t id;
  std::string name;
};

struct StepInTarget {
  int64_t id;
  std::string label;
};

struct GotoTarget {
  int64_t id;
  std::string label;
  std::int64_t line;
  std::optional<std::int64_t> column;
  std::optional<std::int64_t> endLine;
  std::optional<std::int64_t> endColumn;
  std::optional<std::string> instructionPointerReference;
};

enum struct CompletionItemType {
  unknown,
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
  std::optional<std::vector<DataBreakPointAccessType>> accessTypes;
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
