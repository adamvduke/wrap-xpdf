# wrap-xpdf

Trying out extracting text from pdf documents using [xpdf](https://www.xpdfreader.com/opensource.html).

Text can be extracted via cgo or via a C++ server that exposes a GPRC service.

## Setup
* Install cmake
* Install absl
* Install protoc
* Install the C++ protoc plugin
* Install the Go proto plugin

## CGO
```
# Run cmake which will download the xpdf source to a known place and create make files for building the various C/C++ dependencies.
$ cd $PROJECT_ROOT/cpp/cmake/build
$ cmake -DNO_FONTCONFIG=ON -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..

# Run make extract to build the C lib that the Go code will link against.
$ make extract

# Generate the go proto code because the example depends on it
$ cd $PROJECT_ROOT/go/example
$ go generate

# Run the Go program with -use-grpc=false
$ cd $PROJECT_ROOT/go/example
$ go run main.go -use-grpc=false -input-path=/path/to/file.pdf -output-path=/path/to/output.txt 2> /dev/null
```

## GPRC
```
# Run cmake which will download the xpdf source to a known place and create make files for building the various C/C++ dependencies.
$ cd $PROJECT_ROOT/cpp/cmake/build
$ cmake -DNO_FONTCONFIG=ON -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..

# Run make xpdf_grpc_server to build the server that hosts the RPC.
$ make xpdf_grpc_server

# Generate the go proto code
$ cd $PROJECT_ROOT/go/example
$ go generate

# In a separate terminal, start the grpc server
$ cd $PROJECT_ROOT/cpp/cmake/build
$ ./xpdf_grpc_server

# Run the Go program with -use-grpc=true
$ cd $PROJECT_ROOT/go/example
$ go run main.go -use-grpc=true -input-path=/path/to/file.pdf -output-path=/path/to/output.txt 2> /dev/null
```

## Notes
To manually generate C++ proto headers/implementations

```
protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` server.proto
protoc --cpp_out=. server.proto
```

## License
Any code from the [xpdf](https://www.xpdfreader.com/opensource.html) or [grpc](https://github.com/grpc/grpc) projects retains its original license.

Other code is subject to the license in the [LICENSE](https://github.com/adamvduke/wrap-xpdf/blob/main/LICENSE) file.