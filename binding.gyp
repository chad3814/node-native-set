{
    "targets": [{
        "target_name": "native",
        "sources": [ "src/native.cpp", "src/map.cpp", "src/iterator.cpp" ],
        "cflags": [ "-std=c++11", "-stdlib=libc++", "-Wall" ],
        "include_dirs" : [ "<!(node -e \"require('nan')\")" ]
    }]
}
