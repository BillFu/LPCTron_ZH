

protoc --cpp_out=. backend_infer.proto
protoc --grpc_out=. --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin backend_infer.proto
