POST /post_body HTTP/1.1
Host: localhost
Content-length: 3

123
POST /post_body HTTP/1.1
Host: localhost
transFer-encOdiNg: chunked

2

DELETE / HTTP/1.1
Host: localhost
Content-length: 5

12345

POST /post_body HTTP/1.1
Host: localhost
Content-length: 1
transFer-encOdiNg: chunked

12345

DELETE /delete_test/ HTTP/1.1
Host: localhost
transfer-encoding: chunked
Content-length: 3

5
---
GET / HTTP/1.1
Host: localhost
transfer-encoding: chunked

2
yy
3
aaa
0

GET / HTTP/1.1
Host: lol

---
GET / HTTP/1.1
Host: localhost
transfer-encoding: chunked

5
yy
