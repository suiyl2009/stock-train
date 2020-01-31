var odiv = document.createElement('div');
odiv.id = "menu";
odiv.innerHTML =
    '<div id="menu">  \
        <div id="hiden"></div>  \
        <div id="slcd">  \
            <ul>  \
                <li>  \
                    菜单1  \
                </li>  \
                <li>菜单2</li>  \
                <li>菜单3</li>  \
                <li>菜单4</li>  \
                <li>配置  \
                   <div id="zcd" class="zcd1">  \
                        <ul>  \
                            <li id="kindconf">分类</li>  \
                            <li id="listconf">列表</li>  \
                            <li id="lineconf">曲线</li>  \
                        </ul>  \
                    </div>  \
                </li>  \
                <li id="fullscreen">全屏</li>  \
                <li id="movehide">&#8681&#8681</li>  \
            </ul>  \
        </div>  \
        <div id="divIframe">  \
            <button id="mybtn" style="position:absolute;top:0">X</button>  \
            <iframe id="myiframe" src="test.html" scrolling="no" style="border:0px;"></iframe>  \
        </div>  \
    </div>';
document.getElementById('body').appendChild(odiv);

var diag = document.getElementById("divIframe");
document.getElementById("lineconf").onclick = function () {
    console.log("block");
    diag.style.height = '600px';
    diag.style.width = '800px';
    //document.getElementById("myiframe").src = "conf/lineconf.html";
    diag.style.display = 'block';
    diag.style.border = '5px solid #00ffcc';

    return;
}
document.getElementById("mybtn").onclick = function () {
    diag.style.display = 'none';
}

// 隐藏菜单
var obscure = document.querySelector("#movehide");
var open = document.querySelector("#slcd");
var ensconce = document.querySelector("#hiden");
obscure.onclick = function () {
    open.style.marginBottom = "-300px";
    setTimeout(function () {
        ensconce.style.display = "block";
    }, 350)

}
//显示菜单
var showC = document.querySelector("#hiden");
showC.onmousemove = function () {
    open.style.marginBottom = "0px";
    setTimeout(function () {
        ensconce.style.display = "none";
    }, 100)

}

// 全屏
function requestFullScreen(element) {
    // 判断各种浏览器，找到正确的方法
    var requestMethod = element.requestFullScreen || //W3C
    element.webkitRequestFullScreen || //Chrome��
    element.mozRequestFullScreen || //FireFox
    element.msRequestFullScreen; //IE11
    if (requestMethod) {
        requestMethod.call(element);
    }
    else if (typeof window.ActiveXObject !== "undefined") {//for Internet Explorer
        var wscript = new ActiveXObject("WScript.Shell");
        if (wscript !== null) {
            wscript.SendKeys("{F11}");
        }
    }
}
function exitFull() {
    //判断各种浏览器，找到正确的方法
    var exitMethod = document.exitFullscreen || //W3C
    document.mozCancelFullScreen || //Chrome等
    document.webkitExitFullscreen || //FireFox
    document.webkitExitFullscreen; //IE11
    if (exitMethod) {
        exitMethod.call(document);
    }
    else if (typeof window.ActiveXObject !== "undefined") {//for Internet Explorer
        var wscript = new ActiveXObject("WScript.Shell");
        if (wscript !== null) {
            wscript.SendKeys("{F11}");
        }
    }
}
document.querySelector("#fullscreen").onclick = function () {
    requestFullScreen(document.documentElement);
}