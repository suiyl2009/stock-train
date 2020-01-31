import pymysql
import struct 
from urllib import parse,request
import json
from copy import deepcopy

# 连接MySQL数据库
connection = pymysql.connect(host='127.0.0.1', port=3306, user='root', passwd='123456', db='stock',
charset='utf8mb4', cursorclass=pymysql.cursors.DictCursor)

list = [] 
try:
  # 通过cursor创建游标
  cursor = connection.cursor()
  # 创建sql 语句
  sql = "select * from tb_trade_point"
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
    item = deepcopy(data)
    item['point'] = []
    idx = 0
    while idx < len(data['point']):
        #print(struct.unpack('ii', data['point'][idx:8+idx]))
        point = {}
        point['type'] = struct.unpack('i', data['point'][idx:4+idx])[0]
        point['date'] = struct.unpack('i', data['point'][4+idx:8+idx])[0]
        #print(point)
        item['point'].append(point)
        idx += 8
    #print(item)
    list.append(item)
except:
  print("查询失败")

cursor.close() #关闭游标连接
connection.close() # 关闭数据库连接

#print(list)

for data in list:
    textmod = {"method": "single.line", "code": data['code'], "item": ["DayPR"], "before": -1}
    textmod = json.dumps(textmod).encode(encoding='utf-8')

    print(textmod)
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
    for point in data['point']:
        point['DayPR'] = dyline[dxline.index(point['date'])]

print(list)

connection = pymysql.connect(host='127.0.0.1', port=3306, user='root', passwd='123456', db='stock',
charset='utf8mb4', cursorclass=pymysql.cursors.DictCursor)
try:
  # 通过cursor创建游标
  cursor = connection.cursor()
  for data in list:
    data['point'] = sorted(data['point'], key=lambda x: x['date'])
    temp = []
    for i in data['point']:
        if i not in temp:
            temp.append(i)
    data['point'] = temp
    # 创建sql 语句
    sql = "insert into tb_trade_point2(userID, code, point, beginDate, endDate) values(%d, '%s', '%s', %d, %d)"%(data['userID'],data['code'],json.dumps(data['point']),data['beginDate'],data['endDate'])
    print(sql)
    # 执行sql语句
    cursor.execute(sql)
except:
  print("插入失败")

cursor.close() #关闭游标连接
connection.close() # 关闭数据库连接

'''
textmod = {"method": "single.line", "code": "000001", "item": ["DayPR"], "before": 2}
textmod = json.dumps(textmod).encode(encoding='utf-8')

print(textmod)
header_dict = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko',"Content-Type": "application/json"}
url='http://192.168.72.128/Stock'
req = request.Request(url=url,data=textmod,headers=header_dict)
res = request.urlopen(req)
res = res.read()
print(res)
print(res.decode(encoding='utf-8'))
#输出内容:{"jsonrpc":"2.0","result":"37d991fd583e91a0cfae6142d8d59d7e","id":1}
'''