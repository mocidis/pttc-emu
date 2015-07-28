# pttc-emu
pttc-emu giả lập thiết bị ptt card sử dung pseudo-terminal

* Cách build:

    Chạy make. Kết quả sinh ra là file pttc-emu

* Cách chạy:

    Chạy pttc-emu. Chương trình sẽ in ra device file tương ứng cho thiết bị ptt card giả lập. Ví dụ: /dev/ttys001

    Sử dụng file thiết bị này để kết nối tới thiết bị và test chức năng điều khiển pttc

#PROTOCOL
============================
Hearder 5x2 (J8):

Pin 10 -> 5VDC

Pin 1  -> GND

Pin 2  -> Reset (not connect)

Pin 3 -> TX

Pin 5 -> RX
-----------------------------
Khi khoi dong xong      : "ready" is sent to PC via UART 

Nhan giu PTT (Press) (PTT ON)   : "L1" is sent to PC via UART

Nha PTT  (PTT OFF)      : "L0" is sent to PC via UART

PC --gửi dữ liệu--> PTTC: PTTCard gửi "L0" hoặc "L1" tùy vào trạng thái PTT của PTTCard lúc đó. 
