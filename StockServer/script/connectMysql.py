#import pymysql.cursor
import pymysql
import struct 

# 连接MySQL数据库

connection = pymysql.connect(host='127.0.0.1', port=3306, user='root', passwd='123456', db='stock',

charset='utf8mb4', cursorclass=pymysql.cursors.DictCursor)

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
    print(data)
    idx = 0
    while idx < len(data['point']):
        print(struct.unpack('ii', data['point'][0:8]))
        idx += 8

except:

  print("查询失败")

cursor.close() #关闭游标连接

connection.close() # 关闭数据库连接