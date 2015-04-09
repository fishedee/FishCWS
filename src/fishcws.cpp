/*
 *  FISHCWS - FISHCWS is an Chinese Word Segmentation System Based on the HTTP protocol.
 *
 *       https://github.com/fishedee/FishCWS
 *
 *  Copyright 2015 Li Jinwei.  All rights reserved.
 *
 *  Use and distribution licensed under the BSD license. 
 *
 *  Authors:
 *      Li Jinwei <306766045@qq.com> http://blog.fishedee.com
 */
extern "C" {
#include <sys/types.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <wait.h>

#include <err.h>
#include <event.h>
#include <evhttp.h>
}

#include "NLPIR.h"
#include "jsoncpp/json/json.h"
#include <functional>
#include <string>
#include <vector>
#define VERSION "1.0.0"
using namespace std;

class CommandLine{
public:
	string httpcws_settings_listen;
	int httpcws_settings_port;
	string httpcws_settings_datapath;
	bool httpcws_settings_daemon;
	int httpcws_settings_timeout;
private:
	void show_help(void){
		const char *b = "FishCWS v" VERSION " written by Li Jinwei (http://blog.fishedee.com)\n"
			  "\n"
			   "-l <ip_addr>  interface to listen on, default is 0.0.0.0\n"
			   "-p <num>      TCP port number to listen on (default: 1985)\n"
			   "-x <path>     Chinese dictionary path (example: /usr/local/httpcws/dict)\n"
			   "-t <second>   timeout for an http request (default: 120)\n"		   
			   "-d            run as a daemon\n"
			   "-h            print this help and exit\n"	   
			   "\n";
		fprintf(stderr, b, strlen(b));
		exit(0);
	}
public:
	CommandLine(){
		httpcws_settings_listen = "0.0.0.0";
		httpcws_settings_port = 1985;
		httpcws_settings_datapath = ""; /*中文词典数据库路径 */
		httpcws_settings_daemon = false;
		httpcws_settings_timeout = 120; /* 单位：秒 */
	}
	void Run(int argc, char* argv[]){
		char c;
		while ((c = getopt(argc, argv, "l:p:x:t:dh")) != -1) {
			switch (c) {
			case 'l':
				httpcws_settings_listen = strdup(optarg);
				break;
			case 'p':
				httpcws_settings_port = atoi(optarg);
				break;
			case 'x':
				httpcws_settings_datapath = strdup(optarg); /* 词库文件存储路径 */
				break;
			case 't':
				httpcws_settings_timeout = atoi(optarg);
				break;			
			case 'd':
				httpcws_settings_daemon = true;
				break;
			case 'h':			
			default:
				show_help();
				break;
			}
		}
		
		/* 判断是否加了必填参数 -x */
		if (httpcws_settings_datapath == "") {
			show_help();
			fprintf(stderr, "Attention: Please use the indispensable argument: -x <path>\n\n");		
			exit(1);
		}
	}
};
class Network{
private:
	typedef function<Json::Value(const Json::Value&)> JsonHandler;
	JsonHandler handler;
	struct evhttp *httpd;
public:
	static void Handler(struct evhttp_request *req, void *arg){
		/*
		struct evkeyvalq httpcws_http_query;
		evhttp_parse_query(evhttp_request_uri(req), &httpcws_http_query);
		evhttp_clear_headers(&httpcws_http_query);
		*/
		
		/* 接收POST表单信息 */
		string inputData(
			(const char*) EVBUFFER_DATA(req->input_buffer),
			(const char*) EVBUFFER_DATA(req->input_buffer) + EVBUFFER_LENGTH(req->input_buffer)
		);
		
		/*字符串流转换成JSON格式*/
		Json::Value inputJson;
		Json::Value outputJson;
		Json::Reader reader;
		bool ok = reader.parse( inputData , inputJson , false );
		if( ok == false ){
			outputJson["code"] = 1;
			outputJson["msg"] = "invalid json format input";
			outputJson["data"] = "";
		}else{
			outputJson = ((Network*)arg)->handler(inputJson);
		}
		
		/*JSON格式转换成字符串流*/
		Json::FastWriter writer;
		string outputData = writer.write( outputJson );
		
		/* 返回响应信息 */
		struct evbuffer *buf = evbuffer_new();
		evhttp_add_header(req->output_headers, "Server", "HTTPCWS/1.0.0");
		evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=GB2312");
		evhttp_add_header(req->output_headers, "Connection", "close");
		evbuffer_add_printf(buf, "%s", outputData.c_str());
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
		evbuffer_free(buf);	
	}
public:
	Network(
		const string& httpcws_settings_listen , 
		int httpcws_settings_port ,
		int httpcws_settings_timeout,
		JsonHandler inhandler
	){
		event_init();
		httpd = evhttp_start(httpcws_settings_listen.c_str(), httpcws_settings_port);
		if (httpd == NULL) {
			fprintf(stderr, "Error: Unable to listen on %s:%d\n\n", httpcws_settings_listen.c_str(), httpcws_settings_port);       
			exit(1);
		}
		evhttp_set_timeout(httpd, httpcws_settings_timeout);
		evhttp_set_gencb(httpd, &Network::Handler, this);
		handler = inhandler;
	}
	void Run(){
		event_dispatch();
	}
	~Network(){
		evhttp_free(httpd);
	}
};

class NLPTR{
public:
	NLPTR(const std::string& httpcws_settings_datapath){
		if(!ICTCLAS_Init("/usr/local/fishcws",UTF8_CODE,0)){
			printf("ICTCLAS_Init Fail\n");
			fprintf(stderr, "ERROR: Count not open the Chinese dictionary!\n\n");		
			exit(1);
		}
		ICTCLAS_SetPOSmap(ICT_POS_MAP_SECOND);
		
		fprintf(stderr, "Loading Chinese dictionary '%s' into memory, please waitting ......\n",httpcws_settings_datapath.c_str());
		int nCount = NLPIR_ImportUserDict(httpcws_settings_datapath.c_str());
		printf("OK! %d words has loaded into memory.\n\n", nCount);
	}
	vector<string> ParagraphProcess( const string& input ){
		vector<string> result;
		int nCount = 0;
		const result_t* pVecResult = NLPIR_ParagraphProcessA(input.c_str(),&nCount,true);
		for( int i = 0 ; i < nCount ; ++i ){
			result.push_back(
				input.substr(pVecResult[i].start,pVecResult[i].length)
			);
		}
		return result;
	}
};

class Business{
private:
	NLPTR nlptr;
public:
	Business(const std::string& httpcws_settings_datapath):nlptr(httpcws_settings_datapath){
	}
	Json::Value Handle( const Json::Value& inputJson ){
		Json::Value outputValue;
		if( inputJson["type"].isString() && inputJson["type"] == "ParagraphProcess"
			&& inputJson["data"].isString()){
			//分词请求
			vector<string> output = nlptr.ParagraphProcess(inputJson["data"].asString());
			Json::Value strings;
			for( unsigned int i = 0 ; i < output.size() ; ++i )
				strings.append(output[i]);
			outputValue["code"] = 0;
			outputValue["msg"] = "";
			outputValue["data"] = strings;
		}else{
			//其它请求
			outputValue["code"] = 1;
			outputValue["msg"] = "invalid request type";
			outputValue["data"] = "";
		}
		return outputValue;
	}
};
void runAsDaemon(){
	pid_t pid;

	/* Fork off the parent process */       
	pid = fork();
	if (pid < 0) {
			exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0) {
			exit(EXIT_SUCCESS);
	}
}
int main(int argc, char* argv[]){
	//处理输入的commandLine信息
	CommandLine commandLine;
	commandLine.Run(argc,argv);
	
	//判断是否以daemon方式启动
	if (commandLine.httpcws_settings_daemon == true)
		runAsDaemon();
	
	//执行业务逻辑
	Business business(
		commandLine.httpcws_settings_datapath
	);
	Network network(
		commandLine.httpcws_settings_listen , 
		commandLine.httpcws_settings_port ,
		commandLine.httpcws_settings_timeout,
		bind(&Business::Handle,business,std::placeholders::_1)
	);
	network.Run();
	return 0;
}