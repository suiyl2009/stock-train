var cstrCycleDay = "day";
var cstrCycleWeek = "week";
var cstrCycleMonth = "month";
var cstrCycleYear = "year";

function CKPoint()
{
    this.m_iOpen = 0;
    this.m_iClose = 0;
    this.m_iHigh = 0;
    this.m_iLow = 0;
}

var EClickRect = {};
EClickRect.eClickReserve  = 0;
EClickRect.eClickTrade    = 1;

function CClickRect (){
    this.m_eType = EClickRect.eClickReserve;
    this.m_iTop = 0;
    this.m_iLeft = 0;
    this.m_iHeight = 0;
    this.m_iWidth = 0;

    this.Reset = function(){
        this.m_eType = EClickRect.eClickReserve;
        this.m_iTop = 0;
        this.m_iLeft = 0;
        this.m_iHeight = 0;
        this.m_iWidth = 0;
    }

    this.InRect = function(x, y){
        if ((x > this.m_iLeft) && (x < this.m_iLeft + this.m_iWidth) && (y > this.m_iTop) && (y < this.m_iTop + this.m_iHeight))
        {
            return true;
        }

        return false;
    }
}

var kline = kline || {};

kline.m_canvas = document.getElementById("linecanvas");
kline.m_cvtext = kline.m_canvas.getContext("2d");

kline.init = function () {
    console.log("kline.init");
    // 配置
    //console.log(conf.m_def.kline);
    //conf.set("kline", conf.m_def.kline);
    kline.m_conf = conf.get('kline');
    kline.m_global = conf.get('global');
    kline.m_user = user.get();

    // view 画点数区域
    kline.m_iPointTop = 0;
    kline.m_iPointLeft = 0;
    kline.m_iPointHeight = 0;
    kline.m_iPointWidth = 0;

    // view 画vol区域
    kline.m_iVolTop = 0;
    //m_iVolLeft = 0;
    kline.m_iVolHeight = 0;
    //m_iVolWidth = 0;

    // view 显示信息区域
    kline.m_iInfoTop = 0;
    kline.m_iInfoLeft = 0;
    kline.m_iInfoHeight = 0;
    kline.m_iInfoWidth = 0;

    // 当前位置
    kline.m_iMouseX = 0;
    kline.m_iMouseY = 0;
    kline.m_iCurrDataIdx = 0;

    kline.m_bCross = false;  // 是否画十字星
    kline.m_bKeyValid = true; // 按键是否有效

    // 基本配置信息
    kline.m_strCode = "";            // 股票代码
    kline.m_strCycle = "";   // 周期，日周月年
    //m_iRecover  = 0;              // 是否复权
    //m_iShowCnt  = 200;            // 一屏显示元素数量

    // 远程获取的数据
    kline.m_iKMax = 0;
    kline.m_iKMin = 0;
    kline.m_iVolMax = 0;
    kline.m_iVolMin = 0;
    kline.m_vecData = [];
    kline.m_iMaxInterval = 0;  // 0：左右数据都没完全加载到客户端；1：左边数据加载完；2：右边数据加载完；3：左右数据都加载完

    // 曲线点数数据
    kline.m_idxBegin = 0; // 显示点数在m_vecData下标范围
    kline.m_idxEnd = 0;

    kline.m_iShowKMax = 0; // 显示区段最大值
    kline.m_iShowKMin = 0; // 显示区段最小值
    kline.m_iShowVolMax = 0;
    kline.m_iShowVolMin = 0;
    kline.m_dw = 0;
    kline.m_vecKPoint = [];
    kline.m_vecXPoint = [];
    kline.m_vecVolPoint = [];

    kline.m_vecInfoClick = [];

    /*
    m_ptrKLineTabView = ptrTabView;
	if (NULL == m_ptrKLineTabView)
	{
		strRetCode = CERRPARAM;
		ERRORLOG("NULL == m_ptrKLineTabView. strRetCode: "<<strRetCode);
		return 1;
	}

	m_pAvgLine = new CAvgLine;
	if (0 != m_pAvgLine->Init(this, strRetCode))
	{
		ERRORLOG("Call m_pAvgLine->Init(this, strRetCode) error. strRetCode: "<<strRetCode);
		return 1;
	}
    */

    if (false == kline.GetLineData(kline.m_conf.cycle, 0, 0, 0)) {
        console.log("Call kline.GetLineData error");
        return false;
    }

    if (false == kline.show()) {
        console.log("Call kline.show error");
        return false;
    }

    return true;
}

kline.GetLineData = function(strCycle, iDate, iBefore, iAfter)
{
    // 配置
    kline.m_conf = conf.get('kline');
    kline.m_global = conf.get('global');
    kline.m_user = user.get();

    // 清空数据
    kline.m_vecData = [];

    kline.m_idxBegin = 0;
    kline.m_idxEnd = 0;
    kline.m_iShowKMax = 0;
    kline.m_iShowKMin = 0;
    kline.m_iShowVolMax = 0;
    kline.m_iShowVolMin = 0;

    kline.m_vecXPoint = [];   // X轴坐标，全局统一
    kline.m_vecKPoint = [];   // K线的Y轴坐标
    kline.m_vecVolPoint = []; // 成交量Y轴坐标

    // 设置配置信息
    if ((0 == iDate) && (0 == iBefore) && (0 == iAfter))
    {
        iDate = kline.m_global.date;
        iBefore = kline.m_conf.kcount;
        iAfter = 0;
    }

    // 得到股票代码
    idxdata.get();
    if ((0 <= idxdata.m_data.idx) && (idxdata.m_data.codes.length > idxdata.m_data.idx)){
        kline.m_strCode = idxdata.m_data.codes[idxdata.m_data.idx];
        console.log("kline.m_strCode" + kline.m_strCode + ", idx:" + idxdata.m_data.idx);
    }
    else {
        console.log("ERROR: idxdata.m_data.idx: " + " error. idxdata.m_data.codes.length: " + idxdata.m_data.codes.length);
        return false;
    }
    
    if (false == kline.GetCycleData(strCycle, iDate, iBefore, iAfter))
    {
        console.log("Call GetDayData error. iDate: " + iDate + ", iBefore: " + iBefore + ", iAfter: " + iAfter);
        return false;
    }
    kline.m_strCycle = strCycle;

    if (false == kline.GetShowMValue()) {  // 得到显示区段最值
        console.error("Call GetShowMValue.");
        return false;
    }

    /*iRet = m_pAvgLine->GetData(strRetCode);
    if (0 != iRet)
    {
        ERRORLOG("Call m_pAvgLine->GetData error. strRetCode: "<<strRetCode);
        return iRet;
    }

    if (true == CConfig::Instance()->m_objSingle.m_bTradePoint)
    {
        vector<string> vecCode;
        vecCode.push_back(m_strCode);
        if (NULL == CUserInfo::Instance()->m_pPoint)
        {
            CUserInfo::Instance()->m_pPoint = new CTradePoint;
        }
        iRet = CUserInfo::Instance()->m_pPoint->Search(CUserInfo::Instance()->m_ullUserID, vecCode, m_strCycle, 0, strRetCode);
        if (0 != iRet)
        {
            ERRORLOG("Call CUserInfo::Instance()->m_pPoint->Search error. strRetCode: " << strRetCode);
            return iRet;
        }
    }*/

    return true;
}


/*
请求字段列表
字段名称	值类型	备注
method	字符串kline.line
code	字符串	股票代码
cycle	字符串	day, week, month, year
recover	整数	0：不复权；1：向前复权；2：向后复权
date	整型或空	结束日期，例如20170121
before
after

响应字段列表
字段名称	值类型	备注
method	同上	同上
retcode	字符串	错误码，成功为0000
result	数组{“kline”: [[],[],[]…], “max”: xxx, “min”: xxx, “share”: [{“date”: xxx, “vol”: xxx}, {“date”: xxx, “vol”: xxx}, …]}	响应字段值。数组元素为

kline
字段意义	值类型	备注
date	int	日期
open	int	开盘
close	int	收盘
high	int	最高
low	int	最低
amount	int	成交额
vol	int	成交量
daycount	int	几天
*/
kline.GetCycleData = function (strCycle, iDate, iBefore, iAfter)
{
    var req = 
        {
            "method":"index.kline",
            "code": kline.m_strCode,
            "cycle": strCycle,
            "date": iDate,
            "before": iBefore,
            "after": iAfter
        };
    console.log('kline.req: ' + JSON.stringify(req));
    function remoteKline(objJson) {
        if (objJson.retcode != '0000') {
            console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
            return false;
        }
        kline.m_iKMax = objJson.result.kmax;
        kline.m_iKMin = objJson.result.kmin;
        kline.m_iVolMax = objJson.result.volmax;
        kline.m_iVolMin = objJson.result.volmin;
        kline.m_vecData = objJson.result.kline;

        return true;
    }
    idxdata.send(req, remoteKline);

	// 显示坐标区域
	kline.m_idxBegin = 0;
	kline.m_idxEnd = kline.m_vecData.length;

	// 显示区间最值
	kline.m_iShowKMax = kline.m_iKMax;
	kline.m_iShowKMin = kline.m_iKMin;
	kline.m_iShowVolMax = kline.m_iVolMax;
	kline.m_iShowVolMin = kline.m_iVolMin;

	return true;
}

kline.show = function () {
    //console.log("kline.show");

    //设置颜色
    kline.m_cvtext.clearRect(0, 0, kline.m_canvas.width, kline.m_canvas.height);
    kline.m_cvtext.beginPath();
    kline.m_cvtext.fillStyle = "#ffffff";
    kline.m_cvtext.font = "20px SimSun";

    // 左侧曲线区域
    kline.m_iPointTop = 2;
    kline.m_iPointLeft = 2;
    kline.m_iPointHeight = (kline.m_canvas.height - 35) * 5 / 6; // 与vol按4:1分高度;
    kline.m_iPointWidth = kline.m_canvas.width - 200;
    kline.m_iVolTop = kline.m_iPointTop + kline.m_iPointHeight;
    kline.m_iVolHeight = (kline.m_canvas.height - 35) / 6;

    // 右侧信息区域
    kline.m_iInfoTop = 2;
    kline.m_iInfoLeft = kline.m_iPointLeft + kline.m_iPointWidth;
    kline.m_iInfoHeight = kline.m_iPointHeight + kline.m_iVolHeight;
    kline.m_iInfoWidth = kline.m_canvas.width - 2 - kline.m_iInfoLeft;

    //画左面K的边框
    //-------------->
    //              |
    //<------------\|/
    kline.m_cvtext.lineWidth = 1.5;
    kline.m_cvtext.beginPath();
    kline.m_cvtext.strokeStyle = "#ff0000";
    kline.m_cvtext.moveTo(kline.m_iPointLeft, kline.m_iPointTop);
    kline.m_cvtext.lineTo(kline.m_iPointLeft + kline.m_iPointWidth, kline.m_iPointTop);
    kline.m_cvtext.lineTo(kline.m_iPointLeft + kline.m_iPointWidth, kline.m_iPointTop + kline.m_iPointHeight);
    kline.m_cvtext.lineTo(kline.m_iPointLeft, kline.m_iPointTop + kline.m_iPointHeight);
    kline.m_cvtext.lineTo(kline.m_iPointLeft, kline.m_iPointTop);
    kline.m_cvtext.stroke();

    //画左面vol的边框
    kline.m_cvtext.lineWidth = 1.5;
    kline.m_cvtext.beginPath();
    kline.m_cvtext.strokeStyle = "#ff0000";
    kline.m_cvtext.moveTo(kline.m_iPointLeft, kline.m_iVolTop);
    kline.m_cvtext.lineTo(kline.m_iPointLeft + kline.m_iPointWidth, kline.m_iVolTop);
    kline.m_cvtext.lineTo(kline.m_iPointLeft + kline.m_iPointWidth, kline.m_iVolTop + kline.m_iVolHeight);
    kline.m_cvtext.lineTo(kline.m_iPointLeft, kline.m_iVolTop + kline.m_iVolHeight);
    kline.m_cvtext.lineTo(kline.m_iPointLeft, kline.m_iVolTop);
    kline.m_cvtext.stroke();

    //画右面的边框
    kline.m_cvtext.lineWidth = 1.5;
    kline.m_cvtext.beginPath();
    kline.m_cvtext.strokeStyle = "#ff0000";
    kline.m_cvtext.moveTo(kline.m_iInfoLeft, kline.m_iInfoTop);
    kline.m_cvtext.lineTo(kline.m_iInfoLeft + kline.m_iInfoWidth, kline.m_iInfoTop);
    kline.m_cvtext.lineTo(kline.m_iInfoLeft + kline.m_iInfoWidth, kline.m_iInfoTop + kline.m_iInfoHeight);
    kline.m_cvtext.lineTo(kline.m_iInfoLeft, kline.m_iInfoTop + kline.m_iInfoHeight);
    kline.m_cvtext.lineTo(kline.m_iInfoLeft, kline.m_iInfoTop);
    kline.m_cvtext.stroke();

    // 画八条虚线
    for (var y = kline.m_iPointHeight + kline.m_iPointTop - kline.m_iPointHeight / 9;
    y > kline.m_iPointTop + 10;y -= kline.m_iPointHeight / 9){
        kline.m_cvtext.lineWidth = 0.5;
        kline.m_cvtext.beginPath();
        kline.m_cvtext.strokeStyle = "#ff0000";
        kline.m_cvtext.moveTo(kline.m_iPointLeft, y);
        kline.m_cvtext.lineTo(kline.m_iPointLeft + kline.m_iPointWidth, y);
        kline.m_cvtext.stroke();
    }

    // 生成曲线点数
    if (false == kline.GetPoint()){
        console.log("Call GetPoint error.");
        return false;
    }

    // 画左侧k线及下方成交量
    if (false == kline.DrawKPoint()) {
        console.log("Call DrawPoint error.");
        return false;
    }

    // 画曲线显示信息
    if (false == kline.DrawSign()) {
        console.log("Call DrawSign error.");
        return false;
    }

    // 画右侧显示信息
    if (false == kline.DrawInfo()) {
        console.log("Call DrawInfo error.");
        return false;
    }

    // 画均线
    /*iRet = m_pAvgLine->Draw(strRetCode);
    if (0 != iRet)
    {
        ERRORLOG("Call m_pAvgLine->Draw error. strRetCode: " << strRetCode);
        return iRet;
    }*/

    return true;
}

kline.GetPoint = function () {
	var dh = 0.0;   // 每个点高度
	var dvol = 0.0; // 每个vol点高度

	kline.m_vecXPoint = [];
	kline.m_vecKPoint = [];
	kline.m_vecVolPoint = [];

	if (0 >= kline.m_idxEnd - kline.m_idxBegin)
	{
		return true;
	}

	// 计算X轴坐标
	kline.m_dw = 1.0*kline.m_iPointWidth / (kline.m_idxEnd - kline.m_idxBegin);  // 每个点的宽度
	for (var i = kline.m_idxBegin; (i < kline.m_idxEnd) && (i < kline.m_vecData.length); i++)  // 从右向左画
	{
	    kline.m_vecXPoint.push(kline.m_iPointLeft + kline.m_iPointWidth - (i - kline.m_idxBegin)*kline.m_dw - 0.5*kline.m_dw); // X轴坐标点
	}

	// 计算Y轴坐标, k线和vol
	dh = (kline.m_iShowKMax == kline.m_iShowKMin) ? 0 : (1.0*kline.m_iPointHeight / (kline.m_iShowKMax - kline.m_iShowKMin));
	dvol = (kline.m_iShowVolMax == kline.m_iShowVolMin) ? 0 : (1.0*kline.m_iVolHeight / (kline.m_iShowVolMax - kline.m_iShowVolMin));
	for (var i = kline.m_idxBegin; (i < kline.m_idxEnd) && (i < kline.m_vecData.length); i++)
	{
	    /*
        kline
        字段意义	值类型	备注
        date	int	日期
        open	int	开盘
        close	int	收盘
        high	int	最高
        low	int	最低
        amount	int	成交额
        vol	int	成交量
        daycount	int	几天
        */
	    var objKPoint = new CKPoint();
	    objKPoint.m_iClose = kline.m_iPointTop + kline.m_iPointHeight - (kline.m_vecData[i][2] - kline.m_iShowKMin)*dh;  // close
	    objKPoint.m_iHigh = kline.m_iPointTop + kline.m_iPointHeight - (kline.m_vecData[i][3] - kline.m_iShowKMin)*dh;  // high
	    objKPoint.m_iLow = kline.m_iPointTop + kline.m_iPointHeight - (kline.m_vecData[i][4] - kline.m_iShowKMin)*dh;  // low
	    objKPoint.m_iOpen = kline.m_iPointTop + kline.m_iPointHeight - (kline.m_vecData[i][1] - kline.m_iShowKMin)*dh;  // open
	    kline.m_vecKPoint.push(objKPoint); // k线点数
	    kline.m_vecVolPoint.push(kline.m_iVolTop + kline.m_iVolHeight - (kline.m_vecData[i][6] - kline.m_iShowVolMin)*dvol); // vol点数
	}

	/*iRet = m_pAvgLine->GetPoint(strRetCode);  // 得到均线坐标点
	if (0 != iRet)
	{
		ERRORLOG("Call m_pAvgLine->GetPoint error. strRetCode: "<<strRetCode);
		return iRet;
	}*/

	return true;
}

kline.DrawKPoint = function () {
	for (var i = 0; i < kline.m_vecXPoint.length; i++)
	{
	    kline.m_cvtext.lineWidth = 1;
	    kline.m_cvtext.beginPath();
	    if (kline.m_vecKPoint[i].m_iClose <= kline.m_vecKPoint[i].m_iOpen) //收盘价大于开盘价为阳线
	    {
	        kline.m_cvtext.strokeStyle = "#FF0000";
	        kline.m_cvtext.fillStyle = "#FF0000";
	        //////////////////////begin: 画K线/////////////////////////
	        kline.m_cvtext.moveTo(kline.m_vecXPoint[i], kline.m_vecKPoint[i].m_iHigh); // 与下行一起画高低点
	        kline.m_cvtext.lineTo(kline.m_vecXPoint[i], kline.m_vecKPoint[i].m_iClose);
	        kline.m_cvtext.moveTo(kline.m_vecXPoint[i], kline.m_vecKPoint[i].m_iOpen); // 与下行一起画高低点
	        kline.m_cvtext.lineTo(kline.m_vecXPoint[i], kline.m_vecKPoint[i].m_iLow);
	        kline.m_cvtext.rect( // 设置矩形
                kline.m_vecXPoint[i] - 0.5 * kline.m_dw,
                kline.m_vecKPoint[i].m_iOpen,
                kline.m_dw,
                kline.m_vecKPoint[i].m_iClose - kline.m_vecKPoint[i].m_iOpen
                );
	        //////////////////////end: 画K线/////////////////////////
	        /////////////////////begin: 画vol成交量////////////////////
	        kline.m_cvtext.rect(  // 画矩形
                kline.m_vecXPoint[i] - 0.5 * kline.m_dw,
                kline.m_vecVolPoint[i],
                kline.m_dw,
                kline.m_iVolTop + kline.m_iVolHeight - kline.m_vecVolPoint[i]
                );
	        /////////////////////end: 画vol成交量////////////////////
	        kline.m_cvtext.stroke();//绘制曲线及边框边框
		}
		else
	    {
	        kline.m_cvtext.strokeStyle = "#00FFFF"; //"#40B0D0";
	        kline.m_cvtext.fillStyle = "#00FFFF"; //"#40B0D0";
	        //////////////////////begin: 画K线/////////////////////////
	        kline.m_cvtext.moveTo(kline.m_vecXPoint[i], kline.m_vecKPoint[i].m_iHigh); // 与下行一起画高低点
	        kline.m_cvtext.lineTo(kline.m_vecXPoint[i], kline.m_vecKPoint[i].m_iLow);
	        kline.m_cvtext.stroke();
	        kline.m_cvtext.fillRect(  // 填充矩形
                kline.m_vecXPoint[i] - 0.5 * kline.m_dw,
                kline.m_vecKPoint[i].m_iOpen,
                kline.m_dw,
                kline.m_vecKPoint[i].m_iClose - kline.m_vecKPoint[i].m_iOpen
                );
	        //////////////////////end: 画K线/////////////////////////
	        /////////////////////begin: 画vol成交量////////////////////
	        kline.m_cvtext.fillRect(  // 画矩形
                kline.m_vecXPoint[i] - 0.5 * kline.m_dw,
                kline.m_vecVolPoint[i],
                kline.m_dw,
                kline.m_iVolTop + kline.m_iVolHeight - kline.m_vecVolPoint[i]
                );
	        /////////////////////end: 画vol成交量////////////////////
		}
	}
	return true;
}

kline.DrawSign = function () {
    if (false == kline.DrawSignNormal()){
        console.log("Call DrawSignNormal error.");
        return false;
    }

	return true;
}

kline.DrawSignNormal = function () {
    var fontSize = 15;
    kline.m_cvtext.beginPath();
    kline.m_cvtext.fillStyle = "#ff0000";
    kline.m_cvtext.font = fontSize + "px arial";
    kline.m_cvtext.lineWidth = 0.2;
    kline.m_cvtext.strokeStyle = "#ff0000";

    // 画一些标志性的Y轴业务数值，只在左侧标注一组
    var KM = kline.m_iShowKMax - kline.m_iShowKMin;
	for (var t = 0; t < 9; t++)
	{
	    kline.m_cvtext.fillText(
            Math.round(kline.m_iShowKMax - (KM / 9.0)*t) / 100 + '', 
            kline.m_iPointLeft + 1, 
            kline.m_iPointTop + 1 + fontSize + t * (kline.m_iPointHeight / 9.0)
            );
	}
	kline.m_cvtext.fillText(
        Math.round(kline.m_iShowKMax - (KM / 9.0) * 9) / 100 + '', 
        kline.m_iPointLeft + 1, 
        kline.m_iPointTop + 1 + kline.m_iPointHeight
        );

    // 画日期数据
    /*
    kline
    字段意义	值类型	备注
    date	int	日期
    open	int	开盘
    close	int	收盘
    high	int	最高
    low	int	最低
    amount	int	成交额
    vol	int	成交量
    daycount	int	几天
    */
	if (kline.m_idxEnd - kline.m_idxBegin < 10)
	{
	    if ((kline.m_idxBegin >= 0) && (kline.m_idxBegin < kline.m_vecData.length))
	    {
	        kline.m_cvtext.fillText(
                kline.m_vecData[m_idxBegin][0] + '', 
                kline.m_iPointLeft + kline.m_iPointWidth, 
                kline.m_iVolTop + kline.m_iVolHeight + 16
                );
		}
	}
	else
	{
		var idx = 0;
		for (var t = 0; t < 10; t++)
		{
		    idx = kline.m_idxBegin + Math.round((kline.m_idxEnd - kline.m_idxBegin)*t / 10);
		    if ((idx >= 0) && (idx < kline.m_vecData.length))
		    {
		        kline.m_cvtext.fillText(
                    kline.m_vecData[idx][0] + '', 
                    kline.m_iPointLeft + kline.m_iPointWidth - kline.m_iPointWidth*t / 10, 
                    kline.m_iVolTop + kline.m_iVolHeight + 16
                    );
			}
		}
	}
	if ((kline.m_idxEnd >= 1) && (kline.m_idxEnd <= kline.m_vecData.length))
	{
	    kline.m_cvtext.fillText(
            kline.m_vecData[kline.m_idxEnd - 1][0] + '', 
            kline.m_iPointLeft + 1, 
            kline.m_iVolTop + kline.m_iVolHeight + 16
            );
	}

	return true;
}

kline.DrawInfo = function () {
    kline.m_vecInfoClick = [];
	return kline.DrawInfoNormal();
}

kline.DrawInfoNormal = function () {
    kline.m_cvtext.beginPath();
    kline.m_cvtext.fillStyle = "#ff0000";
    kline.m_cvtext.font = "15px arial";
    kline.m_cvtext.clearRect(kline.m_iInfoLeft, kline.m_iInfoTop, kline.m_iInfoWidth, kline.m_iInfoHeight);

    var i = 1;
    kline.m_cvtext.fillText(kline.m_strCode, kline.m_iInfoLeft + 20, kline.m_iInfoTop + i++*20);
    if ((0 > kline.m_iCurrDataIdx) || (kline.m_vecData.length == 0) || (kline.m_iCurrDataIdx >= kline.m_vecData.length))
    {
        return true;
    }
    /*
    kline
    字段意义	值类型	备注
    date	int	日期
    open	int	开盘
    close	int	收盘
    high	int	最高
    low	int	最低
    amount	int	成交额
    vol	int	成交量
    daycount	int	几天
    */
    kline.m_cvtext.fillText("Date: " + kline.m_vecData[kline.m_iCurrDataIdx][0], kline.m_iInfoLeft + 10, kline.m_iInfoTop + i++*20);  // date
    kline.m_cvtext.fillText("Open: " + kline.m_vecData[kline.m_iCurrDataIdx][1] / 100.0, kline.m_iInfoLeft + 10, kline.m_iInfoTop + i++*20);  // open
    kline.m_cvtext.fillText("Close: " + kline.m_vecData[kline.m_iCurrDataIdx][2] / 100.0, kline.m_iInfoLeft + 10, kline.m_iInfoTop + i++*20);  // close
    kline.m_cvtext.fillText("High: " + kline.m_vecData[kline.m_iCurrDataIdx][3] / 100.0, kline.m_iInfoLeft + 10, kline.m_iInfoTop + i++*20);  // high
    kline.m_cvtext.fillText("Low: " + kline.m_vecData[kline.m_iCurrDataIdx][4] / 100.0, kline.m_iInfoLeft + 10, kline.m_iInfoTop + i++*20);  // low
    kline.m_cvtext.fillText("Amount: " + kline.m_vecData[kline.m_iCurrDataIdx][5], kline.m_iInfoLeft + 10, kline.m_iInfoTop + i++*20);  // amount
    kline.m_cvtext.fillText("vol: " + kline.m_vecData[kline.m_iCurrDataIdx][6], kline.m_iInfoLeft + 10, kline.m_iInfoTop + i++*20);  // vol
    if ((kline.m_iCurrDataIdx + 1 < kline.m_vecData.length) && (kline.m_vecData[kline.m_iCurrDataIdx + 1][2] != 0))
    {
        kline.m_cvtext.fillText(
            "Chg: " + Math.round(10000 *
            (kline.m_vecData[kline.m_iCurrDataIdx][2] - kline.m_vecData[kline.m_iCurrDataIdx + 1][2])
            / kline.m_vecData[kline.m_iCurrDataIdx + 1][2]) / 100 + "%",
            kline.m_iInfoLeft + 10,
            kline.m_iInfoTop + i++ * 20);
    }
    else
    {
        kline.m_cvtext.fillText("Chg: 0", kline.m_iInfoLeft + 10, kline.m_iInfoTop + i++ * 20);

    }

    /*if (NULL != m_pAvgLine)
    {
        int j = 0;
        map<int, vector<int> >::iterator iter;
        for (iter = m_pAvgLine->m_mapData.begin(); iter != m_pAvgLine->m_mapData.end(); iter++)
        {
            if (m_iCurrDataIdx >= iter->second.size())
            {
                continue;
            }
            j++;
            sprintf(temp, "avg: %d, value %.2f", iter->first, (iter->second)[m_iCurrDataIdx] / 100.0);
            dc.TextOut(m_iInfoLeft + 10, m_iInfoTop + 2 + 7 * 20 + j * 20, temp, strlen(temp));
        }
    }*/

    return true;
}

kline.mousedown = function (ev) {
    //console.log("kline.mousedown");
    kline.m_bCross = kline.m_bCross ? false : true;

    //kline.m_objTrain.m_objClickRect
    //console.log("ev.clientX: " + ev.clientX);
    //console.log("ev.clientY: " + ev.clientY);
    //console.log("m_iPointLeft: " + kline.m_objTrain.m_objClickRect.m_iLeft);
    //console.log("m_iPointWidth: " + kline.m_objTrain.m_objClickRect.m_iWidth);
    if ((null != kline.m_objTrain) && (null != kline.m_objTrain.m_objClickRect)) {
        if (ev.clientX >= kline.m_objTrain.m_objClickRect.m_iLeft
            && ev.clientX <= kline.m_objTrain.m_objClickRect.m_iLeft + kline.m_objTrain.m_objClickRect.m_iWidth
            && ev.clientY >= kline.m_objTrain.m_objClickRect.m_iTop
            && ev.clientY <= kline.m_objTrain.m_objClickRect.m_iTop + kline.m_objTrain.m_objClickRect.m_iHeight) {
            kline.TrainTrade();
            //console.log("kline.TrainTrade occur.");
            return true;
        }
    }
    return true;
}

kline.mousemove = function (ev) {
    //console.log("kline.mousemove");

    if (false == kline.m_bCross) {
        return true;
    }

    if (ev.clientX <= kline.m_iPointLeft || ev.clientX >= kline.m_iPointWidth || ev.clientY <= kline.m_iPointTop
        || ev.clientY >= kline.m_iPointHeight + kline.m_iVolHeight) {  // 右侧和底部忽略了尺寸比较小的left和top
        kline.show();
        kline.m_bCross = false;
        return true;
    }

    if (false == kline.SetCurrPoint(ev)) {    // 设置当前位置对应到曲线坐标
        console.log("Error: Call single.SetCurrPoint error.");
        return false;
    }

    if (false == kline.show()) {  // 刷新
        console.log("Error: Call single.show error.");
        return false;
    }

    // 画十字星
    kline.m_cvtext.lineWidth = 1;
    kline.m_cvtext.beginPath();
    kline.m_cvtext.strokeStyle = "#FFFFFF";
    kline.m_cvtext.moveTo(ev.clientX, kline.m_iPointTop);
    kline.m_cvtext.lineTo(ev.clientX, kline.m_iPointHeight + kline.m_iVolHeight);
    kline.m_cvtext.moveTo(kline.m_iPointLeft, ev.clientY);
    kline.m_cvtext.lineTo(kline.m_iPointWidth, ev.clientY);
    //single.m_cvtext.setLineDash([5, 20]);
    //ogc.lineTo(ev.clientX-oc.offsetLeft,ev.clientY-oc.offsetTop);
    kline.m_cvtext.stroke();

    return true;
}

kline.SetCurrPoint = function(ev){
	var dw          = 0.0;
	var dRightX     = 0.0;

	kline.m_iMouseX = ev.clientX;
	kline.m_iMouseY = ev.clientY;

	dw = kline.m_iPointWidth * 1.0 / (kline.m_idxEnd - kline.m_idxBegin);
	dRightX = kline.m_iPointWidth - kline.m_iMouseX + kline.m_iPointLeft; // x轴距右边界的距离
	kline.m_iCurrDataIdx = Math.floor(dRightX / dw) + kline.m_idxBegin;
	if (0 > kline.m_iCurrDataIdx)
	{
	    kline.m_iCurrDataIdx = 0;
	}

	if (kline.m_iCurrDataIdx >= kline.m_vecData.length)
	{
	    kline.m_iCurrDataIdx = 0;
	}

	return true;
}

kline.Zoom = function (iScale) {
	var iTempBegin = 0;
	var iTempEnd = 0;
	var bDataExtraAlter = false;  // GetLineData函数之外引起m_vecData变化

	switch (iScale)
	{
	case ZoomType.add:// 精度扩大
	    iTempBegin = kline.m_idxBegin + Math.floor((kline.m_iCurrDataIdx - kline.m_idxBegin) / 2);
	    iTempEnd = kline.m_idxEnd - Math.floor((kline.m_idxEnd - kline.m_iCurrDataIdx) / 2);
		if (0 > iTempBegin)
		{
			iTempBegin = 0;
		}
		if ((0 < kline.m_vecData.length) && (kline.m_vecData.length <= iTempEnd))
		{
		    iTempEnd = kline.m_vecData.length - 1;
		}
		if (iTempEnd < iTempBegin)
		{
		    iTempBegin = kline.m_idxBegin;
		    iTempEnd = kline.m_idxEnd;
		}
		kline.m_idxBegin = iTempBegin;
		kline.m_idxEnd = iTempEnd;
		break;
	case ZoomType.sub:// 精度缩小
	    if ((kline.m_iCurrDataIdx < 0) || (0 == kline.m_vecData.length) || (kline.m_iCurrDataIdx >= kline.m_vecData.length))
		{
			break;
		}
	    if ((0 == kline.m_global.date) || (kline.m_vecData[kline.m_idxBegin][0] < kline.m_global.date))
		{
	        iTempBegin = kline.m_idxBegin - (kline.m_iCurrDataIdx - kline.m_idxBegin);
		}
		else
		{
	        iTempBegin = kline.m_idxBegin;
		}
		
	    iTempEnd = kline.m_idxEnd + (kline.m_idxEnd - kline.m_iCurrDataIdx);
	    if ((0 > iTempBegin) || (kline.m_vecData.length <= iTempEnd))
		{
	        if (3 == kline.m_iMaxInterval) // 所有k线数据已经下载
			{
			    kline.m_idxBegin = (iTempBegin < 0) ? 0 : iTempBegin;
			    kline.m_idxEnd = (kline.m_vecData.length <= iTempEnd) ? (kline.m_vecData.length - 1) : iTempEnd;
				return true;
			}

	        var iBefore = iTempEnd - kline.m_iCurrDataIdx;
	        var iAfter = kline.m_iCurrDataIdx - iTempBegin;
	        var iDate = kline.m_vecData[kline.m_iCurrDataIdx][0];
	        var lOldSize = kline.m_vecData.length;
	        if (false == kline.GetLineData(kline.m_strCycle, iDate, iBefore, iAfter))
			{
	            console.error("ERROR: Call GetLineData error. iDate: " + iDate<<", iBefore: " + iBefore + ", iAfter: " + iAfter);
				break;
	        }
	        if (lOldSize == kline.m_vecData.length) {
	            kline.m_iMaxInterval = 3;
	        }
			if (kline.m_global.date != 0)
			{
			    for (; kline.m_idxBegin < kline.m_vecData.length; kline.m_idxBegin++)
				{
			        if (kline.m_vecData[kline.m_idxBegin][0] <= kline.m_global.date)
					{
						break;
					}
				}
			}
			kline.m_iCurrDataIdx = Math.floor((kline.m_idxEnd + kline.m_idxBegin) / 2);
		}
		else
		{
			if (iTempEnd < iTempBegin)
			{
			    iTempBegin = kline.m_idxBegin;
			    iTempEnd = kline.m_idxEnd;
			}
			kline.m_idxBegin = iTempBegin;
			kline.m_idxEnd = iTempEnd;
		}
		break;
	case ZoomType.left:// 焦点左移
	    kline.m_idxEnd++;
	    kline.m_idxBegin++;
	    kline.m_iCurrDataIdx = kline.m_idxEnd;
	    if (kline.m_idxEnd >= kline.m_vecData.length)
		{
	        kline.m_idxBegin--;
	        kline.m_idxEnd--;
	        kline.m_iCurrDataIdx = kline.m_idxEnd;
	        if ((3 == kline.m_iMaxInterval) || (1 == kline.m_iMaxInterval)) { // 左侧数据已经加载完
	            return true;
	        }
	        var iInterval = kline.m_idxEnd - kline.m_idxBegin;
	        var iOldSize = kline.m_vecData.length;
	        if (false == kline.GetLineData(kline.m_strCycle, kline.m_vecData[kline.m_idxEnd-1][0], 100, iOldSize-1)) // 不够了，一次取100个备用
			{
	            console.error("Call GetLineData error. date: " + kline.m_vecData[kline.m_idxEnd - 1][0]);
				return false;
	        }
	        if (iOldSize == kline.m_vecData.length) { // 左侧数据已经加载完
	            kline.m_iMaxInterval = 1;
	            return true;
	        }
			bDataExtraAlter = true;
			kline.m_idxEnd = iOldSize + 1;
			kline.m_idxBegin = kline.m_idxEnd - iInterval;
			kline.m_iCurrDataIdx = kline.m_idxEnd;

            console.debug("m_vecData.size: " + kline.m_vecData.length + ", oldsize: " + iOldSize
                + ", kline.m_idxEnd: " + kline.m_idxEnd + "idxbegin: " + kline.m_idxBegin);
        }
        else{
            console.debug("!kline.m_idxEnd >= kline.m_vecData.length. m_vecData.size: " + kline.m_vecData.length 
                + ", kline.m_idxEnd: " + kline.m_idxEnd + "idxbegin: " + kline.m_idxBegin);
        }
		break;
	case ZoomType.right:// 焦点右移
	    console.log("idxbegin: " + kline.m_idxBegin + ", idxEnd: " + kline.m_idxEnd);
	    kline.m_idxBegin--;
	    kline.m_idxEnd--;
	    console.log("idxbegin: " + kline.m_idxBegin + ", idxEnd: " + kline.m_idxEnd);
	    kline.m_iCurrDataIdx = kline.m_idxBegin;
	    if (kline.m_idxBegin < 0)
	    {
	        kline.m_idxBegin++;
	        kline.m_idxEnd++;
	        kline.m_iCurrDataIdx = kline.m_idxBegin;
	        if ((3 == kline.m_iMaxInterval) || (2 == kline.m_iMaxInterval)) { // 右侧数据已经加载完
	            return true;
	        }
	        var iInterval = kline.m_idxEnd - kline.m_idxBegin;
	        var iOldSize = kline.m_vecData.length;
	        if (false == kline.GetLineData(kline.m_strCycle, kline.m_vecData[0][0], iOldSize-1, 100)) // 不够了，一次取100个备用
			{
	            console.error("Call GetLineData error. date: " << kline.m_vecData[m_idxEnd - 1][0]);
				return false;
	        }
	        console.log("iOldSize: " + iOldSize + ", kline.m_vecData.length: " + kline.m_vecData.length);
	        if (iOldSize == kline.m_vecData.length) { // 右侧数据已经加载完
	            kline.m_iMaxInterval = 2;
	            return true;
	        }
			bDataExtraAlter = true;
			kline.m_idxBegin = kline.m_vecData.length - iOldSize;
			kline.m_idxEnd = kline.m_idxBegin + iInterval;
			kline.m_iCurrDataIdx = kline.m_idxBegin;

			console.debug("m_vecData.size: " + kline.m_vecData.length + ", oldsize: " + iOldSize);
	    }
	    //console.log("kline.m_global.date: " + kline.m_global.date);
	    if (0 != kline.m_global.date)
		{
	        while (kline.m_vecData[kline.m_idxBegin][0] > kline.m_global.date)
			{
	            kline.m_idxBegin++;
	            kline.m_idxEnd++;
			}
		}
		break;
	default:
		break;
	}

	if (false == kline.GetShowMValue()) {  // 得到显示区段最值
		console.error("Call GetShowMValue.");
		return false;
	}

	/*if (true == bDataExtraAlter) // 指标数据重新做更新
	{
		iRet = m_pAvgLine->GetData(strRetCode);  // 均线数据
		if (0 != iRet)
		{
			ERRORLOG("Call m_pAvgLine->GetData error. strRetCode: "<<strRetCode);
			return iRet;
		}
	}*/

	if (false == kline.show()) {  // 刷新
	    console.error("Error: Call kline.show error.");
	    return false;
	}

	return true;
}

kline.NextK = function(){
	var bDataExtraAlter = false;

	kline.m_idxBegin--;
	kline.m_idxEnd--;
	kline.m_iCurrDataIdx = kline.m_idxBegin;
	if (kline.m_idxBegin < 0)
	{
	    kline.m_idxBegin++;
	    kline.m_idxEnd++;
	    kline.m_iCurrDataIdx = kline.m_idxBegin;
	    if ((3 == kline.m_iMaxInterval) || (2 == kline.m_iMaxInterval)) { // 右侧数据已经加载完
	        return true;
	    }
	    var iInterval = kline.m_idxEnd - kline.m_idxBegin;
	    var iOldSize = kline.m_vecData.length;
		//iOldSize = (iOldSize == 0) ? iOldSize : iOldSize - 1;
	    if (false == kline.GetLineData(kline.m_strCycle, kline.m_vecData[0][0], iOldSize - 1, 100)) // 不够了，一次取100个备用
		{
	        console.error("Call GetLineData error. date: " + kline.m_vecData[kline.m_idxEnd - 1][0]);
			return false;
	    }
	    if (iOldSize == kline.m_vecData.length) { // 右侧数据已经加载完
	        kline.m_iMaxInterval = 2;
	        return true;
	    }
		bDataExtraAlter = true;
		kline.m_idxBegin = kline.m_vecData.length - iOldSize;
		kline.m_idxEnd = kline.m_idxBegin + iInterval;
		kline.m_iCurrDataIdx = kline.m_idxBegin;

		if (kline.m_idxBegin > 0)
		{
		    kline.m_idxBegin--;
		    kline.m_idxEnd--;
		    kline.m_iCurrDataIdx = kline.m_idxBegin;
		}

		console.debug("m_vecData.size: " + kline.m_vecData.length + ", oldsize: " + iOldSize);
	}

	if ((0 != kline.m_global.date) && (kline.m_global.date < kline.m_vecData[kline.m_idxBegin][0]))
	{
	    kline.m_global.date = kline.m_vecData[kline.m_idxBegin][0];
	    conf.set('global', kline.m_global);
	}

	if (false == kline.GetShowMValue()){  // 得到显示区段最值
		console.error("Call GetShowMValue.");
		return false;
	}

	if (false == kline.show()) {  // 刷新
	    console.error("Error: Call kline.show error.");
	    return false;
	}

	return true;
}

/*
kline
字段意义	值类型	备注
date	int	日期
open	int	开盘
close	int	收盘
high	int	最高
low	int	最低
amount	int	成交额
vol	int	成交量
daycount	int	几天
*/
kline.GetShowMValue = function () {
    if ((kline.m_vecData.length == 0) || (kline.m_idxBegin < 0) || (kline.m_idxBegin >= kline.m_vecData.length))
	{
		return true;
	}
    kline.m_iShowKMax = kline.m_vecData[kline.m_idxBegin][3];
    kline.m_iShowKMin = kline.m_vecData[kline.m_idxBegin][4];
    kline.m_iShowVolMax = kline.m_iShowVolMin = kline.m_vecData[kline.m_idxBegin][6];
    for (var i = kline.m_idxBegin; (i < kline.m_idxEnd) && (i < kline.m_vecData.length) ; i++)
	{
        kline.m_iShowKMax = (kline.m_iShowKMax > kline.m_vecData[i][3]) ? kline.m_iShowKMax : kline.m_vecData[i][3];
        kline.m_iShowKMin = (kline.m_iShowKMin < kline.m_vecData[i][4]) ? kline.m_iShowKMin : kline.m_vecData[i][4];
        kline.m_iShowVolMax = (kline.m_iShowVolMax > kline.m_vecData[i][6]) ? kline.m_iShowVolMax : kline.m_vecData[i][6];
        kline.m_iShowVolMin = (kline.m_iShowVolMin < kline.m_vecData[i][6]) ? kline.m_iShowVolMin : kline.m_vecData[i][6];
	}

	return true;
}

kline.rightMenu = function (e) {
    console.log("not support right menu");
    return false;
}