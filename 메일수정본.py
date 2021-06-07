# 1. while문 True로 계속 돌리기.
# 2. after clear 언제 해줄건지? 영원히 안해도될런지/?
# 3. VS 에서 Kinect랑 this py파일이랑 동시에 돌리는게 되는지?
# 4, 3번 문제없이 잘될때, 연동 잘 되는지 check ㅇㅋㅇ

# ★주의사항 , / vs에서 입력할때 무조건 \n있어야 함 / txt기본설정 \n하나 필수
# 중간에 txt바뀌는건 ㄱㄴ 바뀌어도 10초(vs) 안에는 while문이 돌기때문에
# 바뀌고 메일전송 되는거 확인함



after = [] # 보낸 후


import smtplib # 메일을 보내기 위한 라이브러리 모듈
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication
from time import sleep



sendEmail = "" #보내는 사람 ID : naver만 가능/ 이메일 설정 바꿔야됨
password = ""           #보내는 사람 PW                      
recvEmail = ""   # 받는 사람 email


smtpName = "smtp.naver.com" #smtp 서버 주소
smtpPort = 587 #내 ID의 smtp 포트 번호



while True:
    before = [] # 보내기 전,
    f = open("emergence.txt", 'r')
    lines = f.readlines()
    for line in lines:#list에 동영상 이름 넣기(.avi까지)
        line = line.rstrip('\n') # \n문자 제거
        line = line+'.avi'
        before.append(line)

    if before[0] != '.avi': # 아무것도 없으면 즉, kinect 시작하고 응급감지 한번도 안될때
        for i in range(len(before)):
            if before[i] not in after:
                msg = MIMEMultipart()

                msg['Subject'] ="<긴급> 혼자라고 생각말기 응급알림입니다."
                msg['From'] = sendEmail
                msg['To'] = recvEmail
        
                print(before[i])
                etcFileName = before[i]
                with open(etcFileName, 'rb') as etcFD : 
                    etcPart = MIMEApplication( etcFD.read() )
                    #첨부파일의 정보를 헤더로 추가
                    etcPart.add_header('Content-Disposition','attachment', filename=etcFileName)
                    msg.attach(etcPart)
                    
                after.append(before[i])
                    
                    
                s=smtplib.SMTP( smtpName , smtpPort ) #메일 서버 연결
                s.starttls() #TLS 보안 처리
                s.login( sendEmail , password ) #로그인
                s.sendmail( sendEmail, recvEmail, msg.as_string() ) #메일 전송, 문자열로 변환하여 보냅니다.
                s.close() #smtp 서버 연결을 종료합니다
                
            else:
                print(f'{before}은 이미 보낸 파일입니다.')
                break # 이미 보냈다면 더이상 after 둘러볼 필요x

            
    

    print(f'보내기 전 : {before}')
    print(f'보낸 후 : {after}')
    f.close()
    sleep(10)
    
    if len(after) > 100:
        after = []




        
