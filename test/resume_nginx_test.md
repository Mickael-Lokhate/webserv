## Test requests HTTP

Tester

- [x] GET 
- [x] POST
- [x] PUT
- [x] Upload file PUT
- [x] HEAD
- [x] CGI
- [x] Stress tests

Other test

- [x] Upload file POST
- [x] Session / cookies
- [x] test from web browser (chrome)
- [x] DELETE

## Fichier de configuration nginx

- [x] `listen`
- [x] `server_name`
- [x] `error_page`
- [x] `client_max_body_size`
- [x] `location`
- [x] `if ($request_method != GET ) { return 405; }`
- [x] `return`
- [x] `alias`
- [x] `root`
- [x] `autoindex`
- [x] `index`
- [x] `cgi`
- [x] `upload from PUT` 

## resume headers request

- Accept-Encoding: gzip
- Content-Type: test/file
- Host: localhost:8000
- Transfer-Encoding: chunked
- User-Agent: Go-http-client/1.1
- X-Secret-Header-For-Test: 1

## resume headers response

- Accept-Ranges: bytes
- Connection: close
- Connection: keep-alive
- Content-Length: 153
- Content-Type: application/octet-stream
- Content-Type: text/html
- Content-Type: text/html; charset=utf-8
- Date: Sat, 13 Nov 2021 10:33:45 GMT
- ETag: "618b84a5-37"
- Last-Modified: Wed, 10 Nov 2021 08:36:53 GMT
- Location: http://localhost/directory/nop/
- Server: nginx/1.20.1
- Transfer-Encoding: chunked
