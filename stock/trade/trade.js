//begin: enum status//////////////
var TradeStatus = TradeStatus || {};
TradeStatus.error = -1;
TradeStatus.statistics = 0;
TradeStatus.closeline = 1;
TradeStatus.prline = 2;
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

var trade = trade || {};
trade.m_canvas = document.getElementById("tradecanvas");
trade.m_cvtext = trade.m_canvas.getContext("2d");
trade.m_canvas.width=window.innerWidth;
trade.m_canvas.height = window.innerHeight;
trade.m_status = TradeStatus.statistics;
trade.init = function () {
	trade.gdata = gdata.get();
	if (trade.gdata.tab == ViewTab.trade) {
        switch (trade.gdata.tabStatus) {
            case TradeStatus.statistics:
                tradestatistics.init();
                trade.m_status = TradeStatus.statistics;
                break;
            case TradeStatus.closeline:
                tradeclose.init();
                trade.m_status = TradeStatus.closeline;
                break;
            case TradeStatus.prline:
                tradepr.init();
                trade.m_status = TradeStatus.prline;
                break;
            default:
                console("ERROR: line.gdata.tabStatus[" + trade.gdata.tabStatus + "] out of range");
                break;
        }
    }
    else {
		trade.m_status = TradeStatus.statistics;
		gdata.tab = ViewTab.trade;
		gdata.set();
        tradestatistics.init();
    }
    return;
}
trade.init();

window.onload=function(){  
    function resizeCanvas(){
        trade.m_canvas.width=window.innerWidth;
        trade.m_canvas.height=window.innerHeight;
        switch (trade.m_status){
            case TradeStatus.statistics:
                tradestatistics.show();
                //tradestatistics.selectPoint();
                break;
            case TradeStatus.closeline:
                tradeclose.show();
				break;
			case TradeStatus.prline:
                tradepr.show();
    	        break;
            default:
                console.log("Error: trade.m_status: " + trade.m_status + " is out of range");
    	        break;
        }
	}
	window.onresize=resizeCanvas;
	
	function windowTocanvas(canvas, x, y) {  
        var bbox = trade.m_canvas.getBoundingClientRect();  
        return {  
            x: x - bbox.left * (trade.m_canvas.width / bbox.width),   
            y: y - bbox.top * (trade.m_canvas.height / bbox.height)  
        };  
	}

	window.onkeydown = function(ev) {
		var keyid = ev.keyCode ? ev.keyCode :ev.which;
		//console.log(keyid);
		switch (trade.m_status) {
		    case TradeStatus.statistics:
		        statisticskeydown(keyid);
		        break;
		    case TradeStatus.closeline:
		        closelinekeydown(keyid);
		        break;
		    case TradeStatus.prline:
		        prlinekeydown(keyid);
		        break;
		    default:
		        console.error("trade.m_status error. value: "<<trade.m_status);
		        break;
		}
	}

	function statisticskeydown(keyid) {
	    switch (keyid) {
	        case 37:  // 向左箭头，左移
			    tradestatistics.Zoom(ZoomType.left);
	            break;
	        case 38:  // 向上箭头，精度放大
			    tradestatistics.Zoom(ZoomType.add);
	            break;
	        case 39:  // 向右箭头，右移
			    tradestatistics.Zoom(ZoomType.right);
	            break;
	        case 40:  // 向下箭头，精度缩小
			    tradestatistics.Zoom(ZoomType.sub);
	            break;
	        case 68:  //d
	            if (68 == keyid) {
	                tradestatistics.m_strCycle = "day";
	            }
	        case 87:  // w
	            if (87 == keyid) {
	                tradestatistics.m_strCycle = "week";
	            }
	        case 77:  // m
	            if (77 == keyid) {
	                tradestatistics.m_strCycle = "month";
	            }
            case 89:  // year
                if (89 == keyid) {
                    tradestatistics.m_strCycle = "year";
                }
                //conf.set("kline", kline.m_conf);
                console.log("trade statistics cycle: " + tradestatistics.m_strCycle);
				if (false == tradestatistics.req()) {
					console.log("call tradestatistics.req() error. cycle: " + tradestatistics.m_strCycle);
					return false;
				}
				
				if (false == tradestatistics.show()) {
					console.log("call tradestatistics.show() error.");
					return false;
				}
	            break;
	        default:
	            break;
	    }

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

    trade.m_canvas.onclick=function(event){  
        
        var loc=windowTocanvas(trade.m_canvas,event.clientX,event.clientY)  
        var x=parseInt(loc.x);  
        var y=parseInt(loc.y);  
        //document.getElementById("input_window").value=event.clientX+"--"+event.clientY;  
        //document.getElementById("input_canvas").value=x+"--"+y;  
    }  

	trade.m_canvas.onmousedown = function(ev){
        var ev=ev || window.event;
    
        switch (trade.m_status){
			case TradeStatus.statistics:
				tradestatistics.MouseDown();
				break;
			case TradeStatus.closeline:
	            tradeclose.mousedown(ev);
	            break;
	        case TradeStatus.prline:
	            tradepr.mousedown(ev);
	            break;
            default:
    	        console.log("Error: trade.m_status: " + trade.m_nStatus + " is out of range");
    	        break;
        }
	}
	
	trade.m_canvas.onmousemove = function (ev) {
	    var ev = ev || window.event;
	    switch (trade.m_status) {
			case TradeStatus.statistics:
				tradestatistics.MouseMove(ev);
	            break;
	        case TradeStatus.closeline:
	            tradeclose.mousemove(ev);
	            break;
	        case TradeStatus.prline:
	            tradepr.mousemove(ev);
	            break;
	        default:
	            console.log("Error: trade.m_status: " + trade.m_Status + " is out of range");
	            break;
	    }
	}
} 