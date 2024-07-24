#include <stdio.h>
#include "../include/VideoProcess.h"
#include "../include/httplib/httplib.h"
#include <iostream>
#include <fstream>
#include "../include/base64.h"
#include "time.h"

using namespace httplib;
using namespace std;

void _post(const Request &req, Response &res)
{
    //cout << req.body << endl;
    // res.set_header("Content-Type","application/xml;charset=utf-8");
    res.set_header("Access-Control-Allow-Origin", "*");
    res.status = 200;
    // TODO 正确的Content
    //Warning : 用time返回的时间cuo是不适用于多线程处理场景的
    stringstream ss;
    ss << time(NULL);
    string file_to_process_path = ss.str().append(".mp4");
    decode_form_text(req.body,file_to_process_path);
    //保存文件
    string product_file_name = _VideoShadowRemove(file_to_process_path);
    string result_error = "None";
    //cout << product_file_name << endl;
    if ("None" != product_file_name )
    {
        //返回成品文件名称
        string URI = "";
        URI.append("<URI>");
        URI.append(product_file_name);
        URI.append("</URI>");
        res.set_content(URI.data(), "text/xml");
        //我实在没精力做什么密钥访问啥的了，交给前端吧直接N_N
    }else{
        res.set_content("<URI>Error</URI>", "text/xml");
    }
}



int main()
{
    Server svr;
    svr.set_base_dir("./");

    /// upload
    //svr.Get("/upload", _get);

    // TODO 文件上传的后端处理
    svr.Post("/post", _post);

    /// listen
    svr.listen("localhost", 1234);
    //decode_form_text("MTIz=","B.txt");
    return 0;
}