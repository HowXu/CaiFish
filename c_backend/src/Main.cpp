#include <stdio.h>
#include "../include/VideoShadowRemove.h"
#include "../include/httplib/httplib.h"
#include <iostream>
#include <fstream>

using namespace httplib;
using namespace std;

//这段html发送应该有问题
const char *html = R"(
                   <form id="formElem">
                   <input type="file" name="image_file" accept="image/*"><br>
                   <input type="file" name="text_file" accept="text/*"><br>
                   <input type="submit">
                   </form>
                   <script>
                   formElem.onsubmit = async (e) => {
                   e.preventDefault();
                   let res = await fetch('localhost:1234/post', {
                   method: 'POST',
                   body: new FormData(formElem)
                   });
                   console.log(await res.text());
                   };
                   </script>
                   )";

void _get(const Request & /*req*/, Response &res)
{
    res.set_header("Access-Control-Allow-Origin","*");
    res.set_header("Access-Control-Allow-Headers","*");
    res.set_header("Access-Control-Allow-Method","*");
    res.set_content(html, "text/html");
    res.status = 200;
}

void _post(const Request &req, Response &res)
{
    auto image_file = req.get_file_value("image_file");
    auto text_file = req.get_file_value("text_file");

    cout << "image file length: " << image_file.content.length() << endl
         << "image file name: " << image_file.filename << endl
         << "text file length: " << text_file.content.length() << endl
         << "text file name: " << text_file.filename << endl;

    {
        ofstream ofs(image_file.filename, ios::binary);
        ofs << image_file.content;
    }

    {
        ofstream ofs(text_file.filename);
        ofs << text_file.content;
    }
    res.set_header("Access-Control-Allow-Origin","*");
    res.set_header("Access-Control-Allow-Headers","*");
    res.set_header("Access-Control-Allow-Method","*");
    res.set_content("done", "text/plain");
}
int main()
{
    Server svr;
    svr.set_base_dir("./");

    /// upload
    svr.Get("/upload", _get);

    //TODO 文件上传的后端处理
    svr.Post("/post", _post);

    /// listen
    svr.listen("localhost", 1234);
    return 0;
}