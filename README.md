# C++ HTTP/2

Self-implemented HTTP/2 based on winsock2 and IOCP.

## Todo

- [x] IOCP Model
    - [x] Accept 
    - [x] Recv
    - [x] Send
    - [x] Close

- [ ] Server & Socket interface
    - [x] Server start
    - [x] Server close
    - [x] Server onAccept
    - [x] Server onClose
    - [x] Socket onRecv
    - [x] Socket onClose
    - [x] Socket Write
    - [x] Socket End
    - [ ] Socket Close


- [x] HTTP interface
    - [x] HttpReq
        - [x] HttpReq::headers, HttpReq::method, HttpReq::path
        - [x] HttpReq::clientAddr, other client infos
        - [x] HttpReq::OnData
        - [x] HttpReq::OnEnd
    - [x] HttpRes
        - [x] Status
        - [x] SetHeader
        - [x] Write

- [ ] Refactor
    - [ ] Make HttpReq, HttpRes and Scoket a class
        - [ ] HttpReq
        - [ ] HttpRes
        - [ ] Socket
    - [ ] privatefiy properties
        - [ ] HttpReq
        - [ ] HttpRes
        - [ ] Socket