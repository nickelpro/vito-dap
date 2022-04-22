#ifndef VITO_DAP_HPP
#define VITO_DAP_HPP

#include <cstdint>
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

enum ColumnDescriptorType {
  UnknownColumnDescriptorType,
  String,
  Number,
  Boolean,
  UnixTimestampUTC,
};

struct ColumnDescriptor {
  std::string attributeName;
  std::string label;
  std::optional<std::string> format;
  std::optional<ColumnDescriptorType> type;
  std::optional<std::int64_t> width;
};

enum CheckSumAlgorithm {
  CheckSumAlgorithmUnknown,
  MD5,
  SHA1,
  SHA256,
  Timestamp,
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

enum SourcePresentationHint {
  SourcePresentationHintUnknown,
  Normal,
  Emphasize,
  Deemphasize,
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

enum MessageType {
  MessageTypeUnknown,
  Request,
  Response,
  Event,
};

struct ProtocolMessage {
  std::int64_t seq;
  MessageType type;
};

enum CommandType {
  CommandTypeUnknown,
  Cancel,
};

struct Request : ProtocolMessage {
  CommandType command;
};

enum EventType {
  EventTypeUnknown,
  E_Initialized,
  E_Stopped,
  E_Continued,
  E_Exited,
  E_Thread,
  E_Output,
  E_Breakpoint,
  E_Module,
  E_LoadedSource,
  E_Process,
  E_Capabilities,
  E_ProgressStart,
  E_ProgressUpdate,
  E_ProgressEnd,
  E_Invalidated,
  E_Memory,

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

enum OutputGroup {
  OutputGroupUnknown,
  Start,
  StartCollapsed,
  End,
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

enum ModuleReason {
  ModuleReasonUknown,
  New,
  Changed,
  Removed,
};

struct ModuleEvent : Event {
  ModuleReason reason;
  Module module;
};

struct LoadedSourceEvent : Event {
  ModuleReason reason;
  Source source;
};

enum StartMethod {
  StartMethodUnknown,
  Launch,
  Attach,
  AttachForSuspendedLaunch,
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

} // namespace dap

#endif // VITO_DAP_HPP
