import os
import time
import smtplib

from send_mail import send_mail
from send_mail import maincont

cmd_ps = "ps aux | grep {user} | grep ldpc | grep -v sh | grep -v grep | awk '{{print $24, $22, $10, $19, $20}}'"
cmd_test = "ps aux | grep {user} | grep ldpc | grep -v sh | grep -v grep | awk '{{print $24, $22, $10, $19, $20}}'"
cmd_out = "cat {out} | grep OK | tail -n 8 | awk '{{print $2, $3, $4, $8, $9}}'"

template_log = """
base matrix of code: {target}
current time used: {timeused}
simulation condition: {sim_cond}
current simulation status:
{cur_log}
"""

def parse_ps(user):
    """
    处理命令
    """
    # print(cmd_ps)
    psout = os.popen(cmd_ps.format(user=user)).read().strip()
    # print(psout)
    # pslist = psout.split('\n')
    # print(pslist)
    # target = "x".join(pslist[1].split())
    # return target
    for a_code in psout.split('\n'):
        yield a_code


def parse_out_log(target):
    """处理日志

    Args:
        target (str): 目标日志文件

    Returns:
        str: 日志文件输
    """
    base_out_file = "../out{axb}.log"
    cur_out_file = base_out_file.format(axb=target)
    cat_out = os.popen(cmd_out.format(out=cur_out_file)).read()
    # print(cat_out)
    return cat_out

def parse_all():
    """处理所有的日志

    Returns:
        str: 提取的日志信息
    """
    outmain = ""
    parsed_data = {}
    for cc in parse_ps("kolinahr"):
        # assert(lst_str.__len__() == 5)
        lst_str = cc.split()
        parsed_data['target'] = "x".join(lst_str[0:2])
        parsed_data['timeused'] = lst_str[2]
        parsed_data['sim_cond'] = " ".join(lst_str[3:5])
        parsed_data['cur_log'] = parse_out_log(parsed_data['target'])
        outmain = outmain + template_log.format(**parsed_data)
    return outmain

def test_parse():
    """测试文件处理
    """
    data = parse_all()
    date = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    mail_content = maincont.format(date=date, data=data.replace("\n", "<br>"))
    print(mail_content)

def main():
    """主函数
    """
    while True:
        t = time.localtime()
        if (t.tm_hour == 17 and t.tm_min == 7):
            data = parse_all()
            date = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
            mail_content = maincont.format(date=date, data=data.replace("\n", "<br>"))
            send_mail(mail_content)
            time.sleep(3600)
        else:
            time.sleep(40)


if __name__ == '__main__':
    main()

