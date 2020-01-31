var tradestatistics = tradestatistics || {};

tradestatistics.m_canvas = document.getElementById("tradecanvas");
tradestatistics.m_cvtext = tradestatistics.m_canvas.getContext("2d");

tradestatistics.init = function () {
    // 配置
    //tradestatistics.m_conf = conf.get('single');
    tradestatistics.m_global = conf.get('global');

	// view 画点数区域
	tradestatistics.m_iPointTop      = 0;
	tradestatistics.m_iPointLeft     = 0;
	tradestatistics.m_iPointHeight   = 0;
	tradestatistics.m_iPointWidth    = 0;

	// view 显示信息区域
	tradestatistics.m_iInfoTop       = 0;
	tradestatistics.m_iInfoLeft      = 0;
	tradestatistics.m_iInfoHeight    = 0;
	tradestatistics.m_iInfoWidth     = 0;

    // 位置信息
	tradestatistics.m_iMouseX        = 0;
	tradestatistics.m_iMouseY        = 0;
	tradestatistics.m_iCurrDataIdx   = 0;

    tradestatistics.m_bCross = false;  // 是否画十字星

    // 基本配置信息
	tradestatistics.m_strCycle       = "day";  // 周期，日周月年

    // 远程获取的数据
    tradestatistics.m_mapData        = {}; // map的key是trade，vector按照date有序
    tradestatistics.m_vecDate        = []; 
    tradestatistics.m_nCodeCnt       = 0;
    tradestatistics.m_nPointCnt      = 0;
	//map<int, vector<CPointStatItem> > m_mapData; // map的key是trade，vector按照date有序
    //vector<int> m_vecDate;

	// 曲线点数数据
	tradestatistics.m_idxBegin = 0; // 显示点数下标范围[m_idxBegin, m_idxEnd)
	tradestatistics.m_idxEnd   = 0;

	tradestatistics.m_iShowMax = 0;
	tradestatistics.m_iShowMin = 0;
    tradestatistics.m_mapShowMValue  = {};
    tradestatistics.m_vecXPoint      = [];  // X轴坐标，全局统一
    tradestatistics.m_mapYPoint      = {};  // Y轴坐标
    //vector<int> m_vecXPoint;       // X轴坐标，全局统一
	//map<int, vector<int> > m_mapYPoint;   // Y轴坐标
	
	if (false == tradestatistics.req()) {
	    console.log("call tradestatistics.req() error");
		return false;
	}
	
	if (false == tradestatistics.show()) {
	    console.log("call tradestatistics.show() error.");
		return false;
	}
	
	return true;
}

tradestatistics.req = function() {
    //////begin: 初始化数据////////////////////
    // 远程获取的数据
    tradestatistics.m_mapData        = {}; // map的key是trade，vector按照date有序
    tradestatistics.m_vecDate        = []; 
    tradestatistics.m_nCodeCnt       = 0;
    tradestatistics.m_nPointCnt      = 0;
    //map<int, vector<CPointStatItem> > m_mapData; // map的key是trade，vector按照date有序
    //vector<int> m_vecDate;

    // 曲线点数数据
    tradestatistics.m_idxBegin = 0; // 显示点数下标范围[m_idxBegin, m_idxEnd)
    tradestatistics.m_idxEnd   = 0;

    tradestatistics.m_iShowMax = 0;
    tradestatistics.m_iShowMin = 0;
    tradestatistics.m_mapShowMValue  = {};
    tradestatistics.m_vecXPoint      = [];  // X轴坐标，全局统一
    tradestatistics.m_mapYPoint      = {};  // Y轴坐标
    //////end: 初始化数据////////////////////

    /*
    {"method":"point.statistics","session":"1000001549413417",
     "code":["000002","000004","000006"],"cycle":"day"}

    {"retcode":"0000","result":{
        "point":[{"trade":101,"count":2,"date":20171220,"code":["000002","000002"]}],
        "date":[20171220],"begindate":20171220,"enddate":20171220}}
    */
    gdata.get();
    user.get();
    if (0 == user.m_data.sessionid || "" == user.m_data.sessionid){
        alert("请登录系统，再操作");
        return true;
    }
    var req = 
        {
            "method":"point.statistics",
            "session":user.m_data.sessionid,
            "code":gdata.m_data.codes,
            "cycle": tradestatistics.m_strCycle
        };
    console.log('tradestatistics.req: ' + JSON.stringify(req));
    function remoteTradeStatistics(objJson) {
        if (objJson.retcode != '0000') {
            console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
            return false;
        }
        console.log(JSON.stringify(objJson));
        if (undefined != objJson.result.date){
            tradestatistics.m_vecDate = objJson.result.date;
        }
        if (undefined != objJson.result.codecnt){
            tradestatistics.m_nCodeCnt = objJson.result.codecnt;
        }
        if (undefined != objJson.result.pointcnt){
            tradestatistics.m_nPointCnt = objJson.result.pointcnt;
        }
        if (undefined != objJson.result.point){
            //tradestatistics.m_mapData = objJson.result.point;
            for (var i=0; i<objJson.result.point.length; i++){
                if (!tradestatistics.m_mapData.hasOwnProperty(objJson.result.point[i].trade)){
                    tradestatistics.m_mapData[objJson.result.point[i].trade] = [];
                    tradestatistics.m_mapShowMValue[objJson.result.point[i].trade] 
                        = {"max":objJson.result.point[i].count, "min":objJson.result.point[i].count};
                }
                tradestatistics.m_mapData[objJson.result.point[i].trade].push(objJson.result.point[i]);
                if (tradestatistics.m_mapShowMValue[objJson.result.point[i].trade].max 
                    < objJson.result.point[i].count){
                    tradestatistics.m_mapShowMValue[objJson.result.point[i].trade].max = objJson.result.point[i].count;
                }
                if (tradestatistics.m_mapShowMValue[objJson.result.point[i].trade].min 
                    > objJson.result.point[i].count){
                    tradestatistics.m_mapShowMValue[objJson.result.point[i].trade].min = objJson.result.point[i].count;
                }
            }
        }
        
        return true;
    }
    gdata.send(req, remoteTradeStatistics);

    // 显示坐标区域
	tradestatistics.m_idxBegin = 0;
	tradestatistics.m_idxEnd = tradestatistics.m_vecDate.length;

    // 显示区间最值
    tradestatistics.m_iShowMax = tradestatistics.m_iShowMin = 0;
    for (var item in tradestatistics.m_mapShowMValue){
        tradestatistics.m_iShowMax = tradestatistics.m_iShowMax > tradestatistics.m_mapShowMValue[item].max 
            ? tradestatistics.m_iShowMax : tradestatistics.m_mapShowMValue[item].max;
        tradestatistics.m_iShowMin = tradestatistics.m_iShowMin < tradestatistics.m_mapShowMValue[item].min 
            ? tradestatistics.m_iShowMin : tradestatistics.m_mapShowMValue[item].min;
    }

    return true;
}

tradestatistics.show = function() {
    //console.log("tradestatistics.show");

    tradestatistics.m_cvtext.clearRect(0, 0, tradestatistics.m_canvas.width, tradestatistics.m_canvas.height);

    tradestatistics.m_cvtext.beginPath();
    tradestatistics.m_cvtext.fillStyle = "#ffffff";
    tradestatistics.m_cvtext.font = "20px SimSun";

    // 左侧曲线区域
    tradestatistics.m_iPointTop = 2;
    tradestatistics.m_iPointLeft = 2;
    tradestatistics.m_iPointHeight = tradestatistics.m_canvas.height - 35;
    tradestatistics.m_iPointWidth = tradestatistics.m_canvas.width - 200;

    // 右侧信息区域
    tradestatistics.m_iInfoTop = 2;
    tradestatistics.m_iInfoLeft = tradestatistics.m_iPointLeft + tradestatistics.m_iPointWidth;
    tradestatistics.m_iInfoHeight = tradestatistics.m_iPointHeight;
    tradestatistics.m_iInfoWidth = tradestatistics.m_canvas.width - 2 - tradestatistics.m_iInfoLeft;
	
    //画左面的边框
    //-------------->
    //              |
    //<------------\|/
    tradestatistics.m_cvtext.lineWidth = 1.5;
    tradestatistics.m_cvtext.beginPath();
    tradestatistics.m_cvtext.strokeStyle = "#ff0000";
    tradestatistics.m_cvtext.moveTo(tradestatistics.m_iPointLeft, tradestatistics.m_iPointTop);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iPointLeft + tradestatistics.m_iPointWidth, tradestatistics.m_iPointTop);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iPointLeft + tradestatistics.m_iPointWidth, tradestatistics.m_iPointTop + tradestatistics.m_iPointHeight);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iPointLeft, tradestatistics.m_iPointTop + tradestatistics.m_iPointHeight);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iPointLeft, tradestatistics.m_iPointTop);
    tradestatistics.m_cvtext.stroke();

    // 画右面的边框
    tradestatistics.m_cvtext.lineWidth = 1.5;
    tradestatistics.m_cvtext.beginPath();
    tradestatistics.m_cvtext.strokeStyle = "#ff0000";
    tradestatistics.m_cvtext.moveTo(tradestatistics.m_iInfoLeft, tradestatistics.m_iInfoTop);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iInfoLeft + tradestatistics.m_iInfoWidth, tradestatistics.m_iInfoTop);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iInfoLeft + tradestatistics.m_iInfoWidth, tradestatistics.m_iInfoTop + tradestatistics.m_iInfoHeight);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iInfoLeft, tradestatistics.m_iInfoTop + tradestatistics.m_iInfoHeight);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iInfoLeft, tradestatistics.m_iInfoTop);
    tradestatistics.m_cvtext.stroke();

    // 画八条虚线
    //ctx.lineWidth = 1;
    for (var y = tradestatistics.m_iPointHeight + tradestatistics.m_iPointTop - tradestatistics.m_iPointHeight / 9;
        y > tradestatistics.m_iPointTop + 10;y -= tradestatistics.m_iPointHeight / 9){
        tradestatistics.m_cvtext.lineWidth = 0.5;
        tradestatistics.m_cvtext.beginPath();
        tradestatistics.m_cvtext.strokeStyle = "#ff0000";
        tradestatistics.m_cvtext.moveTo(tradestatistics.m_iPointLeft, y);
        tradestatistics.m_cvtext.lineTo(tradestatistics.m_iPointLeft + tradestatistics.m_iPointWidth, y);
        tradestatistics.m_cvtext.stroke();
    }

    // 画左侧曲线
    if (false == tradestatistics.DrawPoint()) {
        console.log("Call tradestatistics.DrawPoint error");
        return false;
    }

    // 画标记信息
    if (false == tradestatistics.DrawSign()) {
        console.log("Call tradestatistics.DrawSign error");
        return false;
    }

    // 画右侧显示信息
    if (false == tradestatistics.DrawInfo()) {
        console.log("Call tradestatistics.DrawInfo error");
        return false;
    }

	return true;
}

// 画左侧曲线
tradestatistics.DrawPoint = function () {

    if (false == tradestatistics.GetPoint()){
        console.log("Call tradestatistics.GetPoint error");
        return false;
    }
    //console.log("line rect: " + tradestatistics.m_iPointTop + ", " + tradestatistics.m_iPointHeight 
    //    + ", " + tradestatistics.m_iPointLeft + ", " + tradestatistics.m_iPointWidth);

    //console.log('DrawPoint conf: ');
    //console.log(tradestatistics.m_conf.show);
    for (var iter in tradestatistics.m_mapYPoint) {  // 买卖点统计曲线
        // 选择画笔颜色
        tradestatistics.m_cvtext.lineWidth = 1;
        tradestatistics.m_cvtext.beginPath();
        tradestatistics.m_cvtext.strokeStyle = "#ff0000";
        //if (tradestatistics.m_conf.show.hasOwnProperty(iter)) {
        //    tradestatistics.m_cvtext.strokeStyle = tradestatistics.m_conf.show[iter].color;
            //console.log('draw color. type: ' + iter + ', color: ' + tradestatistics.m_cvtext.strokeStyle);
        //}
        //console.log('draw color. type: ' + iter + ', color: ' + tradestatistics.m_cvtext.strokeStyle + ', conf color: ' + tradestatistics.m_conf.show[iter].color);

        var ptrVecY = tradestatistics.m_mapYPoint[iter];
        //console.log(tradestatistics.m_mapYPoint[iter]);
        //DEBUGLOG("(*ptrVecY).size(): "<< (*ptrVecY).size()<<"m_bpReport.m_vecX.size(): "<< m_bpReport.m_vecX.size());
        for (var j = 1; j < ptrVecY.length; j++){
            if ((0 == ptrVecY[j - 1]) || (0 == ptrVecY[j])){
                continue;
            }
            tradestatistics.m_cvtext.moveTo(tradestatistics.m_vecXPoint[j], ptrVecY[j]);
            tradestatistics.m_cvtext.lineTo(tradestatistics.m_vecXPoint[j - 1], ptrVecY[j - 1]);
            //console.log("tradestatistics.m_vecXPoint[j]: " + tradestatistics.m_vecXPoint[j]);
            //console.log("ptrVecY[j]: " + ptrVecY[j]);
            //DEBUGLOG("m_bpReport.m_vecX[j]: "<< m_bpReport.m_vecX[j]<<", (*ptrVecY)[j]: "<< (*ptrVecY)[j]);
        }
        tradestatistics.m_cvtext.stroke();
    }

    return true;
}

// 得到曲线点数坐标
tradestatistics.GetPoint = function () {
    var dw  = 0.0;
    var dh  = 0.0;
    var iHeight = 0;
    var iDate = 0;
    var vecYPoint = [];

    tradestatistics.m_vecXPoint      = [];  // X轴坐标，全局统一
    tradestatistics.m_mapYPoint      = {};  // Y轴坐标

    if (0 >= tradestatistics.m_iShowMax)
	{
		console.log("m_iShowMax error. tradestatistics.m_iShowMax: " + tradestatistics.m_iShowMax);
		return false;
	}
	dh = 1.0*tradestatistics.m_iPointHeight / tradestatistics.m_iShowMax;
    dw = (tradestatistics.m_iPointWidth*1.0) / (1 + tradestatistics.m_idxEnd - tradestatistics.m_idxBegin); // 用浮点数减小误差

    for (var i = tradestatistics.m_idxBegin; i < tradestatistics.m_idxEnd; i++) {    // 财报X轴坐标
        tradestatistics.m_vecXPoint.push(tradestatistics.m_iPointLeft + tradestatistics.m_iPointWidth 
            - dw - (i - tradestatistics.m_idxBegin) * dw); // 从右向左画，右边空出1个的空间
        vecYPoint.push(tradestatistics.m_iPointTop + tradestatistics.m_iPointHeight);
    }

    for (var iter in tradestatistics.m_mapData) {
        tradestatistics.m_mapYPoint[iter] = vecYPoint;
    }

    for (var iter in tradestatistics.m_mapData)
	{
		// 计算Y轴坐标
		var iQueryIdx = 0;
		var iQueryBegin = tradestatistics.m_idxBegin;
		var iQueryEnd = tradestatistics.m_idxEnd - 1;
		for (var j = 0; j < tradestatistics.m_mapData[iter].length; j++)
		{
			iHeight = tradestatistics.m_iPointTop + tradestatistics.m_iPointHeight - dh * tradestatistics.m_mapData[iter][j].count;
            //console.log("iHeight: " + iHeight + ", dh: " + dh + ", m_nCount: " + tradestatistics.m_mapData[iter][j].count
            //    + ", date: " + tradestatistics.m_mapData[iter][j].date + ", trade: " + tradestatistics.m_mapData[iter][j].trade);
			iDate = tradestatistics.m_mapData[iter][j].date;
			/*if (iDate > m_vecDate[iQueryBegin])
			{
				continue;
			}
			if (iDate < m_vecDate[iQueryEnd])
			{
				continue;
			}*/
			//console.log("iQueryBegin: " + iQueryBegin + ", iQueryEnd: " + iQueryEnd + ", iDate: " + iDate
            //    + ", beginDate: " + tradestatistics.m_vecDate[iQueryBegin] + ", endDate: " 
            //    + tradestatistics.m_vecDate[iQueryEnd]);
			iQueryIdx = tradestatistics.SearchVec(tradestatistics.m_vecDate, iDate, iQueryBegin, iQueryEnd);
			if (0 > iQueryIdx)
			{
                console.log("iQueryIdx: " + iQueryIdx);
				continue;
			}
			tradestatistics.m_mapYPoint[iter][iQueryIdx - tradestatistics.m_idxBegin] = iHeight;
            //console.log("m_mapYPoint key: " + iter + ", index: " + (iQueryIdx - tradestatistics.m_idxBegin)
            //    + ", iHeight: " + iHeight + ", tradestatistics.m_mapYPoint[iter][iQueryIdx - tradestatistics.m_idxBegin]: "
            //    + tradestatistics.m_mapYPoint[iter][iQueryIdx - tradestatistics.m_idxBegin]);
		}
    }

    return true;
}

tradestatistics.SearchVec = function (vecNum, iValue, idxBegin, idxEnd) {
    var iMid = 0;
	while (idxBegin <= idxEnd)
	{
		iMid = Math.floor((idxBegin + idxEnd)/2);
		if (iValue == vecNum[iMid])
		{
			return iMid;
		}
		else if (iValue > vecNum[iMid])
		{
			idxEnd = iMid - 1;
		}
		else
		{
			idxBegin = iMid + 1;
		}
	}
	
	return -1;
}

// 画标记信息
tradestatistics.DrawSign = function () {
    //if (NULL == CUserInfo::Instance()->m_pInfo)
    {
        return tradestatistics.DrawSignNormal();
    }
   // else
   // {
	//	    return DrawSignTrain(strRetCode);
   // }
}

tradestatistics.DrawSignNormal = function () {
    // 画日期虚竖线
    var fontSize = 15;
    tradestatistics.m_cvtext.beginPath();
    tradestatistics.m_cvtext.fillStyle = "#ff0000";
    tradestatistics.m_cvtext.font = fontSize + "px arial";
    tradestatistics.m_cvtext.lineWidth = 0.2;
    tradestatistics.m_cvtext.strokeStyle = "#ff0000";
    for (var i = tradestatistics.m_idxBegin; (i < tradestatistics.m_idxEnd) && (i < tradestatistics.m_vecDate.length); i++){
        if ((tradestatistics.m_vecDate[i] % 10000 == 1231) && (i - tradestatistics.m_idxBegin<tradestatistics.m_vecXPoint.length)){
            tradestatistics.m_cvtext.moveTo(tradestatistics.m_vecXPoint[i - tradestatistics.m_idxBegin], tradestatistics.m_iPointTop);
            tradestatistics.m_cvtext.lineTo(tradestatistics.m_vecXPoint[i - tradestatistics.m_idxBegin], tradestatistics.m_iPointTop + tradestatistics.m_iPointHeight);
            //tradestatistics.m_cvtext.stroke();
            tradestatistics.m_cvtext.fillText(tradestatistics.m_vecDate[i] + '', 
                tradestatistics.m_vecXPoint[i - tradestatistics.m_idxBegin] - 30, 
                tradestatistics.m_iPointTop + tradestatistics.m_iPointHeight + fontSize);
        }
    }
    tradestatistics.m_cvtext.stroke();

    // 画一些标志性的Y轴业务数值
    fontSize = 12;
    tradestatistics.m_cvtext.font = fontSize + "px arial";
    var KM = tradestatistics.m_iShowMax - tradestatistics.m_iShowMin;
    for (var t = 0; t < 9; t++){
        tradestatistics.m_cvtext.fillText(Math.round(tradestatistics.m_iShowMax - (KM / 9.0)*t) + '', 
            tradestatistics.m_iPointLeft + 1, tradestatistics.m_iPointTop + 1 + fontSize + t * (tradestatistics.m_iPointHeight / 9.0));
    }
    tradestatistics.m_cvtext.fillText(Math.round(tradestatistics.m_iShowMax - (KM / 9.0) * 9) + '', 
        tradestatistics.m_iPointLeft + 1, tradestatistics.m_iPointTop + 1 + tradestatistics.m_iPointHeight);

    return true;
}

// 画右侧信息
tradestatistics.DrawInfo = function () {
   // if (NULL == CUserInfo::Instance()->m_pInfo)
    {
        return tradestatistics.DrawInfoNormal();
    }
  //  else
  //  {
	//	return DrawInfoTrain(strRetCode);
  //  }

}

tradestatistics.DrawInfoNormal = function () {
    var i = 1;
    var date = 0;
    var temp = "";
    var nBeg = 0;
    var nEnd = 0;
    var nMid = -1;

    tradestatistics.m_cvtext.beginPath();
    tradestatistics.m_cvtext.fillStyle = "#ff0000";
    tradestatistics.m_cvtext.font = "15px arial";
    //tradestatistics.m_cvtext.fillText(tradestatistics.m_strCode + "  " + tradestatistics.m_strName, tradestatistics.m_iInfoLeft + 20, tradestatistics.m_iInfoTop + i++*20);
    temp = "codecnt: " + tradestatistics.m_nCodeCnt + ", pointcnt: " + tradestatistics.m_nPointCnt;
    tradestatistics.m_cvtext.fillText(temp, tradestatistics.m_iInfoLeft + 10, tradestatistics.m_iInfoTop + i++*20);
    date = tradestatistics.m_vecDate[tradestatistics.m_iCurrDataIdx];
    temp = "Date: " + tradestatistics.m_vecDate[tradestatistics.m_iCurrDataIdx];
    tradestatistics.m_cvtext.fillText(temp, tradestatistics.m_iInfoLeft + 10, tradestatistics.m_iInfoTop + i++*20);
    for (var iter in tradestatistics.m_mapData)
    {
        nBeg = 0;
        nEnd = tradestatistics.m_mapData[iter].length - 1;
        //console.log("nBeg: " + nBeg + ", nEnd: " + nEnd);
        while (nBeg <= nEnd){
            nMid = Math.floor((nBeg + nEnd)/2);
            //console.log("date: " + date + ", map date: " + tradestatistics.m_mapData[iter][nMid].date);
            if (date == tradestatistics.m_mapData[iter][nMid].date){
                break;
            }
            else if (date < tradestatistics.m_mapData[iter][nMid].date){
                nBeg = nMid + 1;
            }
            else {
                nEnd = nMid - 1;
            }
        }
        if (nBeg > nEnd){
            continue;
        }
        //nMid = 0;
        temp = "trade: " + iter + ", count: " + tradestatistics.m_mapData[iter][nMid].count;
        //temp = tradestatistics.m_mapData[iter].type + ": " + tradestatistics.m_mapData[iter].items[tradestatistics.m_iCurrQuartIdx]/100;
        //tradestatistics.m_cvtext.fillText(temp, tradestatistics.m_iInfoLeft + 10, tradestatistics.m_iInfoTop + i++*20);
        //temp = "count: " + tradestatistics.m_mapData[iter][nMid].count;
        tradestatistics.m_cvtext.fillText(temp, tradestatistics.m_iInfoLeft + 10, tradestatistics.m_iInfoTop + i++*20);
        temp = "code: ";
        t=0;
        for (; (t<2) && (t<tradestatistics.m_mapData[iter][nMid].code.length); t++){
            temp = temp + tradestatistics.m_mapData[iter][nMid].code[t] + ',';
        }
        tradestatistics.m_cvtext.fillText(temp, tradestatistics.m_iInfoLeft + 10, tradestatistics.m_iInfoTop + i++*20);
        temp = "    ";
        for (; t<tradestatistics.m_mapData[iter][nMid].code.length; t++){
            temp = temp + tradestatistics.m_mapData[iter][nMid].code[t] + ',';
            if (0 == (t-1)%3){
                tradestatistics.m_cvtext.fillText(temp, tradestatistics.m_iInfoLeft + 10, tradestatistics.m_iInfoTop + i++*20);
                temp = "    ";
            }
        }
        if ("    " != temp){
            tradestatistics.m_cvtext.fillText(temp, tradestatistics.m_iInfoLeft + 10, tradestatistics.m_iInfoTop + i++*20);
        }
    }

    return true;
}

tradestatistics.MouseDown = function (ev) {
    tradestatistics.m_bCross = tradestatistics.m_bCross ? false : true;
}

tradestatistics.MouseMove = function (ev) {
    if (false == tradestatistics.m_bCross) {
        return true;
    }
    
    if (ev.clientX <= tradestatistics.m_iPointLeft || ev.clientX >= tradestatistics.m_iPointWidth
        || ev.clientY <= tradestatistics.m_iPointTop || ev.clientY >= tradestatistics.m_iPointHeight) {
        tradestatistics.show();
        return true;
    }

    if (false == tradestatistics.SetCurrPoint(ev)) {    // 设置当前位置对应到曲线坐标
        console.log("Error: Call tradestatistics.SetCurrPoint error.");
        return false;
    }

    if (false == tradestatistics.show()) {  // 刷新
        console.log("Error: Call tradestatistics.show error.");
        return false;
    }

    // 画十字星
    tradestatistics.m_cvtext.lineWidth = 0.5;
    tradestatistics.m_cvtext.beginPath();
    tradestatistics.m_cvtext.strokeStyle = "#ffffff";
    tradestatistics.m_cvtext.moveTo(ev.clientX, tradestatistics.m_iPointTop);
    tradestatistics.m_cvtext.lineTo(ev.clientX, tradestatistics.m_iPointHeight);
    tradestatistics.m_cvtext.moveTo(tradestatistics.m_iPointLeft, ev.clientY);
    tradestatistics.m_cvtext.lineTo(tradestatistics.m_iPointWidth, ev.clientY);
    //tradestatistics.m_cvtext.setLineDash([5, 20]);
    //ogc.lineTo(ev.clientX-oc.offsetLeft,ev.clientY-oc.offsetTop);
    tradestatistics.m_cvtext.stroke();

    return true;
}

tradestatistics.SetCurrPoint = function (ev) {
    var dw = 0.0;
    var dwDay = 0.0;
    var dRightX = 0.0;
    var dRightDayX = 0.0;
    var idxLeap = 0;

    tradestatistics.m_iMouseX = ev.clientX;
    tradestatistics.m_iMouseY = ev.clientY;

    dw = tradestatistics.m_iPointWidth * 1.0 / (1 + tradestatistics.m_idxEnd - tradestatistics.m_idxBegin);
    dRightX = tradestatistics.m_iPointWidth - tradestatistics.m_iMouseX + tradestatistics.m_iPointLeft; // x轴距右边界的距离
    tradestatistics.m_iCurrDataIdx = Math.round((dRightX - dw) / dw) + tradestatistics.m_idxBegin;
    if (0 > tradestatistics.m_iCurrDataIdx)
    {
        tradestatistics.m_iCurrDataIdx = 0;
    }

    if (tradestatistics.m_iCurrDataIdx >= tradestatistics.m_vecXPoint.length)
    {
        tradestatistics.m_iCurrDataIdx = 0;
        return false;
    }

    return true;
}

tradestatistics.Zoom = function (iScale) {
	var iTempBegin  = 0;
	var iTempEnd    = 0;

	if (ZoomType.add == iScale) // 精度扩大
	{
	    iTempBegin = tradestatistics.m_idxBegin + Math.floor((tradestatistics.m_iCurrQuartIdx - tradestatistics.m_idxBegin)/2);
	    iTempEnd = tradestatistics.m_idxEnd - Math.floor((tradestatistics.m_idxEnd - tradestatistics.m_iCurrQuartIdx)/2);
		if (0 > iTempBegin)
		{
			iTempBegin = 0;
		}
		if ((0 < tradestatistics.m_vecQuartXLine.length) && (tradestatistics.m_vecQuartXLine.length <= iTempEnd))
		{
		    iTempEnd = tradestatistics.m_vecQuartXLine.length - 1;
		}
		if ((0 < tradestatistics.m_vecIndex.length) && (tradestatistics.m_vecIndex.length <= iTempEnd))
		{
		    iTempEnd = tradestatistics.m_vecIndex.length - 1;
		}
		if (iTempEnd < iTempBegin)
		{
		    iTempBegin = tradestatistics.m_idxBegin;
		    iTempEnd = tradestatistics.m_idxEnd;
		}
	}

	if (ZoomType.sub == iScale) // 精度缩小
	{
	    iTempBegin = tradestatistics.m_idxBegin - (tradestatistics.m_iCurrQuartIdx - tradestatistics.m_idxBegin);
	    iTempEnd = tradestatistics.m_idxEnd + (tradestatistics.m_idxEnd - tradestatistics.m_iCurrQuartIdx);
		if (0 > iTempBegin)
		{
			iTempBegin = 0;
		}
		if ((0 < tradestatistics.m_vecQuartXLine.length) && (tradestatistics.m_vecQuartXLine.length <= iTempEnd))
		{
		    iTempEnd = tradestatistics.m_vecQuartXLine.length - 1;
		}
		if ((0 < tradestatistics.m_vecIndex.length) && (tradestatistics.m_vecIndex.length <= iTempEnd))
		{
		    iTempEnd = tradestatistics.m_vecIndex.length - 1;
		}
		if (iTempEnd < iTempBegin)
		{
		    iTempBegin = tradestatistics.m_idxBegin;
		    iTempEnd = tradestatistics.m_idxEnd;
		}
	}

	if (ZoomType.left == iScale) // 焦点左移
	{

	}

	if (ZoomType.right == iScale) // 焦点右移
	{

	}

	tradestatistics.m_idxBegin = iTempBegin;
	tradestatistics.m_idxEnd = iTempEnd;

	if (false == tradestatistics.GetShowMValue()){  // 得到显示区段最值
		console.log("Call GetShowMValue.");
		return false;
	}

	if (false == tradestatistics.show()) {
	    console.log("call tradestatistics.show() error.");
	    return false;
	}

	return true;
}

tradestatistics.GetShowMValue = function(){
	var iMax = 0;
	var iMin = 0;
	var iTemp = 0;
	var bInit = false;
    /*
    	    var objMVal = new CMValue;
	    objMVal.m_iMax = tradestatistics.m_iShowMax;
	    objMVal.m_iMin = tradestatistics.m_iShowMin;
    */
	tradestatistics.m_iShowMax = tradestatistics.m_iShowMin = 0;
	var ptrVec;
	for (var iter in tradestatistics.m_mapQuartYLine)
	{
		iMax = iMin = 0;
		bInit = false;
		ptrVec = tradestatistics.m_mapQuartYLine[iter].items;
		for (var i = tradestatistics.m_idxBegin; (i < tradestatistics.m_idxEnd) && (i < ptrVec.length); i++)
		{
			iTemp = ptrVec[i];
			if ((tradestatistics.m_mapQuartYLine[iter].max < iTemp) || (tradestatistics.m_mapQuartYLine[iter].min > iTemp))
			{
				continue;
			}
			if (false == bInit)
			{
				iMax = iMin = iTemp;
				bInit = true;
			}
			iMax = iTemp > iMax ? iTemp : iMax;
			iMin = iTemp < iMin ? iTemp : iMin;
		}
		var objMVal = new CMValue;
		objMVal.m_iMax = iMax;
		objMVal.m_iMin = iMin;
		tradestatistics.m_mapShowMValue[iter] = objMVal;

		if (tradestatistics.m_iShowMax == tradestatistics.m_iShowMin)
		{
		    tradestatistics.m_iShowMax = objMVal.m_iMax;
		    tradestatistics.m_iShowMin = objMVal.m_iMin;
		}
		tradestatistics.m_iShowMax = objMVal.m_iMax > tradestatistics.m_iShowMax ? objMVal.m_iMax : tradestatistics.m_iShowMax;
		tradestatistics.m_iShowMin = objMVal.m_iMin < tradestatistics.m_iShowMin ? objMVal.m_iMin : tradestatistics.m_iShowMin;
	}

	for (var iter in tradestatistics.m_mapDayYLine)
	{
		iMax = iMin = 0;
		bInit = false;
		ptrVec = tradestatistics.m_mapDayYLine[iter].items;
		var index1 = tradestatistics.m_idxBegin > 0 ? tradestatistics.m_idxBegin - 1 : tradestatistics.m_idxBegin;
		var index2 = tradestatistics.m_idxEnd > 0 ? tradestatistics.m_idxEnd - 1 : tradestatistics.m_idxEnd;
		while ((tradestatistics.m_vecIndex.length > index1) && (-1 == tradestatistics.m_vecIndex[index1])) index1++;
		while ((0 < index2) && (-1 == tradestatistics.m_vecIndex[index2])) index2--;
		console.debug("index1: " + index1 + ", index2: " + index2 + ", m_vecIndex[index1]: " + tradestatistics.m_vecIndex[index1]
		    + ", m_vecIndex[index2]: " + tradestatistics.m_vecIndex[index2]);
		for (var i = tradestatistics.m_vecIndex[index1]; (i < tradestatistics.m_vecIndex[index2]) && (i < ptrVec.length); i++)
		{
			iTemp = ptrVec[i];
			if ((tradestatistics.m_mapDayYLine[iter].m_iMax < iTemp) || (tradestatistics.m_mapDayYLine[iter].m_iMin > iTemp))
			{
				continue;
			}
			if (false == bInit)
			{
				iMax = iMin = iTemp;
				bInit = true;
			}
			iMax = iTemp > iMax ? iTemp : iMax;
			iMin = iTemp < iMin ? iTemp : iMin;
		}
		var objMVal = new CMValue;
		objMVal.m_iMax = iMax;
		objMVal.m_iMin = iMin;
		tradestatistics.m_mapShowMValue[iter] = objMVal;
		console.debug("type: " + iter + ", iMax: " + iMax + ", iMin: " + iMin);

		if (tradestatistics.m_iShowMax == tradestatistics.m_iShowMin)
		{
		    tradestatistics.m_iShowMax = objMVal.m_iMax;
		    tradestatistics.m_iShowMin = objMVal.m_iMin;
		}
		tradestatistics.m_iShowMax = objMVal.m_iMax > tradestatistics.m_iShowMax ? objMVal.m_iMax : tradestatistics.m_iShowMax;
		tradestatistics.m_iShowMin = objMVal.m_iMin < tradestatistics.m_iShowMin ? objMVal.m_iMin : tradestatistics.m_iShowMin;
	}

	if (true == tradestatistics.m_conf.absolute)
	{
	    for (var iterMVal in tradestatistics.m_mapShowMValue)
		{
	        tradestatistics.m_mapShowMValue[iterMVal].m_iMax = tradestatistics.m_iShowMax;
	        tradestatistics.m_mapShowMValue[iterMVal].m_iMin = tradestatistics.m_iShowMin;
		}
	}

	if (false == tradestatistics.AdjustMValue())
	{
		console.error("Call AdjustMValue error");
		return false;
	}

	return true;
}

tradestatistics.AdjustMValue = function () {
	var iTimes = 0;
	var iTemp = 0;
	if (true == tradestatistics.m_mapShowMValue.hasOwnProperty("DayPR"))
	{
	    iTemp = (tradestatistics.m_mapShowMValue["DayPR"].m_iMin == 0) ? 0 : 1.0 * tradestatistics.m_mapShowMValue["DayPR"].m_iMax / tradestatistics.m_mapShowMValue["DayPR"].m_iMin;
		console.debug("AdjustYVal DayPR: " + iTemp);
		if ((iTemp > 100) || (iTemp <= 0))
		{
			return true;
		}
		iTimes = iTimes > iTemp ? iTimes : iTemp;
	}
	if (true == tradestatistics.m_mapShowMValue.hasOwnProperty("DayReinPrice"))
	{
	    iTemp = (tradestatistics.m_mapShowMValue["DayReinPrice"].m_iMin == 0) ? 0 : 1.0 * tradestatistics.m_mapShowMValue["DayReinPrice"].m_iMax / tradestatistics.m_mapShowMValue["DayReinPrice"].m_iMin;
		console.debug("AdjustYVal DayReinPrice: " + iTemp);
		if ((iTemp > 100) || (iTemp <= 0))
		{
			return true;
		}
		iTimes = iTimes > iTemp ? iTimes : iTemp;
	}
	if (true == tradestatistics.m_mapShowMValue.hasOwnProperty("DayCovery"))
	{
	    iTemp = (tradestatistics.m_mapShowMValue["DayCovery"].m_iMin == 0) ? 0 : 1.0 * tradestatistics.m_mapShowMValue["DayCovery"].m_iMax / tradestatistics.m_mapShowMValue["DayCovery"].m_iMin;
		console.debug("AdjustYVal DayCovery: " + iTemp);
		if ((iTemp > 100) || (iTemp <= 0))
		{
			return true;
		}
		iTimes = iTimes > iTemp ? iTimes : iTemp;
	}
	console.debug("AdjustYVal dTimes: " + iTimes);
	if (iTimes < 2)
	{
		return true;
	}

	if (true == tradestatistics.m_mapShowMValue.hasOwnProperty("DayPR"))
	{
	    iTemp = 1.0 * tradestatistics.m_mapShowMValue["DayPR"].m_iMax / tradestatistics.m_mapShowMValue["DayPR"].m_iMin;
	    tradestatistics.m_mapShowMValue["DayPR"].m_iMax = tradestatistics.m_mapShowMValue["DayPR"].m_iMax * iTimes / iTemp;
	}
	if (true == tradestatistics.m_mapShowMValue.hasOwnProperty("DayReinPrice"))
	{
	    iTemp = 1.0 * tradestatistics.m_mapShowMValue["DayReinPrice"].m_iMax / tradestatistics.m_mapShowMValue["DayReinPrice"].m_iMin;
	    tradestatistics.m_mapShowMValue["DayReinPrice"].m_iMax = tradestatistics.m_mapShowMValue["DayReinPrice"].m_iMax * iTimes / iTemp;
	}
	if (true == tradestatistics.m_mapShowMValue.hasOwnProperty("DayCovery"))
	{
	    iTemp = 1.0 * tradestatistics.m_mapShowMValue["DayCovery"].m_iMax / tradestatistics.m_mapShowMValue["DayCovery"].m_iMin;
	    tradestatistics.m_mapShowMValue["DayCovery"].m_iMax = tradestatistics.m_mapShowMValue["DayCovery"].m_iMax * iTimes / iTemp;
	}
	console.debug("execute AdjustYVal");

	return true;
}
