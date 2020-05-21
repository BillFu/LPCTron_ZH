

protoc --cpp_out=backend_infer/ backend_infer.proto
protoc --grpc_out=backend_infer/ --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin backend_infer.proto
