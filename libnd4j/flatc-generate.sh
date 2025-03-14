#!/bin/bash

# flatbuffers
flatc  -o ./include/graph/generated -I ./include/graph/scheme -j -b  -t -c  ./include/graph/scheme/node.fbs ./include/graph/scheme/graph.fbs ./include/graph/scheme/result.fbs ./include/graph/scheme/config.fbs ./include/graph/scheme/array.fbs ./include/graph/scheme/utils.fbs ./include/graph/scheme/variable.fbs
flatc -o ./include/graph/generated -I ./include/graph/scheme -j -b  -t -c ./include/graph/scheme/node.fbs ./include/graph/scheme/graph.fbs ./include/graph/scheme/result.fbs ./include/graph/scheme/config.fbs ./include/graph/scheme/array.fbs ./include/graph/scheme/utils.fbs ./include/graph/scheme/variable.fbs


flatc -o ./include/graph/generated -I ./include/graph/scheme -j -b -t -c -n --js --grpc -p \
    ./include/graph/scheme/node.fbs \
    ./include/graph/scheme/graph.fbs \
    ./include/graph/scheme/result.fbs \
    ./include/graph/scheme/request.fbs \
    ./include/graph/scheme/config.fbs \
    ./include/graph/scheme/array.fbs \
    ./include/graph/scheme/utils.fbs \
    ./include/graph/scheme/variable.fbs \
    ./include/graph/scheme/properties.fbs
	
flatc -o ./include/graph/generated -I ./include/graph/scheme -j -b -t -c -n --js --grpc -p \
    ./include/graph/scheme/uigraphstatic.fbs \
    ./include/graph/scheme/uigraphevents.fbs