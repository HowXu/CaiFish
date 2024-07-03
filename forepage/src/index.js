

function onScroll (){

    let top_ht = document.documentElement.scrollTop;
    let scroll_ht = document.body.scrollHeight="550";
    
    let title = document.getElementById("title");
    let dec = document.getElementById("dec");

	if(top_ht >= scroll_ht / 3){
        let transparent_hdrd = ((top_ht - scroll_ht /3 ) / (scroll_ht / 3));
		title.style.display="inherit";
        dec.style.display="inherit";

        title.style.opacity = transparent_hdrd
        dec.style.opacity = transparent_hdrd
	}else{
		title.style.display="none";
        dec.style.display="none";
	}
}