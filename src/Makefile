
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

binawatch : binawatch_httpd.o binawatch_apicaller.o jsoncpp.o
	g++ -I$(libmicrohttpd_include) -I$(libcurl_include) -I$(jsoncpp_include) binawatch.cpp \
	-L$(libmicrohttpd_lib) \
	-L$(libcurl_lib) \
	${build_dir}/binawatch_utils.o \
	${build_dir}/binawatch_httpd.o \
	${build_dir}/binawatch_apicaller.o \
	${build_dir}/jsoncpp.o \
	-lmicrohttpd -lcurl -lpthread  -o ${build_dir}/binawatch


binawatch_apicaller.o : binawatch_apicaller.cpp binawatch_apicaller.h 
	g++ -I$(libcurl_include) -I$(jsoncpp_include)  -c binawatch_apicaller.cpp  -o ${build_dir}/binawatch_apicaller.o 


binawatch_httpd.o : binawatch_httpd.cpp binawatch_httpd.h binawatch_utils.o
	g++ -I$(libmicrohttpd_include) -I$(jsoncpp_include)  -c binawatch_httpd.cpp -std=c++0x -o ${build_dir}/binawatch_httpd.o 


binawatch_utils.o : binawatch_utils.h binawatch_utils.cpp 
	g++ -c binawatch_utils.cpp -o ${build_dir}/binawatch_utils.o 


jsoncpp.o : $(jsoncpp_src)/jsoncpp.cpp 
	g++ -I$(jsoncpp_include) -c $(jsoncpp_src)/jsoncpp.cpp -o  ${build_dir}/jsoncpp.o


clean:
	rm ${build_dir}/*.o
	rm ${build_dir}/binawatch





 

