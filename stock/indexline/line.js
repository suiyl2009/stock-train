//begin: enum status//////////////
var LineStatus = LineStatus || {};
LineStatus.error = -1;
LineStatus.list = 0;
LineStatus.single = 1;
LineStatus.batch = 2;
LineStatus.kline = 3;
LineStatus.pageUp = 4;
LineStatus.pageDn = 5;
LineStatus.pageMd = 6;
//end: enum status//////////////

//begin: zoom //////////////////
var ZoomType = ZoomType || {};
ZoomType.sub = 0;  // 精度缩小
ZoomType.add = 1;  // 精度扩大
ZoomType.left = 2;  // 左移
ZoomType.right = 3;  // 右移
ZoomType.begin = 4;  // 定位到开始处
ZoomType.end = 5;  // 定位到结束处
//end: zoom //////////////////

function CMValue() {
    this.m_iMax = 0;
    this.m_iMin = 0;
}

var line = line || {};
line.m_canvas = document.getElementById("linecanvas");
line.m_cvtext = list.m_canvas.getContext("2d");
line.m_status = LineStatus.list;
line.m_canvas.width=window.innerWidth;
line.m_canvas.height = window.innerHeight;
line.init = function () {
    line.gdata = gdata.get();
    if (line.gdata.tab == ViewTab.line) {
        switch (line.gdata.tabStatus) {
            case LineStatus.list:
                list.init();
                line.m_status = LineStatus.list;
                break;
            case LineStatus.single:
                single.init();
                line.m_status = LineStatus.single;
                break;
            case LineStatus.batch:
                batch.init();
                line.m_status = LineStatus.batch;
                break;
            case LineStatus.kline:
                kline.init();
                line.m_status = LineStatus.kline;
                break;
            default:
                console("ERROR: line.gdata.tabStatus[" + line.gdata.tabStatus + "] out of range");
                break;
        }
    }
    else {
		line.m_status = LineStatus.list;
		gdata.tab = ViewTab.line;
		gdata.set();
        list.init();
    }
    return;
}
line.init();

window.onload=function(){  
    function resizeCanvas(){
        line.m_canvas.width=window.innerWidth;
        line.m_canvas.height=window.innerHeight;
        switch (line.m_status){
            case LineStatus.list:
                list.show();
                list.selectRow();
                break;
            case LineStatus.single:
                single.show();
    	        break;
            case LineStatus.batch:
                batch.show();
                break;
            case LineStatus.kline:
                kline.show();
    	        break;
            default:
                console.log("Error: line.m_status: " + line.m_nStatus + " is out of range");
    	        break;
        }
	}
	window.onresize=resizeCanvas;
	
	function windowTocanvas(canvas, x, y) {  
        var bbox = line.m_canvas.getBoundingClientRect();  
        return {  
            x: x - bbox.left * (line.m_canvas.width / bbox.width),   
            y: y - bbox.top * (line.m_canvas.height / bbox.height)  
        };  
	}

	window.onkeydown = function(ev) {
		var keyid = ev.keyCode ? ev.keyCode :ev.which;
		//console.log(keyid);
		switch (line.m_status) {
		    case LineStatus.list:
		        listkeydown(keyid);
		        break;
		    case LineStatus.single:
		        singlekeydown(keyid);
		        break;
		    case LineStatus.batch:
		        batchkeydown(keyid);
		        break;
		    case LineStatus.kline:
		        klinekeydown(keyid);
		        break;
		    default:
		        console.error("line.m_status error. value: "<<line.m_status);
		        break;
		}
	}

	function listkeydown(keyid) {
	    return true;
	}

	function singlekeydown(keyid) {
	    switch (keyid) {
	        case 8:  // backspace
	            line.m_status = LineStatus.list;
	            list.show();
	            list.selectRow();
	            break;
	        case 37:  // 向左箭头，左移
	            single.Zoom(ZoomType.left);
	            break;
	        case 38:  // 向上箭头，精度放大
	            single.Zoom(ZoomType.add);
	            break;
	        case 39:  // 向右箭头，右移
	            single.Zoom(ZoomType.right);
	            break;
	        case 40:  // 向下箭头，精度缩小
	            single.Zoom(ZoomType.sub);
	            break;
	        default:
	            break;
	    }

	    return true;
	}

	function batchkeydown(keyid) {
	    switch (keyid) {
	        case 8:  // backspace
	            line.m_status = LineStatus.list;
	            list.show();
	            list.selectRow();
	            break;
	        case 37:  // 向左箭头，左移
	            batch.Zoom(ZoomType.left);
	            break;
	        case 38:  // 向上箭头，精度放大
	            batch.Zoom(ZoomType.add);
	            break;
	        case 39:  // 向右箭头，右移
	            batch.Zoom(ZoomType.right);
	            break;
	        case 40:  // 向下箭头，精度缩小
	            batch.Zoom(ZoomType.sub);
	            break;
	        case 66:  // b
	            batch.Zoom(ZoomType.begin);
	            break;
	        case 69:  // e
	            batch.Zoom(ZoomType.end);
	            break;
	        default:
	            break;
	    }

	    return true;
	}

	function klinekeydown(keyid) {
	    //console.log("klinekeydown: " + keyid);
	    if (false == kline.m_bKeyValid) {
	        return true;
	    }

	    switch (keyid) {
	        case 8:  // backspace
	            line.m_status = LineStatus.list;
	            list.show();
	            list.selectRow();
	            break;
	        case 37:  // 向左箭头，左移
	            kline.Zoom(ZoomType.left);
	            break;
	        case 38:  // 向上箭头，精度放大
	            kline.Zoom(ZoomType.add);
	            break;
	        case 39:  // 向右箭头，右移
	            kline.Zoom(ZoomType.right);
	            break;
	        case 40:  // 向下箭头，精度缩小
	            kline.Zoom(ZoomType.sub);
	            break;
	        case 187:  // =  +
	            kline.NextK();
	            break;
	        case 68:  //d
	            if (68 == keyid) {
	                kline.m_conf.cycle = "day";
	            }
	        case 87:  // w
	            if (87 == keyid) {
	                kline.m_conf.cycle = "week";
	            }
	        case 77:  // m
	            if (77 == keyid) {
	                kline.m_conf.cycle = "month";
	            }
            case 89:  // year
                if (89 == keyid) {
                    kline.m_conf.cycle = "year";
                }
                conf.set("kline", kline.m_conf);
                console.log("cycle: " + kline.m_conf.cycle);
	            if (false == kline.GetLineData(kline.m_conf.cycle, 0, 0, 0)) {
	                console.log("Call kline.GetLineData error");
	                return false;
	            }

	            if (false == kline.show()) {
	                console.log("Call kline.show error");
	                return false;
	            }
	            break;
	        default:
	            break;
	    }

	    return true;
	}
	
	window.oncontextmenu = function (e) {
	    //取消默认的浏览器自带右键 很重要！！
	    e.preventDefault();

	    //console.log('right menu');
	    switch (line.m_status) {
	        case LineStatus.list:
	            list.rightMenu(e);
	            break;
	        case LineStatus.single:
	            single.rightMenu(e);
	            break;
	        case LineStatus.batch:
	            batch.rightMenu(e);
	            break;
	        case LineStatus.kline:
	            kline.rightMenu(e);
	            break;
	        default:
	            console.error("line.m_status error. value: " << line.m_status);
	            break;
	    }

	    /*//获取我们自定义的右键菜单
        var menu = document.querySelector("#menu");
    
        //根据事件对象中鼠标点击的位置，进行定位
        menu.style.left = e.clientX + 'px';
        menu.style.top = e.clientY + 'px';
    
        //改变自定义菜单的宽，让它显示出来
        menu.style.width = '125px';*/
	}

    //关闭右键菜单，很简单
	window.onclick = function (e) {

	    //用户触发click事件就可以关闭了，因为绑定在window上，按事件冒泡处理，不会影响菜单的功能
	    document.querySelector('#rightmenu').style.width = 0;
	    document.querySelector('#rightmenu').style.height = 0;
	}

    line.m_canvas.onclick=function(event){  
        
        var loc=windowTocanvas(line.m_canvas,event.clientX,event.clientY)  
        var x=parseInt(loc.x);  
        var y=parseInt(loc.y);  
        //document.getElementById("input_window").value=event.clientX+"--"+event.clientY;  
        //document.getElementById("input_canvas").value=x+"--"+y;  
    }  

	line.m_canvas.onmousedown = function(ev){
        var ev=ev || window.event;
    
        switch (line.m_status){
            case LineStatus.list:
                var status = list.selectRow(ev);
                console.log("status: " + status);
                switch (status) {
                    case LineStatus.list:
                        break;
                    case LineStatus.single:
                        if (false == single.init()) {
                            console.log("Error: single.init error. code: " + list.m_data[list.m_selectRow].Code);
                            return;
                        }
                        line.m_status = LineStatus.single;
                        break;
                    case LineStatus.batch:
                        if (false == batch.init()) {
                            console.log("Error: batch.init error. code: " + list.m_data[list.m_selectRow].Code);
                            return;
                        }
                        line.m_status = LineStatus.batch;
                        break;
                    case LineStatus.kline:
                        if (false == kline.init()) {
                            console.log("Error: kline.init error. code: " + list.m_data[list.m_selectRow].Code);
                            return;
                        }
                        line.m_status = LineStatus.kline;
                        break;
                    case LineStatus.pageUp:
                    case LineStatus.pageDn:
                    case LineStatus.pageMd:
                        if (false == list.ModifyPage(status)) {
                            console.error("Error: list.ModifyPage error. status: " + status);
                            return;
                        }
                        line.m_status = LineStatus.list;
                        break;
                    default:
                        console.log("ERROR: call list.selectRow(ev) status[" + status + "] error");
                        break;
                }
    	        break;
            case LineStatus.single:
                single.MouseDown(ev);
    	        break;
            case LineStatus.batch:
                batch.mousedown(ev);
    	        break;
            case LineStatus.kline:
                kline.mousedown(ev);
    	        break;
            default:
    	        console.log("Error: line.m_status: " + line.m_nStatus + " is out of range");
    	        break;
        }
	}
	
	line.m_canvas.onmousemove = function (ev) {
	    var ev = ev || window.event;

	    switch (line.m_status) {
	        case LineStatus.list:
	            break;
	        case LineStatus.single:
	            single.MouseMove(ev);
	            break;
	        case LineStatus.batch:
	            batch.mousemove(ev);
	            break;
	        case LineStatus.kline:
	            kline.mousemove(ev);
	            break;
	        default:
	            console.log("Error: line.m_status: " + line.m_nStatus + " is out of range");
	            break;
	    }
	}
} 