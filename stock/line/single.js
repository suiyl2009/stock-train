var single = single || {};

single.m_canvas = document.getElementById("linecanvas");
single.m_cvtext = single.m_canvas.getContext("2d");

single.init = function () {
    // 配置
    single.m_conf = conf.get('single');
    single.m_global = conf.get('global');

	// view 画点数区域
	single.m_iPointTop      = 0;
	single.m_iPointLeft     = 0;
	single.m_iPointHeight   = 0;
	single.m_iPointWidth    = 0;

	// view 显示信息区域
	single.m_iInfoTop       = 0;
	single.m_iInfoLeft      = 0;
	single.m_iInfoHeight    = 0;
	single.m_iInfoWidth     = 0;

	single.m_iMouseX        = 0;
	single.m_iMouseY        = 0;
	single.m_iCurrQuartIdx  = 0;
	single.m_iCurrDayIdx = 0;

	single.m_bCross = false;

	single.m_strCode = "";
	single.m_strName = "";

	single.m_iMax = 0;
	single.m_iMin = 0;
	single.m_vecIndex       = [];
	single.m_vecQuartXLine  = [];
	single.m_vecDayXLine    = [];
	single.m_mapQuartYLine  = {};
	single.m_mapDayYLine    = {};

	// 曲线点数数据
	single.m_idxBegin = 0; // 显示点数下标范围[m_idxBegin, m_idxEnd)
	single.m_idxEnd   = 0;

	single.m_iShowMax = 0;
	single.m_iShowMin = 0;
	single.m_mapShowMValue    = {};
	single.m_bpReport = {
		m_mvY:{},
		m_vecX:[]
	};
	single.m_bpTrend = {
		m_mvY:{},
		m_vecX:[]
	};
	
	if (false == single.req()) {
	    console.log("call single.req() error");
		return false;
	}
	
	if (false == single.show()) {
	    console.log("call single.show() error.");
		return false;
	}
	
	return true;
}

single.req = function() {
    /*single.m_strCode = "000002";
    single.m_strName = "万科A";

	var response = '{"method": "single.line", "retcode": "0000", "result": {"max": 146388, "min": 0, "qyline": [{"type": "ROE", "max": 2375, "min": 0, "items": [1917, 827, 612, 196, 2153, 931, 691, 249, 2144, 927, 683, 260, 1982, 784, 653, 268, 1778, 839, 720, 293, 1536, 0, 761, 0, 1324, 0, 676, 0, 2375, 0, 1069, 0, 2189, 0, 1290, 0, 1869, 0, 1205, 0]}, {"type": "Revenue", "max": 146388, "min": 1885, "items": [146388, 63139, 40961, 9497, 135418, 63415, 41390, 13999, 103116, 46128, 30722, 10345, 71782, 29308, 19988, 7970, 50713, 22380, 16766, 7504, 48881, 29543, 21808, 8164, 40991, 22541, 17255, 6400, 35526, 14173, 11096, 4114, 17918, 8508, 6669, 2303, 10558, 5556, 4334, 1885]}], "qxline": [20141231, 20140930, 20140630, 20140331, 20131231, 20130930, 20130630, 20130331, 20121231, 20120930, 20120630, 20120331, 20111231, 20110930, 20110630, 20110331, 20101231, 20100930, 20100630, 20100331, 20091231, 20090930, 20090630, 20090331, 20081231, 20080930, 20080630, 20080331, 20071231, 20070930, 20070630, 20070331, 20061231, 20060930, 20060630, 20060331, 20051231, 20050930, 20050630, 20050331]}}';
    */

    /*
    请求字段列表
    字段名称	值类型	备注
    method	字符串single.line	
    code	字符串	股票代码
    item	数组[]	需要返回的曲线列表
    date	整型或空	结束日期，例如20170121
    before	整数	以date为标准向前数量，以财报为准数量
    after	整数	以date为标准向后数量，以财报数据为准

    响应字段列表
    字段名称	值类型	备注
    method	同上	同上
    retcode	字符串	错误码，成功为0000
    result	数组[{},{},{}…]	响应字段值。数组元素为{“cde”: xxx, ”item1”: xxx, …}

    // 远程获取曲线数据
    jRoot.clear();
    jRoot["method"] = "single.line";
    jRoot["code"] = m_strCode;
    for (iter = CConfig::Instance()->m_objSingle.m_mapShow.begin();
		iter != CConfig::Instance()->m_objSingle.m_mapShow.end(); iter++)
    {
        jRoot["item"].append(iter->second.m_strLine);
    }
    jRoot["date"] = CConfig::Instance()->m_iLastDate;
    jRoot["before"] = -1;
    jRoot["after"] = 0;*/
    if ((0 <= gdata.m_data.idx) && (gdata.m_data.codes.length > gdata.m_data.idx)){
        single.m_strCode = gdata.m_data.codes[gdata.m_data.idx];
    }
    else {
        console.log("ERROR: gdata.m_data.idx: " + " error. gdata.m_data.codes.length: " + gdata.m_data.codes.length);
        return false;
    }
    single.m_conf = conf.get('single');
    single.m_global = conf.get('global');
    var vecItems = [];
    if (undefined == single.m_conf.show) {
        console.log("ERROR: single.m_conf.show undefined.");
        return false;
    }
    for (var key in single.m_conf.show) {
        vecItems.push(key.trim());
    }
    var req =
        {
            "method": "single.line",
            "code": single.m_strCode,
            "item": vecItems,
            "date": single.m_global.date,
            "before": -1,
            "after": 0
        };
    console.log('single.req: ' + JSON.stringify(req));
    function remoteSingle(objJson) {
        if (objJson.retcode != '0000') {
            console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
            return false;
        }
        //single.m_strCode = gdata.m_data.codes[]
        single.m_strName = objJson.result.name;
        single.m_iMax = objJson.result.max;
        single.m_iMin = objJson.result.min;
        single.m_vecIndex = [];
        single.m_mapQuartYLine = {};
        single.m_vecQuartXLine = [];
        single.m_mapDayYLine = {};
        single.m_vecDayXLine = [];

        if (undefined != objJson.result.index)
        {
            single.m_vecIndex = objJson.result.index;
            //for (var i = 0; i < objJson.result.index.length; i++)
            //{
            //    single.m_vecIndex.push_back(jRoot["index"][i].asInt());
            //}
        }
        if (undefined != objJson.result.qyline) {
            for (var i = 0; i < objJson.result.qyline.length; i++) {
                single.m_mapQuartYLine[objJson.result.qyline[i].type] = objJson.result.qyline[i];
                //console.log(objJson.result.qyline[i].type);
                //console.log(objJson.result.qyline[i]);
            }
        }
        if (undefined != objJson.result.qxline) {
            single.m_vecQuartXLine = objJson.result.qxline;
        }

        if (undefined != objJson.result.dyline) {
            for (var i = 0; i < objJson.result.dyline.length; i++) {
                single.m_mapDayYLine[objJson.result.dyline[i].type] = objJson.result.dyline[i];
                //console.log(objJson.result.dyline[i].type);
                //console.log(single.m_mapDayYLine[objJson.result.dyline[i].type]);
            }
        }
        if (undefined != objJson.result.dxline) {
            single.m_vecDayXLine = objJson.result.dxline;
        }
        //console.log(single.m_iMax);
        //console.log(single.m_iMin);
        //console.log(single.m_vecQuartXLine);
        //console.log(single.m_mapQuartYLine)
        return true;
    }
    gdata.send(req, remoteSingle);

    var indexline = conf.get('indexline');  // 指数曲线
    console.log("indexline.show: " + indexline.show + ", indexline.code: " + indexline.code);
    if ((0 < single.m_vecDayXLine.length) && (true == indexline.show) && ("" != indexline.code)){
        req = 
            {
                "method": "index.single",
                "code": indexline.code,
                "date": single.m_global.date,
                "before": -1,
                "after": 0
            };
        function remoteIndex(objJson) {
            if (objJson.retcode != '0000') {
                console.log("ERROR: retcode[" + objJson.retcode + "] != 0000");
                return false;
            }

            var vecX = objJson.result.dxline;
            var vecY = objJson.result.dyline;
            var vecFilter = [];
            var i=0;
            var j=0;
            var max = 0;
            var min = 0;
            if (0 < vecY.length){
                max = min = vecY[0];
            }
            while (i < single.m_vecDayXLine.length && j < vecX.length){
                if (single.m_vecDayXLine[i] > vecX[j]){
                    vecFilter.push(0);
                    min = min < 0 ? min : 0;
                    max = max > 0 ? max : 0;
                    i++;
                }
                else if (single.m_vecDayXLine[i] < vecX[j]){
                    j++;
                }
                else {
                    vecFilter.push(vecY[j]);
                    min = min < 0 ? min : 0;
                    max = max > vecY[j] ? max : vecY[j];
                    i++;
                    j++;
                }
            }
            while (i < single.m_vecDayXLine.length){
                vecFilter.push(0);
                i++;
            }
            single.m_mapDayYLine[objJson.result.code] = {"type":objJson.result.code, "max":max, "min":min, "items":vecFilter};

            return true;
        }
        gdata.send(req, remoteIndex);
    }

    // 显示坐标区域
	single.m_idxBegin = 0;
	single.m_idxEnd = single.m_vecQuartXLine.length;

    // 显示区间最值
	single.m_iShowMax = single.m_iMax;
	single.m_iShowMin = single.m_iMin;
	for (var iterMValue in single.m_mapQuartYLine){
	    var objMVal = new CMValue;
	    objMVal.m_iMax = single.m_iShowMax;
	    objMVal.m_iMin = single.m_iShowMin;
	    if (false == single.m_conf.absolute) {
	        objMVal.m_iMax = single.m_mapQuartYLine[iterMValue].max;
	        objMVal.m_iMin = single.m_mapQuartYLine[iterMValue].min;
	    }
		single.m_mapShowMValue[iterMValue] = objMVal;
	}
	for (var iterMValue in single.m_mapDayYLine){
	    var objMVal = new CMValue;
	    objMVal.m_iMax = single.m_iShowMax;
	    objMVal.m_iMin = single.m_iShowMin;
	    if (false == single.m_conf.absolute) {
	        objMVal.m_iMax = single.m_mapDayYLine[iterMValue].max;
	        objMVal.m_iMin = single.m_mapDayYLine[iterMValue].min;
	    }
		single.m_mapShowMValue[iterMValue] = objMVal;
	}

	return true;
}

single.show = function() {
    //console.log("single.show");

    single.m_cvtext.clearRect(0, 0, single.m_canvas.width, single.m_canvas.height);

    single.m_cvtext.beginPath();
    single.m_cvtext.fillStyle = "#ffffff";
    single.m_cvtext.font = "20px SimSun";

    // 左侧曲线区域
    single.m_iPointTop = 2;
    single.m_iPointLeft = 2;
    single.m_iPointHeight = single.m_canvas.height - 35;
    single.m_iPointWidth = single.m_canvas.width - 200;

    // 右侧信息区域
    single.m_iInfoTop = 2;
    single.m_iInfoLeft = single.m_iPointLeft + single.m_iPointWidth;
    single.m_iInfoHeight = single.m_iPointHeight;
    single.m_iInfoWidth = single.m_canvas.width - 2 - single.m_iInfoLeft;
	
    //画左面的边框
    //-------------->
    //              |
    //<------------\|/
    single.m_cvtext.lineWidth = 1.5;
    single.m_cvtext.beginPath();
    single.m_cvtext.strokeStyle = "#ff0000";
    single.m_cvtext.moveTo(single.m_iPointLeft, single.m_iPointTop);
    single.m_cvtext.lineTo(single.m_iPointLeft + single.m_iPointWidth, single.m_iPointTop);
    single.m_cvtext.lineTo(single.m_iPointLeft + single.m_iPointWidth, single.m_iPointTop + single.m_iPointHeight);
    single.m_cvtext.lineTo(single.m_iPointLeft, single.m_iPointTop + single.m_iPointHeight);
    single.m_cvtext.lineTo(single.m_iPointLeft, single.m_iPointTop);
    single.m_cvtext.stroke();

    // 画右面的边框
    single.m_cvtext.lineWidth = 1.5;
    single.m_cvtext.beginPath();
    single.m_cvtext.strokeStyle = "#ff0000";
    single.m_cvtext.moveTo(single.m_iInfoLeft, single.m_iInfoTop);
    single.m_cvtext.lineTo(single.m_iInfoLeft + single.m_iInfoWidth, single.m_iInfoTop);
    single.m_cvtext.lineTo(single.m_iInfoLeft + single.m_iInfoWidth, single.m_iInfoTop + single.m_iInfoHeight);
    single.m_cvtext.lineTo(single.m_iInfoLeft, single.m_iInfoTop + single.m_iInfoHeight);
    single.m_cvtext.lineTo(single.m_iInfoLeft, single.m_iInfoTop);
    single.m_cvtext.stroke();

    // 画八条虚线
    //ctx.lineWidth = 1;
    for (var y = single.m_iPointHeight + single.m_iPointTop - single.m_iPointHeight / 9;
        y > single.m_iPointTop + 10;y -= single.m_iPointHeight / 9){
        single.m_cvtext.lineWidth = 0.5;
        single.m_cvtext.beginPath();
        single.m_cvtext.strokeStyle = "#ff0000";
        single.m_cvtext.moveTo(single.m_iPointLeft, y);
        single.m_cvtext.lineTo(single.m_iPointLeft + single.m_iPointWidth, y);
        single.m_cvtext.stroke();
    }

    // 画左侧曲线
    if (false == single.DrawPoint()) {
        console.log("Call single.DrawPoint error");
        return false;
    }

    // 画标记信息
    if (false == single.DrawSign()) {
        console.log("Call single.DrawSign error");
        return false;
    }

    // 画右侧显示信息
    if (false == single.DrawInfo()) {
        console.log("Call single.DrawInfo error");
        return false;
    }
	
	return true;
}

// 画左侧曲线
single.DrawPoint = function () {

    if (false == single.GetPoint()){
        console.log("Call single.GetPoint error");
        return false;
    }
    //console.log("line rect: " + single.m_iPointTop + ", " + single.m_iPointHeight 
    //    + ", " + single.m_iPointLeft + ", " + single.m_iPointWidth);

    console.log('DrawPoint conf: ');
    console.log(single.m_conf.show);
    for (var iter in single.m_bpReport.m_mvY) {  // 财报曲线
        // 选择画笔颜色
        single.m_cvtext.lineWidth = 1;
        single.m_cvtext.beginPath();
        single.m_cvtext.strokeStyle = "#ff0000";
        if (single.m_conf.show.hasOwnProperty(iter)) {
            single.m_cvtext.strokeStyle = single.m_conf.show[iter].color;
            //console.log('draw color. type: ' + iter + ', color: ' + single.m_cvtext.strokeStyle);
        }
        //console.log('draw color. type: ' + iter + ', color: ' + single.m_cvtext.strokeStyle + ', conf color: ' + single.m_conf.show[iter].color);

        var ptrVecY = single.m_bpReport.m_mvY[iter];
        //DEBUGLOG("(*ptrVecY).size(): "<< (*ptrVecY).size()<<"m_bpReport.m_vecX.size(): "<< m_bpReport.m_vecX.size());
        for (var j = 1; j < ptrVecY.length; j++){
            if ((0 == ptrVecY[j - 1]) || (0 == ptrVecY[j])){
                continue;
            }
            single.m_cvtext.moveTo(single.m_bpReport.m_vecX[j], ptrVecY[j]);
            single.m_cvtext.lineTo(single.m_bpReport.m_vecX[j - 1], ptrVecY[j - 1]);
            //DEBUGLOG("m_bpReport.m_vecX[j]: "<< m_bpReport.m_vecX[j]<<", (*ptrVecY)[j]: "<< (*ptrVecY)[j]);
        }
        single.m_cvtext.stroke();
    }

    for (var iter in single.m_bpTrend.m_mvY) {    // 趋势曲线
        // 选择画笔颜色
        single.m_cvtext.lineWidth = 1;
        single.m_cvtext.beginPath();
        single.m_cvtext.strokeStyle = "#ffffff";
        if (single.m_conf.show.hasOwnProperty(iter)) {
            single.m_cvtext.strokeStyle = single.m_conf.show[iter].color;
        }

        var ptrVecY = single.m_bpTrend.m_mvY[iter];
        //DEBUGLOG("(*ptrVecY).size(): " << (*ptrVecY).size()<<", m_bpTrend.m_vecX.size(): "<< m_bpTrend.m_vecX.size());
        for (var j = 1; j < ptrVecY.length; j++){
            if ((0 == ptrVecY[j - 1]) || (0 == ptrVecY[j])){
                continue;
            }
            single.m_cvtext.moveTo(single.m_bpTrend.m_vecX[j], ptrVecY[j]);
            single.m_cvtext.lineTo(single.m_bpTrend.m_vecX[j - 1], ptrVecY[j - 1]);
            //DEBUGLOG("m_bpTrend.m_vecX[j]: " << m_bpTrend.m_vecX[j] << ", (*ptrVecY)[j]: " << (*ptrVecY)[j]);
        }
        //DEBUGLOG("Draw Line OK");
        single.m_cvtext.stroke();
    }

    return true;
}

// 得到曲线点数坐标
single.GetPoint = function () {
    var iMax   = 0;
    var iMin   = 0;
    var dw  = 0.0;
    var dh  = 0.0;
    var strType = "";
    var vecYPoint = [];

    single.m_bpReport.m_mvY = {};
    single.m_bpReport.m_vecX = [];
    single.m_bpTrend.m_mvY = {};
    single.m_bpTrend.m_vecX = [];

    dw = (single.m_iPointWidth*1.0) / (1 + single.m_idxEnd - single.m_idxBegin); // 用浮点数减小误差
    for (var i = single.m_idxBegin; (i < single.m_idxEnd) && (i < single.m_vecQuartXLine.length) ; i++) {    // 财报X轴坐标
        single.m_bpReport.m_vecX.push(single.m_iPointLeft + single.m_iPointWidth - dw - (i - single.m_idxBegin) * dw); // 从右向左画，右边空出1个的空间
    }

    for (var iter in single.m_mapQuartYLine) { // 财报Y轴坐标
        iMax = iMin = 0;
        vecYPoint = [];
        if (single.m_mapShowMValue.hasOwnProperty(iter)){
            iMax = single.m_mapShowMValue[iter].m_iMax;
            iMin = single.m_mapShowMValue[iter].m_iMin;
        }
        // 计算dh
        if (iMax == iMin){
            dh = 0;
        }
        else {
            dh = 1.0*single.m_iPointHeight / (iMax - iMin);
        }
        // 计算Y轴坐标
        for (var j = single.m_idxBegin; (j < single.m_idxEnd) && (j < single.m_mapQuartYLine[iter].items.length); j++){
            if (0 == dh){
                vecYPoint.push(single.m_iPointTop + single.m_iPointHeight/2);
            }
            else{
                vecYPoint.push(single.m_iPointTop + single.m_iPointHeight - (single.m_mapQuartYLine[iter].items[j] - iMin) * dh);
            }
        }
        single.m_bpReport.m_mvY[iter] = vecYPoint.slice(0);
    }

    if ((0 != single.m_vecIndex.length) && (single.m_vecIndex.length != single.m_vecQuartXLine.length)){
        //strRetCode = CERROUTRANGE;
        //ERRORLOG("m_vecIndex.size()["<< m_vecIndex.size()<<"] != m_vecQuartXLine.size()["<< m_vecQuartXLine.size()
        //	<<"]. strRetCode: "<<strRetCode);
        console.log("Error: single.m_vecIndex.length[" + single.m_vecIndex.length + "] != single.m_vecQuartXLine.length[" 
            + single.m_vecQuartXLine.length + "]");
        return false;
    }

    for (var i = single.m_idxBegin; (i < single.m_idxEnd) && (i < single.m_vecIndex.length); i++) {    //分段画日线坐标
        var dPointLeft = single.m_iPointLeft + single.m_iPointWidth - dw - (i - single.m_idxBegin) * dw;
        if (false == single.GetParsePoint(dPointLeft, 1.0 * single.m_iPointTop, dw, 1.0 * single.m_iPointHeight, i)) {
            //ERRORLOG("Call GetParsePoint error. dPointLeft: "<< dPointLeft<<", 1.0*m_iPointTop: "<< 1.0*m_iPointTop
			//    <<", dw: "<<dw<<", 1.0*m_iPointHeight: "<< 1.0*m_iPointHeight<<", i: "<<i<<", strRetCode: "
            //    <<strRetCode);
            console.log("Error: Call GetParsePoint error. dPointLeft: " + dPointLeft + ", 1.0*m_iPointTop: " + 1.0 * single.m_iPointTop
                + ", dw: " + dw + ", 1.0*m_iPointHeight: " + 1.0 * single.m_iPointHeight + ", i: " + i);
            return false;
        }
    }

    return true;
}

single.GetParsePoint = function (dLeft, dTop, dWidth, dHeight, i) {   //计算i --- i-1段
    var iMax        = 0;
    var iMin        = 0;
    var dw       = 0.0;
    var dh       = 0.0;
    var idxBegin    = 0;
    var idxEnd      = 0;
    var idxLeap     = 0;
    vecX = [];
    vecY = [];

    if (((i - 1 >= 0) && (single.m_vecIndex[i - 1] < 0)) || (single.m_vecIndex[i] <= 0))
    {
        return true;
    }

    if (0 == i) //画m_vecDayXLine[0] --- m_vecDayXLine下标到m_vecIndex[i]的曲线
    {
        if (single.m_vecIndex[i] < 40)
        {
            dw = dWidth * 1.0 / 60;
        }
        else
        {
            dw = dWidth * 1.0 / single.m_vecIndex[0];
        }
        idxBegin = 0;
        idxEnd = single.m_vecIndex[i];
        idxLeap = 60 - single.m_vecIndex[i];
    }
    else // 正常情况每个点的宽度
    {
        if (single.m_vecIndex[i] == single.m_vecIndex[i - 1])
        {
            return true;
        }
        dw = dWidth * 1.0 / (single.m_vecIndex[i] - single.m_vecIndex[i-1]);
        idxBegin = single.m_vecIndex[i - 1];
        idxEnd = single.m_vecIndex[i];
        idxLeap = 0;
    }

    for (var t = idxBegin; t < idxEnd; t++) // vecX坐标
    {
        vecX.push(dLeft+dWidth-(t-idxBegin)*dw-idxLeap*dw);
    }
    //single.m_bpTrend.m_vecX.insert(m_bpTrend.m_vecX.end(), vecX.begin(), vecX.end());
    single.m_bpTrend.m_vecX = single.m_bpTrend.m_vecX.concat(vecX);

    //map<string, CSingleYVal>::iterator iter;
    for (var iter in single.m_mapDayYLine)
    {
        vecY = [];
        iMax = iMin = 0;
        if (single.m_mapShowMValue.hasOwnProperty(iter)){
            iMax = single.m_mapShowMValue[iter].m_iMax;
            iMin = single.m_mapShowMValue[iter].m_iMin;
        }
        
        // 计算dh
        if (iMax == iMin)
        {
            dh = 0;
        }
        else
        {
            dh = 1.0*single.m_iPointHeight / (iMax - iMin);
        }
        // 计算Y轴坐标
        for (var j = idxBegin; (j < idxEnd) && (j < single.m_mapDayYLine[iter].items.length) ; j++)
        {
            if (0 == dh)
            {
                vecY.push(single.m_iPointTop + single.m_iPointHeight / 2);
            }
            else
            {
                vecY.push(single.m_iPointTop + single.m_iPointHeight - (single.m_mapDayYLine[iter].items[j] - iMin) * dh);
            }
        }

        if (undefined == single.m_bpTrend.m_mvY[iter]) {
            single.m_bpTrend.m_mvY[iter] = vecY;
        }
        else {
            single.m_bpTrend.m_mvY[iter] = single.m_bpTrend.m_mvY[iter].concat(vecY);
        }
    }

    return true;
}

// 画标记信息
single.DrawSign = function () {
    //if (NULL == CUserInfo::Instance()->m_pInfo)
    {
        return single.DrawSignNormal();
    }
   // else
   // {
	//	    return DrawSignTrain(strRetCode);
   // }
}

single.DrawSignNormal = function () {
    // 画日期虚竖线
    var fontSize = 15;
    single.m_cvtext.beginPath();
    single.m_cvtext.fillStyle = "#ff0000";
    single.m_cvtext.font = fontSize + "px arial";
    single.m_cvtext.lineWidth = 0.2;
    single.m_cvtext.strokeStyle = "#ff0000";
    if (0 < single.m_bpReport.m_vecX.length){
        for (var i = single.m_idxBegin; (i < single.m_idxEnd) && (i < single.m_vecQuartXLine.length); i++){
            if ((single.m_vecQuartXLine[i] % 10000 == 1231) && (i - single.m_idxBegin<single.m_bpReport.m_vecX.length)){
                single.m_cvtext.moveTo(single.m_bpReport.m_vecX[i - single.m_idxBegin], single.m_iPointTop);
                single.m_cvtext.lineTo(single.m_bpReport.m_vecX[i - single.m_idxBegin], single.m_iPointTop + single.m_iPointHeight);
                //single.m_cvtext.stroke();
                single.m_cvtext.fillText(single.m_vecQuartXLine[i] + '', 
                    single.m_bpReport.m_vecX[i - single.m_idxBegin] - 30, 
                    single.m_iPointTop + single.m_iPointHeight + fontSize);
            }
        }
    }
    else{
        var idxDateDay = 0;
        var idxPointDay = 0;
        for (var i = single.m_idxBegin; (i < single.m_idxEnd) && (i < single.m_vecIndex.length); i++){
            idxDateDay = single.m_vecIndex[i];
            idxPointDay = single.m_vecIndex[i] - single.m_vecIndex[single.m_idxBegin];
            if ((0 > idxDateDay) || (0 > idxPointDay)){
                continue;
            }
            if ((12 == single.m_vecDayXLine[idxDateDay] % 10000 / 100) || (1 == single.m_vecDayXLine[idxDateDay] % 10000 / 100)){
                single.m_cvtext.moveTo(single.m_bpTrend.m_vecX[idxPointDay], single.m_iPointTop);
                single.m_cvtext.lineTo(single.m_bpTrend.m_vecX[idxPointDay], single.m_iPointTop + single.m_iPointHeight);
                //single.m_cvtext.stroke();
                single.m_cvtext.fillText(single.m_vecDayXLine[idxDateDay] + '', 
                    single.m_bpTrend.m_vecX[idxPointDay] - 30, 
                    single.m_iPointTop + single.m_iPointHeight + fontSize);
            }
        }
    }
    single.m_cvtext.stroke();

    // 画一些标志性的Y轴业务数值
    fontSize = 12;
    single.m_cvtext.font = fontSize + "px arial";
    if (true == single.m_conf.absolute){
        // 只在左侧标注一组
        var KM = single.m_iShowMax - single.m_iShowMin;
        for (var t = 0; t < 9; t++){
            single.m_cvtext.fillText(Math.round(single.m_iShowMax - (KM / 9.0)*t) / 100 + '', 
                single.m_iPointLeft + 1, single.m_iPointTop + 1 + fontSize + t * (single.m_iPointHeight / 9.0));
        }
        single.m_cvtext.fillText(Math.round(single.m_iShowMax - (KM / 9.0) * 9) / 100 + '', 
            single.m_iPointLeft + 1, single.m_iPointTop + 1 + single.m_iPointHeight);
    }
    else{
		var iCnt = 0;
        for (var iter in single.m_mapQuartYLine){
            if (iCnt > 4){
                break;
            }
            single.m_cvtext.fillStyle = "#0000ff";
            if (single.m_conf.show.hasOwnProperty(iter)) {  // 选择画笔颜色
                single.m_cvtext.fillStyle = single.m_conf.show[iter].color;
            }

            var KM = single.m_mapShowMValue[iter].m_iMax - single.m_mapShowMValue[iter].m_iMin;
            for (var t = 0; t < 9; t++){
                single.m_cvtext.fillText(Math.round(single.m_mapShowMValue[iter].m_iMax - (KM / 9.0)*t) / 100 + '', 
                    single.m_iPointLeft + 1, single.m_iPointTop + 1 + (iCnt + 1) * fontSize + t * (single.m_iPointHeight / 9.0));
            }
            if (0 == iCnt){
                single.m_cvtext.fillText(Math.round(single.m_mapShowMValue[iter].m_iMax - (KM / 9.0) * 9) / 100 + '', 
                    single.m_iPointLeft + 1, single.m_iPointTop + 1 + (iCnt + 1) * fontSize + single.m_iPointHeight);
            }
            iCnt++;
        }

        for (var iter in single.m_mapDayYLine)
        {
            if (iCnt > 4)
            {
                break;
            }
            single.m_cvtext.fillStyle = "#0000ff";
            if (single.m_conf.show.hasOwnProperty(iter)) {  // 选择画笔颜色
                single.m_cvtext.fillStyle = single.m_conf.show[iter].color;
            }

            var KM = single.m_mapShowMValue[iter].m_iMax - single.m_mapShowMValue[iter].m_iMin;
            for (var t = 0; t < 9; t++)
            {
                single.m_cvtext.fillText(Math.round(single.m_mapShowMValue[iter].m_iMax - (KM / 9.0)*t) / 100 + '', 
                    single.m_iPointLeft + 1, single.m_iPointTop + 1 + (iCnt + 1) * fontSize + t * (single.m_iPointHeight / 9.0));
            }
            if (0 == iCnt)
            {
                single.m_cvtext.fillText(Math.round(single.m_mapShowMValue[iter].m_iMax - (KM / 9.0) * 9) / 100 + '', 
                    single.m_iPointLeft + 1, single.m_iPointTop + 1 + (iCnt + 1) * fontSize + single.m_iPointHeight - 20);
            }
            iCnt++;
        }
    }

    return true;
}

// 画右侧信息
single.DrawInfo = function () {
   // if (NULL == CUserInfo::Instance()->m_pInfo)
    {
        return single.DrawInfoNormal();
    }
  //  else
  //  {
	//	return DrawInfoTrain(strRetCode);
  //  }

}

single.DrawInfoNormal = function () {
    var i = 1;
    var temp = "";

    single.m_cvtext.beginPath();
    single.m_cvtext.fillStyle = "#ff0000";
    single.m_cvtext.font = "15px arial";
    single.m_cvtext.fillText(single.m_strCode + "  " + single.m_strName, single.m_iInfoLeft + 20, single.m_iInfoTop + i++*20);
    if ((0 < single.m_vecDayXLine.length) && (single.m_iCurrDayIdx < single.m_vecDayXLine.length))
    {
        temp = "Date: " + single.m_vecDayXLine[single.m_iCurrDayIdx];
    }
    else
    {
        temp = "Date: " + single.m_vecQuartXLine[single.m_iCurrQuartIdx];
    }
    single.m_cvtext.fillText(temp, single.m_iInfoLeft + 10, single.m_iInfoTop + i++*20);
    for (var iter in single.m_mapQuartYLine)
    {
        temp = single.m_mapQuartYLine[iter].type + ": " + single.m_mapQuartYLine[iter].items[single.m_iCurrQuartIdx]/100;
        single.m_cvtext.fillText(temp, single.m_iInfoLeft + 10, single.m_iInfoTop + i++*20);
    }
    for (var iter in single.m_mapDayYLine)
    {
        if ((0 > single.m_iCurrDayIdx) || (single.m_mapDayYLine[iter].items.length <= single.m_iCurrDayIdx))
        {
            continue;
        }
        temp = single.m_mapDayYLine[iter].type + ": " + single.m_mapDayYLine[iter].items[single.m_iCurrDayIdx]/100;
        single.m_cvtext.fillText(temp, single.m_iInfoLeft + 10, single.m_iInfoTop + i++*20);
    }

    return true;
}

single.MouseDown = function (ev) {
    single.m_bCross = single.m_bCross ? false : true;
}

single.MouseMove = function (ev) {
    if (false == single.m_bCross) {
        return true;
    }
    
    if (ev.clientX <= single.m_iPointLeft || ev.clientX >= single.m_iPointWidth
        || ev.clientY <= single.m_iPointTop || ev.clientY >= single.m_iPointHeight) {
        single.show();
        return true;
    }

    if (false == single.SetCurrPoint(ev)) {    // 设置当前位置对应到曲线坐标
        console.log("Error: Call single.SetCurrPoint error.");
        return false;
    }

    if (false == single.show()) {  // 刷新
        console.log("Error: Call single.show error.");
        return false;
    }

    // 画十字星
    single.m_cvtext.lineWidth = 0.5;
    single.m_cvtext.beginPath();
    single.m_cvtext.strokeStyle = "#ffffff";
    single.m_cvtext.moveTo(ev.clientX, single.m_iPointTop);
    single.m_cvtext.lineTo(ev.clientX, single.m_iPointHeight);
    single.m_cvtext.moveTo(single.m_iPointLeft, ev.clientY);
    single.m_cvtext.lineTo(single.m_iPointWidth, ev.clientY);
    //single.m_cvtext.setLineDash([5, 20]);
    //ogc.lineTo(ev.clientX-oc.offsetLeft,ev.clientY-oc.offsetTop);
    single.m_cvtext.stroke();

    return true;
}

single.SetCurrPoint = function (ev) {
    var dw = 0.0;
    var dwDay = 0.0;
    var dRightX = 0.0;
    var dRightDayX = 0.0;
    var idxLeap = 0;

    single.m_iMouseX = ev.clientX;
    single.m_iMouseY = ev.clientY;

    dw = single.m_iPointWidth * 1.0 / (1 + single.m_idxEnd - single.m_idxBegin);
    dRightX = single.m_iPointWidth - single.m_iMouseX + single.m_iPointLeft; // x轴距右边界的距离
    single.m_iCurrQuartIdx = Math.round((dRightX - dw) / dw) + single.m_idxBegin;
    if (0 > single.m_iCurrQuartIdx)
    {
        single.m_iCurrQuartIdx = 0;
    }

    if (single.m_iCurrQuartIdx >= single.m_vecQuartXLine.length)
    {
        single.m_iCurrQuartIdx = 0;
        return false;
    }

    if (0 == single.m_vecIndex.length) // 不需要生成dayx坐标
    {
        return true;
    }

    if (dRightX > dw * (single.m_iCurrQuartIdx - single.m_idxBegin + 1)) // 在m_iCurrQuartIdx到m_iCurrQuartIdx+1中找
    {
        if ((single.m_iCurrQuartIdx + 1 >= single.m_vecIndex.length)
            || (-1 == single.m_vecIndex[single.m_iCurrQuartIdx + 1])
            || (-1 == single.m_vecIndex[single.m_iCurrQuartIdx]))
        {
            return true;
        }
        dRightDayX = dRightX - dw * (single.m_iCurrQuartIdx - single.m_idxBegin + 1);
        dwDay = dw / (single.m_vecIndex[single.m_iCurrQuartIdx + 1] - single.m_vecIndex[single.m_iCurrQuartIdx]);
        single.m_iCurrDayIdx = single.m_vecIndex[single.m_iCurrQuartIdx] + Math.round(dRightDayX / dwDay);
    }
    else if (dRightX < dw * (single.m_iCurrQuartIdx - single.m_idxBegin + 1))  // 在m_iCurrQuartIdx-1到m_iCurrQuartIdx中找
    {
        if (-1 == single.m_vecIndex[0])
        {
            return true;
        }

        if (0 == single.m_iCurrQuartIdx)
        {
            if (single.m_vecIndex[0] < 40)
            {
                dwDay = dw * 1.0 / 60;
                idxLeap = 60 - single.m_vecIndex[0];
            }
            else
            {
                dwDay = dw * 1.0 / single.m_vecIndex[0];
                idxLeap = 0;
            }
            single.m_iCurrDayIdx = Math.round((dRightX - idxLeap * dwDay) / dwDay);
            if (0 > single.m_iCurrDayIdx)
            {
                single.m_iCurrDayIdx = 0;
            }
            //console.log("single.m_iCurrDayIdx: " + single.m_iCurrDayIdx);
            return true;
        }

        if ((-1 == single.m_vecIndex[single.m_iCurrQuartIdx - 1]) || (-1 == single.m_vecIndex[single.m_iCurrQuartIdx]))
        {
            return true;
        }
        dRightDayX = dRightX - dw * (single.m_iCurrQuartIdx - single.m_idxBegin);
        dwDay = dw / (single.m_vecIndex[single.m_iCurrQuartIdx] - single.m_vecIndex[single.m_iCurrQuartIdx - 1]);
        single.m_iCurrDayIdx = single.m_vecIndex[single.m_iCurrQuartIdx - 1] + Math.round(dRightDayX / dwDay);
    }
    else
    {
        if (-1 == single.m_vecIndex[single.m_iCurrQuartIdx])
        {
            return true;
        }
        single.m_iCurrDayIdx = single.m_vecIndex[single.m_iCurrQuartIdx];
    }

    return true;
}

single.Zoom = function (iScale) {
	var iTempBegin  = 0;
	var iTempEnd    = 0;

	if (ZoomType.add == iScale) // 精度扩大
	{
	    iTempBegin = single.m_idxBegin + Math.floor((single.m_iCurrQuartIdx - single.m_idxBegin)/2);
	    iTempEnd = single.m_idxEnd - Math.floor((single.m_idxEnd - single.m_iCurrQuartIdx)/2);
		if (0 > iTempBegin)
		{
			iTempBegin = 0;
		}
		if ((0 < single.m_vecQuartXLine.length) && (single.m_vecQuartXLine.length <= iTempEnd))
		{
		    iTempEnd = single.m_vecQuartXLine.length - 1;
		}
		if ((0 < single.m_vecIndex.length) && (single.m_vecIndex.length <= iTempEnd))
		{
		    iTempEnd = single.m_vecIndex.length - 1;
		}
		if (iTempEnd < iTempBegin)
		{
		    iTempBegin = single.m_idxBegin;
		    iTempEnd = single.m_idxEnd;
		}
	}

	if (ZoomType.sub == iScale) // 精度缩小
	{
	    iTempBegin = single.m_idxBegin - (single.m_iCurrQuartIdx - single.m_idxBegin);
	    iTempEnd = single.m_idxEnd + (single.m_idxEnd - single.m_iCurrQuartIdx);
		if (0 > iTempBegin)
		{
			iTempBegin = 0;
		}
		if ((0 < single.m_vecQuartXLine.length) && (single.m_vecQuartXLine.length <= iTempEnd))
		{
		    iTempEnd = single.m_vecQuartXLine.length - 1;
		}
		if ((0 < single.m_vecIndex.length) && (single.m_vecIndex.length <= iTempEnd))
		{
		    iTempEnd = single.m_vecIndex.length - 1;
		}
		if (iTempEnd < iTempBegin)
		{
		    iTempBegin = single.m_idxBegin;
		    iTempEnd = single.m_idxEnd;
		}
	}

	if (ZoomType.left == iScale) // 焦点左移
	{

	}

	if (ZoomType.right == iScale) // 焦点右移
	{

	}

	single.m_idxBegin = iTempBegin;
	single.m_idxEnd = iTempEnd;

	if (false == single.GetShowMValue()){  // 得到显示区段最值
		console.log("Call GetShowMValue.");
		return false;
	}

	if (false == single.show()) {
	    console.log("call single.show() error.");
	    return false;
	}

	return true;
}

single.GetShowMValue = function(){
	var iMax = 0;
	var iMin = 0;
	var iTemp = 0;
	var bInit = false;
    /*
    	    var objMVal = new CMValue;
	    objMVal.m_iMax = single.m_iShowMax;
	    objMVal.m_iMin = single.m_iShowMin;
    */
	single.m_iShowMax = single.m_iShowMin = 0;
	var ptrVec;
	for (var iter in single.m_mapQuartYLine)
	{
		iMax = iMin = 0;
		bInit = false;
		ptrVec = single.m_mapQuartYLine[iter].items;
		for (var i = single.m_idxBegin; (i < single.m_idxEnd) && (i < ptrVec.length); i++)
		{
			iTemp = ptrVec[i];
			if ((single.m_mapQuartYLine[iter].max < iTemp) || (single.m_mapQuartYLine[iter].min > iTemp))
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
		single.m_mapShowMValue[iter] = objMVal;

		if (single.m_iShowMax == single.m_iShowMin)
		{
		    single.m_iShowMax = objMVal.m_iMax;
		    single.m_iShowMin = objMVal.m_iMin;
		}
		single.m_iShowMax = objMVal.m_iMax > single.m_iShowMax ? objMVal.m_iMax : single.m_iShowMax;
		single.m_iShowMin = objMVal.m_iMin < single.m_iShowMin ? objMVal.m_iMin : single.m_iShowMin;
	}

	for (var iter in single.m_mapDayYLine)
	{
		iMax = iMin = 0;
		bInit = false;
		ptrVec = single.m_mapDayYLine[iter].items;
		var index1 = single.m_idxBegin > 0 ? single.m_idxBegin - 1 : single.m_idxBegin;
		var index2 = single.m_idxEnd > 0 ? single.m_idxEnd - 1 : single.m_idxEnd;
		while ((single.m_vecIndex.length > index1) && (-1 == single.m_vecIndex[index1])) index1++;
		while ((0 < index2) && (-1 == single.m_vecIndex[index2])) index2--;
		console.debug("index1: " + index1 + ", index2: " + index2 + ", m_vecIndex[index1]: " + single.m_vecIndex[index1]
		    + ", m_vecIndex[index2]: " + single.m_vecIndex[index2]);
		for (var i = single.m_vecIndex[index1]; (i < single.m_vecIndex[index2]) && (i < ptrVec.length); i++)
		{
			iTemp = ptrVec[i];
			if ((single.m_mapDayYLine[iter].m_iMax < iTemp) || (single.m_mapDayYLine[iter].m_iMin > iTemp))
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
		single.m_mapShowMValue[iter] = objMVal;
		console.debug("type: " + iter + ", iMax: " + iMax + ", iMin: " + iMin);

		if (single.m_iShowMax == single.m_iShowMin)
		{
		    single.m_iShowMax = objMVal.m_iMax;
		    single.m_iShowMin = objMVal.m_iMin;
		}
		single.m_iShowMax = objMVal.m_iMax > single.m_iShowMax ? objMVal.m_iMax : single.m_iShowMax;
		single.m_iShowMin = objMVal.m_iMin < single.m_iShowMin ? objMVal.m_iMin : single.m_iShowMin;
	}

	if (true == single.m_conf.absolute)
	{
	    for (var iterMVal in single.m_mapShowMValue)
		{
	        single.m_mapShowMValue[iterMVal].m_iMax = single.m_iShowMax;
	        single.m_mapShowMValue[iterMVal].m_iMin = single.m_iShowMin;
		}
	}

	if (false == single.AdjustMValue())
	{
		console.error("Call AdjustMValue error");
		return false;
	}

	return true;
}

single.AdjustMValue = function () {
	var iTimes = 0;
	var iTemp = 0;
	if (true == single.m_mapShowMValue.hasOwnProperty("DayPR"))
	{
	    iTemp = (single.m_mapShowMValue["DayPR"].m_iMin == 0) ? 0 : 1.0 * single.m_mapShowMValue["DayPR"].m_iMax / single.m_mapShowMValue["DayPR"].m_iMin;
		console.debug("AdjustYVal DayPR: " + iTemp);
		if ((iTemp > 100) || (iTemp <= 0))
		{
			return true;
		}
		iTimes = iTimes > iTemp ? iTimes : iTemp;
	}
	if (true == single.m_mapShowMValue.hasOwnProperty("DayReinPrice"))
	{
	    iTemp = (single.m_mapShowMValue["DayReinPrice"].m_iMin == 0) ? 0 : 1.0 * single.m_mapShowMValue["DayReinPrice"].m_iMax / single.m_mapShowMValue["DayReinPrice"].m_iMin;
		console.debug("AdjustYVal DayReinPrice: " + iTemp);
		if ((iTemp > 100) || (iTemp <= 0))
		{
			return true;
		}
		iTimes = iTimes > iTemp ? iTimes : iTemp;
	}
	if (true == single.m_mapShowMValue.hasOwnProperty("DayCovery"))
	{
	    iTemp = (single.m_mapShowMValue["DayCovery"].m_iMin == 0) ? 0 : 1.0 * single.m_mapShowMValue["DayCovery"].m_iMax / single.m_mapShowMValue["DayCovery"].m_iMin;
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

	if (true == single.m_mapShowMValue.hasOwnProperty("DayPR"))
	{
	    iTemp = 1.0 * single.m_mapShowMValue["DayPR"].m_iMax / single.m_mapShowMValue["DayPR"].m_iMin;
	    single.m_mapShowMValue["DayPR"].m_iMax = single.m_mapShowMValue["DayPR"].m_iMax * iTimes / iTemp;
	}
	if (true == single.m_mapShowMValue.hasOwnProperty("DayReinPrice"))
	{
	    iTemp = 1.0 * single.m_mapShowMValue["DayReinPrice"].m_iMax / single.m_mapShowMValue["DayReinPrice"].m_iMin;
	    single.m_mapShowMValue["DayReinPrice"].m_iMax = single.m_mapShowMValue["DayReinPrice"].m_iMax * iTimes / iTemp;
	}
	if (true == single.m_mapShowMValue.hasOwnProperty("DayCovery"))
	{
	    iTemp = 1.0 * single.m_mapShowMValue["DayCovery"].m_iMax / single.m_mapShowMValue["DayCovery"].m_iMin;
	    single.m_mapShowMValue["DayCovery"].m_iMax = single.m_mapShowMValue["DayCovery"].m_iMax * iTimes / iTemp;
	}
	console.debug("execute AdjustYVal");

	return true;
}
