
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

file_box.addEventListener("change", function () {
    file_text.textContent = file_box.files[0].name;
    //选中后改名
});

let submit_buuron = document.getElementById("submit");

let file_formData = new FormData();
file_formData.append("video", file_box.files[0]);

//TODO 文件上传