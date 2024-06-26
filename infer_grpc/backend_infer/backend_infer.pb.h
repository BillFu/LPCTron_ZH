// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: backend_infer.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_backend_5finfer_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_backend_5finfer_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3008000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3008000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_backend_5finfer_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_backend_5finfer_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_backend_5finfer_2eproto;
class JobReply;
class JobReplyDefaultTypeInternal;
extern JobReplyDefaultTypeInternal _JobReply_default_instance_;
class JobRequest;
class JobRequestDefaultTypeInternal;
extern JobRequestDefaultTypeInternal _JobRequest_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::JobReply* Arena::CreateMaybeMessage<::JobReply>(Arena*);
template<> ::JobRequest* Arena::CreateMaybeMessage<::JobRequest>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class JobRequest :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:JobRequest) */ {
 public:
  JobRequest();
  virtual ~JobRequest();

  JobRequest(const JobRequest& from);
  JobRequest(JobRequest&& from) noexcept
    : JobRequest() {
    *this = ::std::move(from);
  }

  inline JobRequest& operator=(const JobRequest& from) {
    CopyFrom(from);
    return *this;
  }
  inline JobRequest& operator=(JobRequest&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const JobRequest& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const JobRequest* internal_default_instance() {
    return reinterpret_cast<const JobRequest*>(
               &_JobRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(JobRequest* other);
  friend void swap(JobRequest& a, JobRequest& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline JobRequest* New() const final {
    return CreateMaybeMessage<JobRequest>(nullptr);
  }

  JobRequest* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<JobRequest>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const JobRequest& from);
  void MergeFrom(const JobRequest& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  #else
  bool MergePartialFromCodedStream(
      ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(JobRequest* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "JobRequest";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_backend_5finfer_2eproto);
    return ::descriptor_table_backend_5finfer_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string sentence_id = 1;
  void clear_sentence_id();
  static const int kSentenceIdFieldNumber = 1;
  const std::string& sentence_id() const;
  void set_sentence_id(const std::string& value);
  void set_sentence_id(std::string&& value);
  void set_sentence_id(const char* value);
  void set_sentence_id(const char* value, size_t size);
  std::string* mutable_sentence_id();
  std::string* release_sentence_id();
  void set_allocated_sentence_id(std::string* sentence_id);

  // string sr = 2;
  void clear_sr();
  static const int kSrFieldNumber = 2;
  const std::string& sr() const;
  void set_sr(const std::string& value);
  void set_sr(std::string&& value);
  void set_sr(const char* value);
  void set_sr(const char* value, size_t size);
  std::string* mutable_sr();
  std::string* release_sr();
  void set_allocated_sr(std::string* sr);

  // string ipa_id_seq = 3;
  void clear_ipa_id_seq();
  static const int kIpaIdSeqFieldNumber = 3;
  const std::string& ipa_id_seq() const;
  void set_ipa_id_seq(const std::string& value);
  void set_ipa_id_seq(std::string&& value);
  void set_ipa_id_seq(const char* value);
  void set_ipa_id_seq(const char* value, size_t size);
  std::string* mutable_ipa_id_seq();
  std::string* release_ipa_id_seq();
  void set_allocated_ipa_id_seq(std::string* ipa_id_seq);

  // @@protoc_insertion_point(class_scope:JobRequest)
 private:
  class HasBitSetters;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr sentence_id_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr sr_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr ipa_id_seq_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_backend_5finfer_2eproto;
};
// -------------------------------------------------------------------

class JobReply :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:JobReply) */ {
 public:
  JobReply();
  virtual ~JobReply();

  JobReply(const JobReply& from);
  JobReply(JobReply&& from) noexcept
    : JobReply() {
    *this = ::std::move(from);
  }

  inline JobReply& operator=(const JobReply& from) {
    CopyFrom(from);
    return *this;
  }
  inline JobReply& operator=(JobReply&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const JobReply& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const JobReply* internal_default_instance() {
    return reinterpret_cast<const JobReply*>(
               &_JobReply_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(JobReply* other);
  friend void swap(JobReply& a, JobReply& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline JobReply* New() const final {
    return CreateMaybeMessage<JobReply>(nullptr);
  }

  JobReply* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<JobReply>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const JobReply& from);
  void MergeFrom(const JobReply& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  #else
  bool MergePartialFromCodedStream(
      ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(JobReply* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "JobReply";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_backend_5finfer_2eproto);
    return ::descriptor_table_backend_5finfer_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string error_msg = 2;
  void clear_error_msg();
  static const int kErrorMsgFieldNumber = 2;
  const std::string& error_msg() const;
  void set_error_msg(const std::string& value);
  void set_error_msg(std::string&& value);
  void set_error_msg(const char* value);
  void set_error_msg(const char* value, size_t size);
  std::string* mutable_error_msg();
  std::string* release_error_msg();
  void set_allocated_error_msg(std::string* error_msg);

  // bool isOK = 1;
  void clear_isok();
  static const int kIsOKFieldNumber = 1;
  bool isok() const;
  void set_isok(bool value);

  // @@protoc_insertion_point(class_scope:JobReply)
 private:
  class HasBitSetters;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr error_msg_;
  bool isok_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_backend_5finfer_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// JobRequest

// string sentence_id = 1;
inline void JobRequest::clear_sentence_id() {
  sentence_id_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& JobRequest::sentence_id() const {
  // @@protoc_insertion_point(field_get:JobRequest.sentence_id)
  return sentence_id_.GetNoArena();
}
inline void JobRequest::set_sentence_id(const std::string& value) {
  
  sentence_id_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:JobRequest.sentence_id)
}
inline void JobRequest::set_sentence_id(std::string&& value) {
  
  sentence_id_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:JobRequest.sentence_id)
}
inline void JobRequest::set_sentence_id(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  sentence_id_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:JobRequest.sentence_id)
}
inline void JobRequest::set_sentence_id(const char* value, size_t size) {
  
  sentence_id_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:JobRequest.sentence_id)
}
inline std::string* JobRequest::mutable_sentence_id() {
  
  // @@protoc_insertion_point(field_mutable:JobRequest.sentence_id)
  return sentence_id_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* JobRequest::release_sentence_id() {
  // @@protoc_insertion_point(field_release:JobRequest.sentence_id)
  
  return sentence_id_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void JobRequest::set_allocated_sentence_id(std::string* sentence_id) {
  if (sentence_id != nullptr) {
    
  } else {
    
  }
  sentence_id_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), sentence_id);
  // @@protoc_insertion_point(field_set_allocated:JobRequest.sentence_id)
}

// string sr = 2;
inline void JobRequest::clear_sr() {
  sr_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& JobRequest::sr() const {
  // @@protoc_insertion_point(field_get:JobRequest.sr)
  return sr_.GetNoArena();
}
inline void JobRequest::set_sr(const std::string& value) {
  
  sr_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:JobRequest.sr)
}
inline void JobRequest::set_sr(std::string&& value) {
  
  sr_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:JobRequest.sr)
}
inline void JobRequest::set_sr(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  sr_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:JobRequest.sr)
}
inline void JobRequest::set_sr(const char* value, size_t size) {
  
  sr_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:JobRequest.sr)
}
inline std::string* JobRequest::mutable_sr() {
  
  // @@protoc_insertion_point(field_mutable:JobRequest.sr)
  return sr_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* JobRequest::release_sr() {
  // @@protoc_insertion_point(field_release:JobRequest.sr)
  
  return sr_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void JobRequest::set_allocated_sr(std::string* sr) {
  if (sr != nullptr) {
    
  } else {
    
  }
  sr_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), sr);
  // @@protoc_insertion_point(field_set_allocated:JobRequest.sr)
}

// string ipa_id_seq = 3;
inline void JobRequest::clear_ipa_id_seq() {
  ipa_id_seq_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& JobRequest::ipa_id_seq() const {
  // @@protoc_insertion_point(field_get:JobRequest.ipa_id_seq)
  return ipa_id_seq_.GetNoArena();
}
inline void JobRequest::set_ipa_id_seq(const std::string& value) {
  
  ipa_id_seq_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:JobRequest.ipa_id_seq)
}
inline void JobRequest::set_ipa_id_seq(std::string&& value) {
  
  ipa_id_seq_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:JobRequest.ipa_id_seq)
}
inline void JobRequest::set_ipa_id_seq(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  ipa_id_seq_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:JobRequest.ipa_id_seq)
}
inline void JobRequest::set_ipa_id_seq(const char* value, size_t size) {
  
  ipa_id_seq_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:JobRequest.ipa_id_seq)
}
inline std::string* JobRequest::mutable_ipa_id_seq() {
  
  // @@protoc_insertion_point(field_mutable:JobRequest.ipa_id_seq)
  return ipa_id_seq_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* JobRequest::release_ipa_id_seq() {
  // @@protoc_insertion_point(field_release:JobRequest.ipa_id_seq)
  
  return ipa_id_seq_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void JobRequest::set_allocated_ipa_id_seq(std::string* ipa_id_seq) {
  if (ipa_id_seq != nullptr) {
    
  } else {
    
  }
  ipa_id_seq_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ipa_id_seq);
  // @@protoc_insertion_point(field_set_allocated:JobRequest.ipa_id_seq)
}

// -------------------------------------------------------------------

// JobReply

// bool isOK = 1;
inline void JobReply::clear_isok() {
  isok_ = false;
}
inline bool JobReply::isok() const {
  // @@protoc_insertion_point(field_get:JobReply.isOK)
  return isok_;
}
inline void JobReply::set_isok(bool value) {
  
  isok_ = value;
  // @@protoc_insertion_point(field_set:JobReply.isOK)
}

// string error_msg = 2;
inline void JobReply::clear_error_msg() {
  error_msg_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& JobReply::error_msg() const {
  // @@protoc_insertion_point(field_get:JobReply.error_msg)
  return error_msg_.GetNoArena();
}
inline void JobReply::set_error_msg(const std::string& value) {
  
  error_msg_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:JobReply.error_msg)
}
inline void JobReply::set_error_msg(std::string&& value) {
  
  error_msg_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:JobReply.error_msg)
}
inline void JobReply::set_error_msg(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  error_msg_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:JobReply.error_msg)
}
inline void JobReply::set_error_msg(const char* value, size_t size) {
  
  error_msg_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:JobReply.error_msg)
}
inline std::string* JobReply::mutable_error_msg() {
  
  // @@protoc_insertion_point(field_mutable:JobReply.error_msg)
  return error_msg_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* JobReply::release_error_msg() {
  // @@protoc_insertion_point(field_release:JobReply.error_msg)
  
  return error_msg_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void JobReply::set_allocated_error_msg(std::string* error_msg) {
  if (error_msg != nullptr) {
    
  } else {
    
  }
  error_msg_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), error_msg);
  // @@protoc_insertion_point(field_set_allocated:JobReply.error_msg)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_backend_5finfer_2eproto
