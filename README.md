![header](https://capsule-render.vercel.app/api?type=waving&color=gradient&height=280&section=header&text=IOCP%20Chatting%20Server&fontSize=70&fontColor=ffffff&fontAlign=50&fontAlignY=45)

## 🎮 IOCP를 활용한 채팅 서버 포트폴리오

## 🔎 프로필
- 이름 `IOCP를 활용한 채팅 서버`
- 기간 `2024-06 ~ 2024-07`
- 인원 `1명`
- 역할 `서버`
- 플랫폼 `PC`
<br>

## ⚙️ 개발 환경
- 언어 `C++`
- 통합개발환경 `Visual Studio 2022`
- 형상관리 `Git`
- 운영체제 `Windows 11`
<br>

## 💻 목적
- `멀티 스레드`가 공유하는 자원에 적절하게 `잠금(lock)`을 걸어 상호 배제를 보장할 수 있습니다.
- OSI 7계층 중에서 전송 계층에 있는 `TCP와 UDP`의 성질을 이해합니다.
- `IOCP`를 활용해 클라이언트-서버 프레임워크를 구축하고 `패킷`을 설계해 데이터를 주고받습니다.
<br>

## 📜 구현 사항
- `IOCP`를 활용해 워커 스레드에서 소켓 함수(Bind, Listen, Accept, Connect, Send, Receive, Disconnect)를 블록하지 않고 호출하도록 구현합니다.
- `클라이언트 세션`과 `서버 세션`에서 발생한 이벤트를 분석해 그 타입에 알맞은 작업을 `IOCP 큐`에 등록 후 처리(Register and Process)하도록 일관적으로 관리합니다.
- `투 포인터` 알고리즘으로 수신 버퍼, 발신 버퍼에 데이터를 읽고 쓸 수 있습니다.
- `구글 프로토버프`로 패킷을 설계하고 `헤더`에 있는 메타 데이터를 읽어 패킷을 분석할 수 있습니다.
<br>

## ⓒ 2024. Tak Junseok all rights reserved.
이 리포지토리에 기재된 코드와 리포트에 대한 저작권은 탁준석에게 있습니다. 사전에 합의되지 않은 내용을 무단으로 도용(URL 연결 등), 불법으로 복사(캡처)하여 사용할 경우 사전 경고 없이 저작권법에 의한 처벌을 받을 수 있습니다.

The copyright for the codes and reports listed in this repository belongs to Tak Junseok. If you steal(such as URL connection) or illegally copy(capture) anything that is not agreed in advance, you may be punished by copyright law without prior warning.
