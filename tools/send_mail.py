import os
import time
import smtplib
from email.mime.text import MIMEText
#设置服务器所需信息
#163邮箱服务器地址
mail_host = 'smtp.163.com'
#163用户名
mail_user = 'jiangxuanli00'
#密码(部分邮箱为授权码) 
mail_pass = 'HSEZMBOTFCNSSXRB'
#邮件发送方邮箱地址
sender = 'jiangxuanli00@163.com'
#邮件接受方邮箱地址，注意需要[]包裹，这意味着你可以写多个邮件地址群发
receivers = ['1311772940@qq.com', '21S005095@stu.hit.edu.cn']

#设置email信息
#邮件内容设置
# ifc = os.popen("make parse", 'r')
# ifc = os.popen("ifconfig", 'r')
# mail_content = ifc.read()
# message = MIMEText(mail_content,'plain','utf-8')

maincont = """
<!DOCTYPE html>
<html><head><title>Simulation Resut</title></head>
<body><h1>Simulation Result</h1><p>
<p>
时间：{date}<br>
仿真结果如下
</p>
<h2>数据</h2>
<p>
{data}
</p>
<h2>图</h2>
{svg}
</body>
</html>
"""

# read svg data
with open("../outsvg.svg", 'r') as f:
    svgpic = f.read()

# read raw data
with open("../outdata.dat", 'r') as f:
    data = "<br>".join(f.readlines())

# combine html
mail_content = maincont.format(date=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), data=data, svg=svgpic)

message = MIMEText(mail_content,'html','utf-8')

#邮件主题
message['Subject'] = '[auto] simulation result'
#发送方信息
message['From'] = sender 
#接受方信息
message['To'] = ";".join(receivers)

#登录并发送邮件
def send_mail():
    try:
        smtpObj = smtplib.SMTP() 
        #连接到服务器
        smtpObj.connect(mail_host,25)
        #登录到服务器
        smtpObj.login(mail_user,mail_pass) 
        #发送
        smtpObj.sendmail(
            sender,receivers,message.as_string()) 
        #退出
        smtpObj.quit() 
        print('success')
    except smtplib.SMTPException as e:
        print('error',e) #打印错误

if __name__ == "__main__":
    send_mail()
    # print("send following:\n\n")
    print(mail_content)
    # print(message.as_string()

