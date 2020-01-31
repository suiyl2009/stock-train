from __future__ import division
import pymysql
import struct 
from urllib import parse,request
import json
from copy import deepcopy

# 连接MySQL数据库
connection = pymysql.connect(host='127.0.0.1', port=3306, user='root', passwd='123456', db='stock',
charset='utf8mb4', cursorclass=pymysql.cursors.DictCursor)

class CBuyPoint(object):
    def __init__(self):
        self.code = ""
        self.point = []
        self.beginDate = 0
        self.endDate = 0
        self.currPr = 0
        self.currDate = 0
        self.ratio = 0.0

mapCodes = {}
try:
  # 通过cursor创建游标
  cursor = connection.cursor()
  # 创建sql 语句
  sql = "select * from tb_trade_point2"
  # 执行sql语句
  cursor.execute(sql)
  # 获取所有记录列表
  results = cursor.fetchall()
#  print(results)   
  for data in results:  # 打印结果      
    #print(data)
    #print(data['point'])
    #print(data['point'][0:4])
    #print(struct.unpack('i', data['point'][0:4]))
    #print(data)
    point = CBuyPoint()
    point.code = data['code']
    point.point = json.loads(data['point'])
    point.beginDate = data['beginDate']
    point.endDate = data['endDate']
    mapCodes[point.code] = point
    #print(data)
    #print(point.code)
except:
  print("查询失败")

cursor.close() #关闭游标连接
connection.close() # 关闭数据库连接

for key in mapCodes.keys():
    textmod = {"method": "single.line", "code": key, "item": ["DayPR"], "before": -1}
    textmod = json.dumps(textmod).encode(encoding='utf-8')

    #print(textmod)
    header_dict = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko',"Content-Type": "application/json"}
    url='http://192.168.72.128/Stock'
    req = request.Request(url=url,data=textmod,headers=header_dict)
    res = request.urlopen(req)
    res = res.read()
    #print(res)
    #print(res.decode(encoding='utf-8'))
    jsonres = json.loads(res.decode(encoding='utf-8'))
    #print(jsonres)
    dyline = jsonres['result']['dyline'][0]['items']
    dxline = jsonres["result"]['dxline']
    if 0 < len(dxline):
        mapCodes[key].currDate = dxline[0]
        mapCodes[key].currPr = dyline[0]
        if 0 < len(mapCodes[key].point):
            lastItem = mapCodes[key].point[-1]
            mapCodes[key].ratio = mapCodes[key].currPr / lastItem['DayPR']
    print(mapCodes[key].__dict__)