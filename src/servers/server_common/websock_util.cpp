#include "sha1.hpp"
#include "websock_util.hpp"

void getAcceptKey(const char *in_key, char *buff, int buff_size){
  SHA1Context sha;
  uint8_t Message_Digest[20];
  int err;
  buff[0] = '\0';

  char key[1000];
  if(strlen(in_key) > 900){
    return;
  }
  sprintf(key, "%s%s", in_key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

  err = SHA1Reset(&sha);
  if (err){ printf("Error: getAcceptKey() SHA1Reset()\n"); return; }

  err = SHA1Input(&sha, (const unsigned char *) key, strlen(key));
  if (err){ printf("Error: getAcceptKey() SHA1Input()\n"); return; }

  err = SHA1Result(&sha, Message_Digest);
  if (err){ printf("Error: getAcceptKey() SHA1Result()\n"); return; }

  int i=0;
  int x=0;
  int l=0;
  const char *w = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned char c;
  for(int a=0; a<20; a++){
    c = Message_Digest[a];
    x = x << 8 | c;
    for(l += 8; l >= 6; l -= 6){
      buff[i++] = w[(x >> (l -6)) & 0x3f];
      if(i > buff_size-10){
	buff[0] = '\0';
	return;
      }
    }
  }
  if(l>0){
    x <<= 6 -l;
    buff[i++] = w[x & 0x3f];
  }
  for(; i%4;){
    buff[i++] = '=';
  }
  buff[i] = '\0';
}
