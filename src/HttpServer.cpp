#include "HttpServer.h"
#include <algorithm>
#include "Utils.h"

HttpServer::HttpServer()
    : srv(Server()) {
}

void HttpServer::Listen(ListenOptions opt, function<void(HttpServer&)> cb) {
  return this->srv.Listen(opt, [=](Server& srv) -> void {
    cb(*this);
    srv.OnAccpet([=](Server& srv, Socket& socket) -> void {
      return this->_acceptHandler(srv, socket);
    });
  });
}

void HttpServer::Close() {
  return this->Close();
}

void HttpServer::Join() {
  return this->Join();
}

void HttpServer::OnReq(function<void(HttpReq&, HttpRes&)> cb) {
  this->reqCb.push_back(cb);
}

void HttpServer::_acceptHandler(Server&, Socket& socket) {
  auto totalLen = new u_long(0);
  auto recvLen = new u_long(0);
  auto bodyBuffers = new vector<pair<char*, u_long>>();
  auto headStr = new string("");
  auto headEnded = new bool(false);
  auto firstChunk = new bool(true);
  auto ended = new bool(false);
  auto httpReq = new HttpReq();
  auto httpRes = new HttpRes();
  socket.OnRecv([=](Socket& socket, WSABUF buf, u_long len) -> void {
    if (*ended) {
      return;
    }

    if (*headEnded) {
      char* copy = new char[len];
      memcpy(copy, buf.buf, len);
      bodyBuffers->push_back({copy, len});
      *recvLen += len;
      for (auto cb : httpReq->dataCb) {
        cb(copy, len);
      }
      if (*totalLen == *recvLen && *recvLen != 0) {
        for (auto cb : httpReq->endCb)
          cb();

        *ended = true;
        return;
      }
      return;
    }
    string chunk(buf.buf);
    if (*firstChunk) {
      // first chunk,
      *firstChunk = false;
      size_t pos = chunk.find("\r\n");
      if (pos == string::npos) {
        socket.End();  // not http
        *ended = true;
        return;
      }
    }

    size_t pos = chunk.find("\r\n\r\n");

    if (pos != string::npos) {
      *headStr += chunk.substr(0, pos);
      *headEnded = true;
      // parse header
      auto headerLines = split(*headStr, "\r\n");
      int i = 0;
      for (auto line : headerLines) {
        if (i++ != 0) {
          auto h = split(line, ": ");
          transform(h[0].begin(),
                    h[0].end(),
                    h[0].begin(),
                    [](char in) -> char {
                      if (in <= 'Z' && in >= 'A')
                        return in - ('Z' - 'z');
                      return in;
                    });
          httpReq->headers[h[0]] = h[1];
        }
      }

      auto firstLine = split(headerLines[0], " ");
      httpReq->method = firstLine[0];
      httpReq->path = firstLine[1];
      for (auto cb : this->reqCb) {
        cb(*httpReq, *httpRes);
      }

      if (httpReq->headers.find("content-length") == httpReq->headers.end()) {
        for (auto cb : httpReq->endCb)
          cb();

        *ended = true;
        return;
      } else {
        // found
        *totalLen = stoi(httpReq->headers["content-length"]);
      }

      size_t headLen = pos + 4;
      if (len > headLen) {
        char* copy = new char[len - headLen];
        memcpy(copy, buf.buf + headLen, len - headLen);
        bodyBuffers->push_back({copy, len - headLen});
        *recvLen += len - headLen;
        for (auto cb : httpReq->dataCb) {
          cb(copy, len - headLen);
        }
        if (*totalLen == *recvLen && *recvLen != 0) {
          for (auto cb : httpReq->endCb)
            cb();

          *ended = true;
          return;
        }
      }
    } else
      *headStr += chunk;
  });

  socket.OnClose([=](Socket&) -> void {
    printf("Closed!\n");
    delete totalLen;
  });
}

void HttpReq::OnData(function<void(char*, u_long)> cb) {
  this->dataCb.push_back(cb);
}

void HttpReq::OnEnd(function<void(void)> cb) {
  this->endCb.push_back(cb);
}