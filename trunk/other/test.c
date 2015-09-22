#include <stdio.h>
#include <string.h>
#include "stringx.h"
#include "urlparser.h"
#include "http-client-c.h"

#define TEST_REQ "GET /index.html HTTP/1.1\r\n" \
                 "Host: 127.0.0.1:80\r\n" \
                 "Connection: keep-alive\r\n\r\n"

int main()
{
  struct parsed_url *purl = parse_url("http://127.0.0.1/");
  struct http_response *hrep = http_req(TEST_REQ, purl);
  char buf[1024];
  memset((void *)buf, 0, sizeof(buf));
  if ( hrep )
    printf("%s\n", hrep->body);
  return 0;
}