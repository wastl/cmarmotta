# Apache Marmotta LevelDB/C++ Backend

This repository implements an experimental high-performance backend for Apache Marmotta
using LevelDB as storage and gRPC as communication channel between the Java frontend
and the C++ backend. 

If it proves to be useful, the repository will eventually be merged into the main 
development branch of Apache Marmotta

## Dependencies (C++)

To compile the C++ backend, you need to have the following dependencies installed:

  * libraptor (used for parsing/serializing in C++)
  * librasqal (used for server-side SPARQL evaluation)
  * libglog (logging)
  * libgflags (command line arguments)
  * libleveldb (database backend)
  * libgrpc (gRPC runtime)
  * libprotobuf (messaging, data model)

With the exception of libgrpc and libprotobuf, all libraries are available in Linux repositories.
Debian:

    apt-get install libraptor2-dev librasqal3-dev libgoogle-glog-dev libgflags-dev libleveldb-dev
    
The backend uses the new Proto 3 format and the gRPC SDK. These need to be installed separately,
please follow the instructions at [https://github.com/grpc/grpc](https://github.com/grpc/grpc/blob/master/INSTALL).


## Compilation (C++)

The backend uses cmake to compile the modules. Create a new directory `build`, run cmake, and run make:

    mkdir build && cd build
    cmake ..
    make

## Compilation (Java)

The frontend is compiled with Maven and depends on many Apache Marmotta modules to work. Build it with

    cd java
    mvn clean install
    
## Running C++ Backend

Start the backend from the cmake build directory as follows:

    ./service/marmotta_persistence -db /path/to/database -port 10000
    
The binary accepts many different options. Please see `--help` for details.

## Running Apache Marmotta 

A preconfigured version of Apache Marmotta is available in `java/webapp`. It connects to 
`localhost:10000` by default and can be started with:

    mvn tomcat7:run
    
Afterwards, point your browser to `localhost:8080`.

## Command Line Client

A C++ command line client is available for very fast bulk imports and simple queries. To import
a large turtle file, run:

    ./client/marmotta_client --format=turtle import file.ttl

The client connects by default to `localhost:10000` (change with `--host` and `--port` flags).
