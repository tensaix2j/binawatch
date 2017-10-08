
libmicrohttpd=../lib/libmicrohttpd-0.9.55
libmicrohttpd_include=${libmicrohttpd}/include
libmicrohttpd_lib=${libmicrohttpd}/lib

libcurl_dir=../lib/libcurl-7.56.0
libcurl_include=${libcurl_dir}/include
libcurl_lib=${libcurl_dir}/lib

jsoncpp_dir=../lib/jsoncpp-1.8.3
jsoncpp_include=${jsoncpp_dir}/include
jsoncpp_src=${jsoncpp_dir}/src


export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$libmicrohttpd_lib:$libcurl_lib

./binawatch
