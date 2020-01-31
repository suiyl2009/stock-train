var odiv = document.createElement('div');
odiv.id = "menu";
odiv.innerHTML =
    '<div id="menu">  \
        <div id="hiden"></div>  \
        <div id="slcd">  \
            <ul>  \
                <li id="login">  \
                    用户  \
                </li>  \
                <li id="linemenu">曲线</li>  \
                <li id="trademenu">买卖点</li>    \
                <li style="width:80px;">训练统计</li>  \
                <li>配置  \
                   <div class="zcd" id="zcd1">  \
                        <ul>  \
                            <li id="dateconf">日期</li>  \
                            <li id="kindconf">分类</li>  \
                            <li id="listconf">列表</li>  \
                            <li id="lineconf">曲线</li>  \
                            <li id="klineconf">K线</li>  \
                            <li id="condconf">条件</li>  \
                            <li id="trainconf">训练</li> \
                            <li id="defaultconf">默认</li> \
                        </ul>  \
                    </div>  \
                </li>  \
                <li>帮助  \
                  <div class="zcd" id="zcd2"> \
                        <ul>  \
                            <li><a target="_blank" href="http://39.96.2.239/puyublog/index/article/index/16">使用说明</a></li>  \
                            <li><a href="/index.html">首页导航</a></li>  \
                        </ul>  \
                  </div> \
                </li>\
                <li id="indexline">指数</li>  \
                <li id="fullscreen">全屏</li>  \
                <li id="movehide">&#8681&#8681</li>  \
            </ul>  \
        </div>  \
        <div id="divIframe" style="border:0px;">  \
            <button id="mybtn" style="position:absolute;top:0;right:0"><img src="../btn-close.png" /></button>  \
            <iframe id="myiframe" frameborder="0" scrolling="no" style="border:0px;"></iframe>  \
        </div>  \
    </div>';
document.getElementById('body').appendChild(odiv);

document.getElementById("login").onclick = function () {
    //javascript:window.open("../user/login.html");
    user.get();
    if ("" == user.m_data.sessionid) {
        window.location.href = "../user/login.html";
    }
    else {
        window.location.href = "../user/info.html";
    }
    
}

document.getElementById("linemenu").onclick = function () {
    //javascript:window.open("../user/login.html");
    window.location.href = "../index.html";
}

document.getElementById("trademenu").onclick = function () {
    //javascript:window.open("../user/login.html");
    window.location.href = "../trade/trade.html";
}

var diag = document.getElementById("divIframe");
var menuitem = 0;
document.getElementById("dateconf").onclick = function () {
    console.log("block");
    diag.style.height = '130px';
    diag.style.width = '300px';
    document.getElementById("myiframe").style.background = 'rgba(244, 245, 245, 0.8)';
    document.getElementById("myiframe").src = "../conf/datediag.html";
    document.getElementById("myiframe").style.height = '130px';
    document.getElementById("myiframe").style.width = '300px';
    diag.style.display = 'block';
    diag.style.background = 'rgba(244, 245, 245, 0.8)';

    menuitem = 1;

    return;
}
document.getElementById("kindconf").onclick = function () {
    console.log("block");
    diag.style.height = '575px';
    diag.style.width = '800px';
    document.getElementById("myiframe").style.background = 'rgba(244, 245, 245, 0.8)';
    document.getElementById("myiframe").src = "../conf/kinddiag.html";
    document.getElementById("myiframe").style.height = '575px';
    document.getElementById("myiframe").style.width = '800px';
    diag.style.display = 'block';
    diag.style.background = 'rgba(244, 245, 245, 0.8)';

    menuitem = 2;

    return;
}
document.getElementById("listconf").onclick = function () {
    console.log("block");
    diag.style.height = '622px';
    diag.style.width = '522px';
    document.getElementById("myiframe").src = "../conf/listdiag.html";
    document.getElementById("myiframe").style.height = '622px';
    document.getElementById("myiframe").style.width = '522px';
    diag.style.display = 'block';
    diag.style.border = '5px solid #808080';

    menuitem = 3;

    return;
}
document.getElementById("lineconf").onclick = function () {
    console.log("block");
    diag.style.height = '622px';
    diag.style.width = '522px';
    document.getElementById("myiframe").src = "../conf/linediag.html";
    document.getElementById("myiframe").style.height = '622px';
    document.getElementById("myiframe").style.width = '522px';
    diag.style.display = 'block';
    diag.style.border = '5px solid #808080';

    menuitem = 4;

    return;
}
document.getElementById("condconf").onclick = function () {
    console.log("block");
    diag.style.height = '560px';
    diag.style.width = '900px';
    document.getElementById("myiframe").src = "../conf/conddiag.html";
    document.getElementById("myiframe").style.height = '560px';
    document.getElementById("myiframe").style.width = '900px';
    diag.style.display = 'block';
    diag.style.border = '5px solid #808080';

    menuitem = 5;

    return;
}
document.getElementById("trainconf").onclick = function () {
    console.log("block");
    diag.style.height = '300px';
    diag.style.width = '450px';
    document.getElementById("myiframe").src = "../conf/traindiag.html";
    document.getElementById("myiframe").style.height = '300px';
    document.getElementById("myiframe").style.width = '450px';
    diag.style.display = 'block';
    diag.style.border = '5px solid #808080';

    menuitem = 6;

    return;
}
document.getElementById("mybtn").onclick = function () {
    diag.style.display = 'none';

    console.log("gdata.m_data.tab: " + gdata.m_data.tab + ", ViewTab.line: " + ViewTab.line
        + ", menuitem: " + menuitem);
    if ((menuitem == 1) && (gdata.m_data.tab == ViewTab.line)) {
        line.init();
    }

    if ((menuitem == 2) && (gdata.m_data.tab == ViewTab.line)) {
        line.init();
    }

    if ((menuitem == 3) && (gdata.m_data.tab == ViewTab.line) && (gdata.m_data.tabStatus == LineStatus.list)) {
        line.init();
    }

    if ((menuitem == 4) && (gdata.m_data.tab == ViewTab.line)) {
        if ((gdata.m_data.tabStatus == LineStatus.single) || (gdata.m_data.tabStatus == LineStatus.batch)) {
            line.init();
        }
    }

    if ((menuitem == 5) && (gdata.m_data.tab == ViewTab.line)) {
        line.init();
    }
}

document.getElementById("indexline").onclick = function () {
    //javascript:window.open("../user/login.html");
    window.location.href = "../indexline/indexline.html";
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
