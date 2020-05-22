// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: backend_infer.proto
#ifndef GRPC_backend_5finfer_2eproto__INCLUDED
#define GRPC_backend_5finfer_2eproto__INCLUDED

#include "backend_infer.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc_impl {
class CompletionQueue;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc_impl

namespace grpc {
namespace experimental {
template <typename RequestT, typename ResponseT>
class MessageAllocator;
}  // namespace experimental
}  // namespace grpc

class backend_infer final {
 public:
  static constexpr char const* service_full_name() {
    return "backend_infer";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status commitJob(::grpc::ClientContext* context, const ::JobRequest& request, ::JobReply* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::JobReply>> AsynccommitJob(::grpc::ClientContext* context, const ::JobRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::JobReply>>(AsynccommitJobRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::JobReply>> PrepareAsynccommitJob(::grpc::ClientContext* context, const ::JobRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::JobReply>>(PrepareAsynccommitJobRaw(context, request, cq));
    }
    class experimental_async_interface {
     public:
      virtual ~experimental_async_interface() {}
      virtual void commitJob(::grpc::ClientContext* context, const ::JobRequest* request, ::JobReply* response, std::function<void(::grpc::Status)>) = 0;
      virtual void commitJob(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::JobReply* response, std::function<void(::grpc::Status)>) = 0;
      virtual void commitJob(::grpc::ClientContext* context, const ::JobRequest* request, ::JobReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) = 0;
      virtual void commitJob(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::JobReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) = 0;
    };
    virtual class experimental_async_interface* experimental_async() { return nullptr; }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::JobReply>* AsynccommitJobRaw(::grpc::ClientContext* context, const ::JobRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::JobReply>* PrepareAsynccommitJobRaw(::grpc::ClientContext* context, const ::JobRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status commitJob(::grpc::ClientContext* context, const ::JobRequest& request, ::JobReply* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::JobReply>> AsynccommitJob(::grpc::ClientContext* context, const ::JobRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::JobReply>>(AsynccommitJobRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::JobReply>> PrepareAsynccommitJob(::grpc::ClientContext* context, const ::JobRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::JobReply>>(PrepareAsynccommitJobRaw(context, request, cq));
    }
    class experimental_async final :
      public StubInterface::experimental_async_interface {
     public:
      void commitJob(::grpc::ClientContext* context, const ::JobRequest* request, ::JobReply* response, std::function<void(::grpc::Status)>) override;
      void commitJob(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::JobReply* response, std::function<void(::grpc::Status)>) override;
      void commitJob(::grpc::ClientContext* context, const ::JobRequest* request, ::JobReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) override;
      void commitJob(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::JobReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit experimental_async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class experimental_async_interface* experimental_async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class experimental_async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::JobReply>* AsynccommitJobRaw(::grpc::ClientContext* context, const ::JobRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::JobReply>* PrepareAsynccommitJobRaw(::grpc::ClientContext* context, const ::JobRequest& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_commitJob_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status commitJob(::grpc::ServerContext* context, const ::JobRequest* request, ::JobReply* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_commitJob : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_commitJob() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_commitJob() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status commitJob(::grpc::ServerContext* context, const ::JobRequest* request, ::JobReply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestcommitJob(::grpc::ServerContext* context, ::JobRequest* request, ::grpc::ServerAsyncResponseWriter< ::JobReply>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_commitJob<Service > AsyncService;
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_commitJob : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithCallbackMethod_commitJob() {
      ::grpc::Service::experimental().MarkMethodCallback(0,
        new ::grpc_impl::internal::CallbackUnaryHandler< ::JobRequest, ::JobReply>(
          [this](::grpc::ServerContext* context,
                 const ::JobRequest* request,
                 ::JobReply* response,
                 ::grpc::experimental::ServerCallbackRpcController* controller) {
                   return this->commitJob(context, request, response, controller);
                 }));
    }
    void SetMessageAllocatorFor_commitJob(
        ::grpc::experimental::MessageAllocator< ::JobRequest, ::JobReply>* allocator) {
      static_cast<::grpc_impl::internal::CallbackUnaryHandler< ::JobRequest, ::JobReply>*>(
          ::grpc::Service::experimental().GetHandler(0))
              ->SetMessageAllocator(allocator);
    }
    ~ExperimentalWithCallbackMethod_commitJob() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status commitJob(::grpc::ServerContext* context, const ::JobRequest* request, ::JobReply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual void commitJob(::grpc::ServerContext* context, const ::JobRequest* request, ::JobReply* response, ::grpc::experimental::ServerCallbackRpcController* controller) { controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "")); }
  };
  typedef ExperimentalWithCallbackMethod_commitJob<Service > ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_commitJob : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_commitJob() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_commitJob() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status commitJob(::grpc::ServerContext* context, const ::JobRequest* request, ::JobReply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_commitJob : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_commitJob() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_commitJob() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status commitJob(::grpc::ServerContext* context, const ::JobRequest* request, ::JobReply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestcommitJob(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_commitJob : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithRawCallbackMethod_commitJob() {
      ::grpc::Service::experimental().MarkMethodRawCallback(0,
        new ::grpc_impl::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
          [this](::grpc::ServerContext* context,
                 const ::grpc::ByteBuffer* request,
                 ::grpc::ByteBuffer* response,
                 ::grpc::experimental::ServerCallbackRpcController* controller) {
                   this->commitJob(context, request, response, controller);
                 }));
    }
    ~ExperimentalWithRawCallbackMethod_commitJob() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status commitJob(::grpc::ServerContext* context, const ::JobRequest* request, ::JobReply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual void commitJob(::grpc::ServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response, ::grpc::experimental::ServerCallbackRpcController* controller) { controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "")); }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_commitJob : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_commitJob() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler< ::JobRequest, ::JobReply>(std::bind(&WithStreamedUnaryMethod_commitJob<BaseClass>::StreamedcommitJob, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_commitJob() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status commitJob(::grpc::ServerContext* context, const ::JobRequest* request, ::JobReply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedcommitJob(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::JobRequest,::JobReply>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_commitJob<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_commitJob<Service > StreamedService;
};


#endif  // GRPC_backend_5finfer_2eproto__INCLUDED