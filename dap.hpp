#ifndef VITO_DAP_HPP
#define VITO_DAP_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <nlohmann/json.hpp>

namespace dap {

namespace js = nlohmann;

struct ExceptionBreakpointsFilter {
  std::string filter;
  std::string label;
  std::optional<std::string> description;
  std::optional<bool> default_;
  std::optional<bool> supportsCondition;
  std::optional<std::string> conditionDescription;
};

struct ExceptionFilterOptions {
  std::string filterId;
  std::optional<std::string> condition;
};

struct ExceptionPathSegment {
  std::optional<bool> negate;
  std::vector<std::string> name;
};

enum struct ExceptionBreakMode {
  unknown,
  never,
  always,
  unhandled,
  userUnhandled,
};

struct ExceptionOptions {
  std::optional<std::vector<ExceptionPathSegment>> path;
  ExceptionBreakMode breakMode;
};

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
  std::optional<js::json> adapterData;
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
  std::optional<js::json> restart;
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
  std::optional<js::json> data;
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
  std::optional<js::json> __restart;
};

struct LaunchResponse : Response {};

struct AttachRequest : Request {
  std::optional<js::json> __restart;
};

struct AttachResponse : Response {};

struct RestartRequest : Request {
  std::optional<bool> noDebug;
  std::optional<js::json> __restart;
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

} // namespace dap

#endif // VITO_DAP_HPP
