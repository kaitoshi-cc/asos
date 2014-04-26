#include "SHA-1.hpp"

//巡回シフト
#define CircularShift(bits,word) (((word)<<(bits)) | ((word)>>(32-bits)))

//fを計算
unsigned int f(int t,unsigned int B,unsigned int C,unsigned int D)
{
	if(t<20)return (B & C) | ((~ B) & D);
	else if(t<40)return B ^ C ^ D;
	else if(t<60)return (B & C) | (B & D) | (C & D);
	else return B ^ C ^ D;
}

//Kを取得
unsigned int K(int t)
{
	if(t<20)return 0x5A827999;
	else if(t<40)return 0x6ED9EBA1;
	else if(t<60)return 0x8F1BBCDC;
	else return 0xCA62C1D6;
}

/*核心部。ブロックからSHA-1を計算します*/
void SHA1ProcessBlock(unsigned int *W,unsigned int *H)
{
	unsigned int A,B,C,D,E;
	unsigned int TEMP;
	int t;

	for(t=16;t<80;t++)
	{
		W[t] = CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
	}
	A = H[0];
	B = H[1];
	C = H[2];
	D = H[3];
	E = H[4];
	for(t=0;t<80;t++)
	{
		TEMP = CircularShift(5,A) + f(t,B,C,D) + E + W[t] + K(t);
		E = D; D = C; C = CircularShift(30,B); B = A; A = TEMP;
	}
	H[0] += A;
	H[1] += B;
	H[2] += C;
	H[3] += D;
	H[4] += E;
}//特にコメントありません。すみません。

/*外部に公開される関数です。何故かここだけハンガリアン記法。*/
void SHA1(const char *pInput,unsigned int wLength,unsigned int *pwResult)
{
	unsigned char aLastBlock[64] = {0}; //半端ブロック用(手抜き)
	int cnBlock,nLastBlockSize,nIndex,i,t;
	unsigned int awBlock[80];

	/*マジックナンバーです。深く考えないほうがいいです。*/
	pwResult[0] = 0x67452301;
	pwResult[1] = 0xEFCDAB89;
	pwResult[2] = 0x98BADCFE;
	pwResult[3] = 0x10325476;
	pwResult[4] = 0xC3D2E1F0;

	cnBlock= wLength/64; //ブロック数
	nLastBlockSize = wLength-64*cnBlock; //半端な原文のサイズ

	/*最後のブロックの一つ手前まで処理*/
	for(i=0;i<cnBlock;i++)
	{
		for(t=0;t<16;t++)
		{
			awBlock[t] = pInput[i*64+t*4]<<24;
			awBlock[t] |= pInput[i*64+t*4+1]<<16;
			awBlock[t] |= pInput[i*64+t*4+2]<<8;
			awBlock[t] |= pInput[i*64+t*4+3];
		}
		SHA1ProcessBlock(awBlock,pwResult);
	}

	/*最後のブロックの処理*/
	if(nLastBlockSize>55) //最後の64bitが確保出来ない時
	{
		memcpy((char*)aLastBlock,pInput+cnBlock*64,nLastBlockSize);
		aLastBlock[nLastBlockSize] = 0x80; //10000000h

		for(t=0;t<16;t++)
		{
			awBlock[t] = aLastBlock[t*4]<<24;
			awBlock[t] |= aLastBlock[t*4+1]<<16;
			awBlock[t] |= aLastBlock[t*4+2]<<8;
			awBlock[t] |= aLastBlock[t*4+3];
		}
		SHA1ProcessBlock(awBlock,pwResult);
		nIndex = 0;
	}
	else
	{
		memcpy((char*)aLastBlock,pInput+cnBlock*64,nLastBlockSize);
		nIndex = nLastBlockSize;
		aLastBlock[nIndex++] = 0x80; //10000000h
	}

	/*余りは0で埋める*/
	for(t=nIndex;t<60;t++)
	{
		aLastBlock[t] = 0;
	}

	/*サイズ情報。2^29まで*/
	aLastBlock[60] = wLength*8>>24;
	aLastBlock[61] = wLength*8>>16;
	aLastBlock[62] = wLength*8>>8;
	aLastBlock[63] = wLength*8;

	/*ブロックに移す*/
	for(t=0;t<16;t++)
	{
		awBlock[t] = aLastBlock[t*4]<<24;
		awBlock[t] |= aLastBlock[t*4+1]<<16;
		awBlock[t] |= aLastBlock[t*4+2]<<8;
		awBlock[t] |= aLastBlock[t*4+3];
	}
	SHA1ProcessBlock(awBlock,pwResult);

	return;
}

void getAcceptKey(const char *in_key, char *buff, int buff_size){
  char key[1000];
  if(strlen(in_key) > 900){
    buff[0] = '\0';
    return;
  }
  sprintf(key, "%s%s", in_key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

  unsigned int Result[5];
  SHA1(key, strlen(key), Result);

  int i=0;
  int x=0;
  int l=0;
  const char *w = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned char c;
  for(int a=0; a<5; a++){
    for(int j=3; j>=0; j--){
      c = *((unsigned char*)(&Result[a])+j) ;
      x = x << 8 | c;
      for(l += 8; l >= 6; l -= 6){
	buff[i++] = w[(x >> (l -6)) & 0x3f];
	if(i > buff_size-10){
	  buff[0] = '\0';
	  return;
	}
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

