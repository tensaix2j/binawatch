libmicrohttpd=../lib/libmicrohttpd-0.9.55
libmicrohttpd_include=${libmicrohttpd}/include
libmicrohttpd_lib=${libmicrohttpd}/lib

libcurl_dir=../lib/libcurl-7.56.0
libcurl_include=${libcurl_dir}/include
libcurl_lib=${libcurl_dir}/lib

jsoncpp_dir=../lib/jsoncpp-1.8.3
jsoncpp_include=${jsoncpp_dir}/include
jsoncpp_src=${jsoncpp_dir}/src


build_dir=../bin
objects=$(build_dir)/jsoncpp.o $(build_dir)/binawatch_utils.o $(build_dir)/binawatch_webservices.o $(build_dir)/binawatch_db.o $(build_dir)/binawatch_httpd.o $(build_dir)/binawatch_apicaller.o ${build_dir}/binawatch_logger.o



$(build_dir)/binawatch: $(objects)
	g++ -I$(libmicrohttpd_include) -I$(libcurl_include) -I$(jsoncpp_include) binawatch.cpp \
	-L$(libmicrohttpd_lib) \
	-L$(libcurl_lib) \
	$(objects) \
	-lmicrohttpd -lcurl -lcrypto -lsqlite3 -o ${build_dir}/binawatch



$(build_dir)/binawatch_apicaller.o: binawatch_apicaller.cpp binawatch_apicaller.h 
	g++ -I$(libcurl_include) -I$(jsoncpp_include)  -c binawatch_apicaller.cpp  -o ${build_dir}/binawatch_apicaller.o 


$(build_dir)/binawatch_httpd.o: binawatch_httpd.cpp binawatch_httpd.h
	g++ -I$(libmicrohttpd_include) -I$(jsoncpp_include)  -c binawatch_httpd.cpp -o ${build_dir}/binawatch_httpd.o 


$(build_dir)/binawatch_db.o: binawatch_db.cpp binawatch_db.h
	g++  -I$(libmicrohttpd_include) -c binawatch_db.cpp -o ${build_dir}/binawatch_db.o


$(build_dir)/binawatch_webservices.o: binawatch_webservices.cpp binawatch_webservices.h 
	g++ -I$(libmicrohttpd_include) -I$(jsoncpp_include)  -c binawatch_webservices.cpp  -o ${build_dir}/binawatch_webservices.o 


$(build_dir)/binawatch_logger.o: binawatch_logger.cpp binawatch_logger.h 
	g++ -c binawatch_logger.cpp  -o ${build_dir}/binawatch_logger.o 


$(build_dir)/binawatch_utils.o: binawatch_utils.cpp binawatch_utils.h 
	g++ -c binawatch_utils.cpp -o ${build_dir}/binawatch_utils.o 


$(build_dir)/jsoncpp.o: $(jsoncpp_src)/jsoncpp.cpp
	g++ -I$(jsoncpp_include) -c $(jsoncpp_src)/jsoncpp.cpp -o  ${build_dir}/jsoncpp.o


clean:
	rm ${build_dir}/*.o
	rm ${build_dir}/binawatch


