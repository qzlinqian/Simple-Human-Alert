import serial
import email_sending

ser = serial.Serial('/dev/cu.HC-05-DevB', 115200, timeout=1)
head = b'\xff'
conf = b'\xfe'
tail = b'\xfe'
pwd = "qianlin"
sudopwd = "sudoqianlin"
ok = "RIGHT"
clc = "CLEAR"
alert = "ALERT"

flag = 0
tried_times = 0
num = 0

while 1:
    s = ser.read()
    if s == head:
        s = ser.read()
        if s == conf:
            s = ser.read()
            if s == head:
                s = ser.read()
                num = int.from_bytes(s, 'big')
                print("It's the No.%d time." % num)
                flag = 1
            if s == conf:
                flag = 2
                result = email_sending.mail(num, tried_times)
                if result:
                    print("Email send successfully.")
                else:
                    print("Cannot send email temporarily.")
    if flag > 0:
        pwdInput = input("Password: ")
        if (pwdInput == pwd) & (flag == 1):
            flag = 0
            tried_times = 0
            c = bytearray(ok, 'UTF-8')
            ser.write(c)
            ser.flushOutput()
        else:
            if pwdInput == sudopwd:
                flag = 0
                tried_times = 0
                c = bytearray(clc, 'UTF-8')
                ser.write(c)
                ser.flushInput()
            else:
                tried_times += 1
                if tried_times > 2:
                    c = bytearray(alert, 'UTF-8')
                    ser.write(c)
                    ser.flushOutput()
                    result = email_sending.mail(num, tried_times)
                    if result:
                        print("Email send successfully.")
                    else:
                        print("Cannot send email temporarily.")
