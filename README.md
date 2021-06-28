﻿﻿![Crow logo](http://i.imgur.com/wqivvjK.jpg)
Crow is C++ microframework for web. Support Mac, Linux, windows, three platforms, the fastest development speed, the quickest and the strongest, the next step is to support database and ORM.

### [Demo site(example_vs)](http://8.129.58.72:8080/)🚀
(inspired by Python Flask)[fork by Asciphx]

[![Travis Build](https://travis-ci.org/ipkn/crow.svg?branch=master)](https://travis-ci.org/ipkn/crow)
[![Coverage Status](https://coveralls.io/repos/ipkn/crow/badge.svg?branch=master)](https://coveralls.io/r/ipkn/crow?branch=master)

```c++
#include "crow.h"
int main(){
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });
    app.port(18080).multithreaded().run();
}
```

## Features

 - Easy routing
   - Similiar to Flask
   - Type-safe Handlers (see Example)
 - Very Fast
   - ![https://docs.google.com/spreadsheets/d/1KidO9XpuwCRZ2p_JRDJj2aep61H8Sh_KDOhApizv4LE/pubchart?oid=2041467789&format=image](./Benchmark.png)
   - More data on [crow-benchmark](https://github.com/ipkn/crow-benchmark)
 -  The third-party JSON parser nlohmann (Crow::json) is used for static reflection and outputs JSON.
 - [Mustache](http://mustache.github.io/) based templating library (crow::mustache)
 - Header only
 - Provide an amalgamated header file [`crow_all.h`](https://github.com/ipkn/crow/releases/download/v0.1/crow_all.h) with every features ([Download from here](https://github.com/ipkn/crow/releases/download/v0.1/crow_all.h))
 - Middleware support
 - Websocket support
 - Support static resources and the default is in the 'static/' directory
 - Modular development, high efficiency, simple code
## Still in development
 - ~~Built-in ORM~~
   - Check [sqlpp11](https://github.com/rbock/sqlpp11) if you want one.

## Examples
#### Static reflection
```c++
  app.route_dynamic("/list")([]() {
	List list=json::parse(R"({"user":{"is":false,"age":25,"weight":50.6,"name":"www","state":null},
            "userList":[{"is":true,"weight":52.0,"age":23,"state":true,"name":"wwzzgg"},
	    {"is":true,"weight":51.0,"name":"best","age":26}]})").get<List>();
	json json_output=json(list);
	return json_output.dump(2);
  });
```
#### Server rendering
```c++
  CROW_ROUTE(app,"/")([] {
	char name[64];gethostname(name,64);
	mustache::Ctx x;x["servername"]=name;
	auto page=mustache::load("index.html");
	return page.render(x);
  });
```

#### JSON Response
```c++
CROW_ROUTE(app, "/json")([]{
    crow::json x;
	x["message"]="Hello, World!";
	x["double"]=3.1415926;
	x["int"]=2352352;
	x["true"]=true;
	x["false"]=false;
	x["null"]=nullptr;
	x["bignumber"]=2353464586543265455;
    return x;
});
```

#### Arguments
```c++
CROW_ROUTE(app,"/hello/<int>")([](int count){
    if (count > 100) return crow::Res(400);
    std::ostringstream os;
    os << count << " bottles of beer!";
    return crow::Res(os.str());
});
```
Handler arguments type check at compile time
```c++
// Compile error with message "Handler type is mismatched with URL paramters"
CROW_ROUTE(app,"/another/<int>")([](int a, int b){
    return crow::Res(500);
});
```

#### Handling JSON Requests
```c++
CROW_ROUTE(app, "/add_json").methods("POST"_method)
([](const crow::Req& req){
    auto x = crow::json::load(req.body);
    if (!x)
        return crow::Res(400);
    int sum = x["a"].i()+x["b"].i();
    std::ostringstream os;
    os << sum;
    return crow::Res{os.str()};
});
```

## How to Build

If you just want to use crow, copy amalgamate/crow_all.h and include it.

### Requirements
 - C++ compiler with good C++14 support (tested with g++>6.0)
 - Any version of boost library
 - CMake for build examples
 - Linking with tcmalloc/jemalloc is recommended for speed.
 - Now supporting VS2019 with limited functionality (only run-time check for url is available.)

### Building (Tests, Examples)
Out-of-source build with CMake is recommended.
```
mkdir build
cd build
cmake ..
make
```
You can run tests with following commands:
```
ctest
```
### Installing missing dependencies
#### Ubuntu
    sudo apt-get install build-essential libtcmalloc-minimal4 && sudo ln -s /usr/lib/libtcmalloc_minimal.so.4 /usr/lib/libtcmalloc_minimal.so
#### OSX
    brew install boost google-perftools
#### Windows
> first install boost  
> second modify CmakeLists.txt

##### CmakeLists.txt[example]
```cmake
SET(BOOST_ROOT "E:/Code/boost_1_75_0") #Installation address of decompressed version
set(Boost_USE_STATIC_LIBS ON) #Support anything else
```
### Attributions

Crow uses the following libraries.

    http-parser

    https://github.com/nodejs/http-parser

    http_parser.c is based on src/http/ngx_http_parse.c from NGINX copyright
    Igor Sysoev.

    Additional changes are licensed under the same terms as NGINX and
    copyright Joyent, Inc. and other Node contributors. All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE. 


    qs_parse

    https://github.com/bartgrantham/qs_parse

    Copyright (c) 2010 Bart Grantham
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.


    TinySHA1

    https://github.com/mohaps/TinySHA1

    TinySHA1 - a header only implementation of the SHA1 algorithm. Based on the implementation in boost::uuid::details

    Copyright (c) 2012-22 SAURAV MOHAPATRA mohaps@gmail.com
    Permission to use, copy, modify, and distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    json 

    https://github.com/nlohmann/json

    Copyright (c) 2013-2021 Niels Lohmann
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
