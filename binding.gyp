{
    "targets": [{
        "target_name": "native",
        "sources": [ "src/set.cpp", "src/iterator.cpp" ],
        "include_dirs" : [ "<!(node -e \"require('nan')\")" ],
        "cflags": ["-std=c++11", "-Wall"],
        "conditions": [
            ['OS == "mac"', {
                'xcode_settings': {
                    'CLANG_CXX_LIBRARY': 'libc++'
                }
            }]
        ]
    }]
}
