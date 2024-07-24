
function onScroll() {
    let top_ht = document.documentElement.scrollTop;
    let scroll_ht = document.body.scrollHeight = "550";

    let title = document.getElementById("title");
    let dec = document.getElementById("dec");
    let video = document.getElementById("video");

    if (top_ht >= scroll_ht / 3) {
        let transparent_hdrd = ((top_ht - scroll_ht / 3) / (scroll_ht / 3));

        //把这三个货放在一个div不就得了？
        title.style.display = "inherit";
        dec.style.display = "inherit";
        video.style.display = "inherit";

        title.style.opacity = transparent_hdrd
        dec.style.opacity = transparent_hdrd
        video.style.opacity = transparent_hdrd
    } else {
        title.style.opacity = 0
        dec.style.opacity = 0
        video.style.opacity = 0
    }
}

let file_text = document.getElementById("text");
let file_box = document.getElementById("fileinp");
let video_box = document.getElementById("video_play");

file_box.addEventListener("change", function () {
    file_text.textContent = file_box.files[0].name;
    //选中后改名

});

let submit_button = document.getElementById("submit");

/**
 * CSDN https://blog.csdn.net/freezingxu/article/details/77922367 函数
 * 向服务器发起ajax请求
 * 方法的入参说明如下：
 * paramIn：发送给服务器端的请求参数
 * urlIn：服务器端webservice地址
 * callbackFuncIn：当完成请求以后，调用的用以处理服务器响应参数的回调方法的方法名，该方法接收一个HTTP_REQUEST对象作为入参
 */
function baseRequest(paramIn, urlIn, callbackFuncIn) {
    let http_request = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari等非IE的浏览器
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE浏览器
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
                http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) { }
        }
    }

    if (!http_request) {
        alert("无法创建基于XmlHttp的Ajax请求对象");
        return false;
    }

    http_request.onreadystatechange = function () { baseCallback(http_request, callbackFuncIn) };
    http_request.open("POST", urlIn, true);
    http_request.setRequestHeader('Content-Type', 'text/plain;charset=utf-8');
    http_request.send(paramIn);
}

/**
 * 向服务器发起ajax的请求后，接受服务器的响应参数，并调用用户自定义的回调处理方法
 * 方法的入参说明如下：
 * httpRequestIn：发送给服务器端的请求对象
 * callbackFuncIn：当完成请求以后，调用的用以处理服务器响应参数的回调方法的方法名，该方法接收一个HTTP_REQUEST对象作为入参
 */
function baseCallback(httpRequestIn, callbackFuncIn) {
    
    //alert(httpRequestIn.responseXML)
    if(httpRequestIn.readyState == 4){
        if (httpRequestIn.status == 200) {
            callbackFuncIn(httpRequestIn);
        } else {
            alert("服务器出现问题");
        }
    }
        
}




submit_button.addEventListener("click", function () {
    if (!file_box.files.length) {   //如果文件为空返回提示
        alert('请选择文件');
        return;
    }

    let reader = new FileReader();

    reader.readAsDataURL(file_box.files[0]);
    reader.onload = function () {
        let fileStringBase64; //这个是处理后的存粹Data
        fileStringBase64 = this.result;
        fileStringBase64 = fileStringBase64.substring(fileStringBase64.indexOf(",") + 1, fileStringBase64.length);
        
        //咱也不知道为什么只能在回调函数里用

        //上传响应内容，服务器地址，回调函数
        baseRequest(fileStringBase64,"http://localhost:1234/post",function(http_request){
            //这里只有成功回调函数
            //alert(http_request.body)
            if(http_request.responseText != "null"){
                //http_request.abort()
                let parser = new DOMParser();
                //XMl解析获得内置URI
                let videoURI = parser.parseFromString(http_request.responseText,"text/xml").getElementsByTagName("URI")[0].childNodes[0].nodeValue;
                //alert(videoURI)
                //这里应该开个反向代理然后指向服务器，交给后端了N_N
                video_box.src = "http://localhost/video/" + videoURI;
                
            }
        })

    }
    //console.log(fileStringBase64)
});



//TODO 文件上传