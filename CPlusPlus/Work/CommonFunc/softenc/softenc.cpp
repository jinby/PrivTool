#include "stdafx.h"
#include "assert.h"
#include "softenc.h"
#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"
#include "time.h"
#include "des.h"
#include "md5.h"
#include "sha1.h"

#define TRUE	1
#define FALSE	0

NAMESPACE_BEGIN(SOFTENC)
//////////////////////////////////////////////////////////////////////
//	class CRandomGenerator

void CRandomGenerator::Seed()
{
	srand(time(NULL));
}

void CRandomGenerator::GetBlock(unsigned char* outString, unsigned long length)
{
	if(outString == NULL)
		return;
	
	for(unsigned long l = 0; l < length; l++)
		outString[l] = (unsigned char)rand();
}

unsigned short CRandomGenerator::GetShort(unsigned short wMin, unsigned short wMax)
{
	return (unsigned short)(rand()*1.0/(RAND_MAX)*wMax + wMin);
}

/////////////////////////////////////////////////////////////////////
//	class CByteQueue

CByteQueue::CByteQueue()
{
	m_pbBuffer = NULL;
	m_cbSize = 0;
}

CByteQueue::~CByteQueue()
{
	if(m_pbBuffer != NULL){
		free(m_pbBuffer);
		m_pbBuffer = NULL;
	}
	m_cbSize = 0;
}

void CByteQueue::Reverse()
{
	ByteReverse(m_pbBuffer, m_cbSize);
}

void CByteQueue::Put(const unsigned char* inString, unsigned long length)
{
	unsigned char* p = (unsigned char*)realloc(m_pbBuffer, m_cbSize + length);
	if(p != NULL){
		memcpy(p + m_cbSize, inString, length);
		m_cbSize += length;
	}

	m_pbBuffer = p;
}

unsigned long CByteQueue::Get(unsigned char* outString, unsigned long getMax) const
{
	if(m_pbBuffer == NULL || outString == NULL)
		return 0;

	unsigned long getLen = getMax;
	if(getLen > m_cbSize)
		getLen = m_cbSize;

	memcpy(outString, m_pbBuffer, getLen);

	return getLen;
}

CByteQueue& CByteQueue::operator=(const CByteQueue& t)
{
	if(this != &t){
		Put(t.GetData(), t.CurrentSize());
	}

	return *this;
}

/////////////////////////////////////////////////////////////////////
//	class Integer

Integer::Integer(unsigned long t)
{
	unsigned char b[4];
	BigEndian_DWord2ByteStream(t, b, sizeof(b));
	m_byteQueue.Put(b, sizeof(b));
}

Integer::Integer(const Integer& t)
{
	m_byteQueue = t.m_byteQueue;
}

Integer& Integer::operator=(const Integer& t)
{
	if(this != &t)
		m_byteQueue = t.m_byteQueue;

	return *this;
}

long Integer::ConvertToLong() const
{
	unsigned char b[4];
	unsigned long lSize = m_byteQueue.Get(b, sizeof(b));
	unsigned long lResult;
	BigEndian_ByteStream2DWord(b, lSize, lResult);

	return (long)lResult;
}

//////////////////////////////////////////////////////////////////////
//	class PKCS_EncryptionPaddingScheme

unsigned long PKCS_EncryptionPaddingScheme::MaxUnpaddedLength(unsigned long paddedLength) const
{
	return paddedLength/8 > 10 ? paddedLength/8-10 : 0;
}

void PKCS_EncryptionPaddingScheme::Pad(CRandomGenerator &rng, const unsigned char *input, unsigned long inputLen, unsigned char *pkcsBlock, unsigned long pkcsBlockLen) const
{
	assert (inputLen <= MaxUnpaddedLength(pkcsBlockLen));

	// convert from bit length to unsigned char length
	if (pkcsBlockLen % 8 != 0)
	{
		pkcsBlock[0] = 0;
		pkcsBlock++;
	}
	pkcsBlockLen /= 8;

	pkcsBlock[0] = 2;  // block type 2

	// pad with non-zero random bytes
	for (unsigned long i = 1; i < pkcsBlockLen-inputLen-1; i++)
		pkcsBlock[i] = (unsigned char)rng.GetShort(1, 0xff);

	pkcsBlock[pkcsBlockLen-inputLen-1] = 0;     // separator
	memcpy(pkcsBlock+pkcsBlockLen-inputLen, input, inputLen);
}

unsigned long PKCS_EncryptionPaddingScheme::Unpad(const unsigned char *pkcsBlock, unsigned long pkcsBlockLen, unsigned char *output) const
{
	unsigned long maxOutputLen = MaxUnpaddedLength(pkcsBlockLen);

	// convert from bit length to unsigned char length
	if (pkcsBlockLen % 8 != 0)
	{
		if (pkcsBlock[0] != 0)
			return 0;
		pkcsBlock++;
	}
	pkcsBlockLen /= 8;

	// Require block type 2.
	if (pkcsBlock[0] != 2)
		return 0;

	// skip past the padding until we find the seperator
	unsigned long i=1;
	while (i<pkcsBlockLen && pkcsBlock[i++]) { // null body
		}
	assert(i==pkcsBlockLen || pkcsBlock[i-1]==0);

	unsigned long outputLen = pkcsBlockLen - i;
	if (outputLen > maxOutputLen)
		return 0;

	memcpy (output, pkcsBlock+i, outputLen);
	return outputLen;
}

//////////////////////////////////////////////////////////////////////
//	class PKCS_SignaturePaddingScheme

unsigned long PKCS_SignaturePaddingScheme::MaxUnpaddedLength(unsigned long paddedLength) const
{
	return paddedLength/8 > 10 ? paddedLength/8-10 : 0;
}

void PKCS_SignaturePaddingScheme::Pad(CRandomGenerator &, const unsigned char *input, unsigned long inputLen, unsigned char *pkcsBlock, unsigned long pkcsBlockLen) const
{
	assert (inputLen <= MaxUnpaddedLength(pkcsBlockLen));

	// convert from bit length to unsigned char length
	if (pkcsBlockLen % 8 != 0)
	{
		pkcsBlock[0] = 0;
		pkcsBlock++;
	}
	pkcsBlockLen /= 8;

	pkcsBlock[0] = 1;   // block type 1

	// padd with 0xff
	memset(pkcsBlock+1, 0xff, pkcsBlockLen-inputLen-2);

	pkcsBlock[pkcsBlockLen-inputLen-1] = 0;               // separator
	memcpy(pkcsBlock+pkcsBlockLen-inputLen, input, inputLen);
}

unsigned long PKCS_SignaturePaddingScheme::Unpad(const unsigned char *pkcsBlock, unsigned long pkcsBlockLen, unsigned char *output) const
{
	unsigned long maxOutputLen = MaxUnpaddedLength(pkcsBlockLen);

	// convert from bit length to unsigned char length
	if (pkcsBlockLen % 8 != 0)
	{
		if (pkcsBlock[0] != 0)
			return 0;
		pkcsBlock++;
	}
	pkcsBlockLen /= 8;

	// Require block type 1.
	if (pkcsBlock[0] != 1)
		return 0;

	// skip past the padding until we find the seperator
	unsigned long i=1;
	while (i<pkcsBlockLen && pkcsBlock[i++])
		if (pkcsBlock[i-1] != 0xff)     // not valid padding
			return 0;
	assert(i==pkcsBlockLen || pkcsBlock[i-1]==0);

	unsigned long outputLen = pkcsBlockLen - i;
	if (outputLen > maxOutputLen)
		return 0;

	memcpy (output, pkcsBlock+i, outputLen);
	return outputLen;
}


//////////////////////////////////////////////////////////////////////
//	class RSAPubKeyHolder

RSAPubKeyHolder::RSAPubKeyHolder(CByteQueue &bt)
{
	
}

//////////////////////////////////////////////////////////////////////
//	class RSAPriKeyHolder

RSAPriKeyHolder::RSAPriKeyHolder(CByteQueue &bt)
{

}

//////////////////////////////////////////////////////////////////////
//	class CBCPaddedEncryptor

CBCPaddedEncryptor::CBCPaddedEncryptor(BlockCipherBase* cipher, unsigned char* IV)
{
	m_pCipher = cipher;
	unsigned long nBlockSize = m_pCipher->BlockSize();
	memcpy(m_pbBuffer, IV, nBlockSize);
	m_cbBuffer = 0;
}

void CBCPaddedEncryptor::ProcessBuf()
{
	unsigned long nBlockSize = m_pCipher->BlockSize();
	m_pCipher->ProcessBlock(m_pbBuffer, nBlockSize);
	m_outputQueue.Put(m_pbBuffer, nBlockSize);
	m_cbBuffer = 0;
}

void CBCPaddedEncryptor::Put(unsigned char inByte)
{
	unsigned long nBlockSize = m_pCipher->BlockSize();
	m_pbBuffer[m_cbBuffer++] ^= inByte;
	if(m_cbBuffer == nBlockSize)
		ProcessBuf();
}

void CBCPaddedEncryptor::Put(unsigned char* inString, unsigned long length)
{
	unsigned long nBlockSize = m_pCipher->BlockSize();
	while(m_cbBuffer && length){
		CBCPaddedEncryptor::Put(*inString++);
		length--;
	}

	while(length >= nBlockSize){
		xorbuf(m_pbBuffer, inString, nBlockSize);
		ProcessBuf();
		inString += nBlockSize;
		length -= nBlockSize;
	}

	while(length--)
		CBCPaddedEncryptor::Put(*inString++);
}

void CBCPaddedEncryptor::InputFinished()
{
	unsigned long nBlockSize = m_pCipher->BlockSize();
	unsigned char pad = (unsigned char)(nBlockSize - m_cbBuffer);
	do{
		Put(pad);
	}
	while(m_cbBuffer != 0);
}

//////////////////////////////////////////////////////////////////////
//	class CBCPaddedDecryptor

CBCPaddedDecryptor::CBCPaddedDecryptor(BlockCipherBase* cipher, unsigned char* IV)
{
	m_pCipher = cipher;
	unsigned long nBlockSize = m_pCipher->BlockSize();
	memcpy(m_pbIV, IV, nBlockSize);
	m_cbBuffer = 0;
}

void CBCPaddedDecryptor::ProcessBuf()
{
	unsigned long nBlockSize = m_pCipher->BlockSize();
	unsigned char pbTemp[MAXBLOCKSIZE];
	m_pCipher->ProcessBlock(m_pbBuffer, pbTemp, nBlockSize);

	xorbuf(pbTemp, m_pbIV, nBlockSize);
	m_outputQueue.Put(pbTemp, nBlockSize);
	
	memcpy(m_pbIV, m_pbBuffer, nBlockSize);
	m_cbBuffer = 0;
}

void CBCPaddedDecryptor::Put(unsigned char inByte)
{
	unsigned long nBlockSize = m_pCipher->BlockSize();
	if(m_cbBuffer == nBlockSize)
		ProcessBuf();
	m_pbBuffer[m_cbBuffer++] = inByte;
}

void CBCPaddedDecryptor::Put(unsigned char* inString, unsigned long length)
{
	unsigned long nBlockSize = m_pCipher->BlockSize();
	while((m_cbBuffer != nBlockSize) && length){
		CBCPaddedDecryptor::Put(*inString++);
		length--;
	}

	while (length >= nBlockSize){
		ProcessBuf();
		memcpy(m_pbBuffer, inString, nBlockSize);
		m_cbBuffer = nBlockSize;
		inString += nBlockSize;
		length -= nBlockSize;
	}

	while(length--)
		CBCPaddedDecryptor::Put(*inString++);
}

void CBCPaddedDecryptor::InputFinished()
{
	unsigned long nBlockSize = m_pCipher->BlockSize();
	m_pCipher->ProcessBlock(m_pbBuffer, nBlockSize);
	xorbuf(m_pbBuffer, m_pbIV, nBlockSize);
	//if (m_pbBuffer[nBlockSize-1] > nBlockSize)
	//	m_pbBuffer[nBlockSize-1] = 0;     // something's wrong with the padding
	//m_outputQueue.Put(m_pbBuffer, nBlockSize-m_pbBuffer[nBlockSize-1]);
	m_outputQueue.Put(m_pbBuffer, nBlockSize);
}

//////////////////////////////////////////////////////////////////////
//	class DESEncryption

DESEncryption::DESEncryption(const unsigned char* key)
{
	memcpy(m_pbKey, key, sizeof(m_pbKey));
}

void DESEncryption::ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length) const
{
    des_context ctx;
	des_set_key(&ctx, (unsigned char* )m_pbKey);
	for(unsigned long i = 0; i < length/BLOCKSIZE; i++)
		des_encrypt(&ctx, inBlock+i*BLOCKSIZE, outBlock+i*BLOCKSIZE);
}

//////////////////////////////////////////////////////////////////////
//	class DESDecryption

DESDecryption::DESDecryption(const unsigned char* key)
{
	memcpy(m_pbKey, key, sizeof(m_pbKey));
}

void DESDecryption::ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length) const
{
	des_context ctx;
	des_set_key(&ctx, (unsigned char* )m_pbKey);
	for(unsigned long i = 0; i < length/BLOCKSIZE; i++)
		des_decrypt(&ctx, inBlock+i*BLOCKSIZE, outBlock+i*BLOCKSIZE);
}


//////////////////////////////////////////////////////////////////////
//	class DES_EDE_Encryption

DES_EDE_Encryption::DES_EDE_Encryption(const unsigned char* key)
{
	memcpy(m_pbKey, key, sizeof(m_pbKey));
}

void DES_EDE_Encryption::ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length) const
{
    des3_context ctx;
	des3_set_2keys(&ctx, (unsigned char* )m_pbKey, (unsigned char* )(m_pbKey + 8));
	for(unsigned long i = 0; i < length/BLOCKSIZE; i++)
		des3_encrypt(&ctx, inBlock+i*BLOCKSIZE, outBlock+i*BLOCKSIZE);
}
void DES_EDE_Encryption::SetKey(const unsigned char* key)
{
	memcpy(m_pbKey, key, sizeof(m_pbKey));
}

//////////////////////////////////////////////////////////////////////
//	class DES_EDE_Decryption

DES_EDE_Decryption::DES_EDE_Decryption(const unsigned char* key)
{
	memcpy(m_pbKey, key, sizeof(m_pbKey));
}

void DES_EDE_Decryption::ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length) const
{
    des3_context ctx;
	des3_set_2keys(&ctx, (unsigned char* )m_pbKey, (unsigned char* )(m_pbKey + 8));
	for(unsigned long i = 0; i < length/BLOCKSIZE; i++)
		des3_decrypt(&ctx, inBlock+i*BLOCKSIZE, outBlock+i*BLOCKSIZE);
}
void DES_EDE_Decryption::SetKey(const unsigned char* key)
{
	memcpy(m_pbKey, key, sizeof(m_pbKey));
}
//////////////////////////////////////////////////////////////////////
//	class TripleDES_Encryption

TripleDES_Encryption::TripleDES_Encryption(const unsigned char* key)
{
	memcpy(m_pbKey, key, sizeof(m_pbKey));
}

void TripleDES_Encryption::ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length) const
{
    des3_context ctx;
	des3_set_3keys(&ctx, (unsigned char* )m_pbKey, (unsigned char* )(m_pbKey + 8), (unsigned char* )(m_pbKey + 16));
	for(unsigned long i = 0; i < length/BLOCKSIZE; i++)
		des3_encrypt(&ctx, inBlock+i*BLOCKSIZE, outBlock+i*BLOCKSIZE);
}

//////////////////////////////////////////////////////////////////////
//	class TripleDES_Decryption

TripleDES_Decryption::TripleDES_Decryption(const unsigned char* key)
{
	memcpy(m_pbKey, key, sizeof(m_pbKey));
}

void TripleDES_Decryption::ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length) const
{
    des3_context ctx;
	des3_set_3keys(&ctx, (unsigned char* )m_pbKey, (unsigned char* )(m_pbKey + 8), (unsigned char* )(m_pbKey + 16));
	for(unsigned long i = 0; i < length/BLOCKSIZE; i++)
		des3_decrypt(&ctx, inBlock+i*BLOCKSIZE, outBlock+i*BLOCKSIZE);
}

//////////////////////////////////////////////////////////////////////
//	class MD5

MD5::MD5()
{
	m_pCTX = (unsigned char* )new MD5_CTX;
	Init();
}

MD5::~MD5()
{
	if(m_pCTX)
		delete m_pCTX;
}

void MD5::SetHashState(unsigned char* pCTX)
{
	if(m_pCTX == pCTX || !pCTX)
		return;

	if(!m_pCTX)
		m_pCTX = (unsigned char* )new MD5_CTX;

	if(m_pCTX)
		memcpy(m_pCTX, pCTX, sizeof(MD5_CTX));
}

void MD5::Init()
{
	MD5Init((MD5_CTX* )m_pCTX);
}

void MD5::Update(const unsigned char *input, unsigned long length)
{
	MD5Update((MD5_CTX* )m_pCTX, (unsigned char* )input, length);
}

void MD5::Final(unsigned char *hash)
{
	MD5Final(hash, (MD5_CTX* )m_pCTX);
	Init();
}

//////////////////////////////////////////////////////////////////////
//	class SHA

SHA::SHA()
{
	m_pCTX = (unsigned char* )new sha1_ctx;
	Init();
}

SHA::~SHA()
{
	if(m_pCTX)
		delete m_pCTX;
}

void SHA::SetHashState(unsigned char* pCTX)
{
	if(m_pCTX == pCTX || !pCTX)
		return;

	if(!m_pCTX)
		m_pCTX = (unsigned char* )new sha1_ctx;

	if(m_pCTX)
		memcpy(m_pCTX, pCTX, sizeof(sha1_ctx));
}

void SHA::Init()
{
	sha1_init((sha1_ctx* )m_pCTX);
}

void SHA::Update(const unsigned char *input, unsigned long length)
{
	sha1_update((sha1_ctx* )m_pCTX, length, (unsigned char* )input);
}

void SHA::Final(unsigned char *hash)
{
	sha1_final((sha1_ctx* )m_pCTX);
	sha1_digest((sha1_ctx* )m_pCTX, DIGESTSIZE, hash);
	Init();
}

CMac::CMac()
{
	Reset();
}

CMac::~CMac()
{
}

void CMac::Reset()
{
	memset(m_pbKey, 0, sizeof(m_pbKey));
	m_cbKey = 0;
	memset(m_pbBuffer, 0, sizeof(m_pbBuffer));
	memset(m_pbRemainData, 0, sizeof(m_pbRemainData));
	m_cbRemainData = 0;
}

void CMac::Init(unsigned char* pbKey, unsigned long cbKey, unsigned char* pbInitVector /* = 0 */)
{
	memcpy(m_pbKey, pbKey, cbKey);
	m_cbKey = cbKey;

	if(pbInitVector) memcpy(m_pbBuffer, pbInitVector, BLOCKSIZE);
}

int CMac::ProcessBlock()
{
	if(m_cbRemainData != BLOCKSIZE)
		return 0;
	
	for(int i = 0; i <BLOCKSIZE; i++)
		m_pbBuffer[i] = m_pbBuffer[i] ^ m_pbRemainData[i];

	DESEncryption e(m_pbKey);
	//DES_EDE_Encryption e(m_pbKey);
	e.ProcessBlock(m_pbBuffer);

	m_cbRemainData = 0;

	return 1;
}

int CMac::Update(unsigned char* pbData, unsigned long cbData)
{
	if(m_cbKey == 0)
		return 0;

	while((m_cbRemainData < BLOCKSIZE) && cbData){
		m_pbRemainData[m_cbRemainData++] = *pbData;
		pbData++;
		cbData--;
	}
	if(m_cbRemainData < BLOCKSIZE)
		return 1;

	ProcessBlock();

	while(cbData >= BLOCKSIZE){
		memcpy(m_pbRemainData, pbData, BLOCKSIZE);
		m_cbRemainData = BLOCKSIZE;
		pbData += BLOCKSIZE;
		cbData -= BLOCKSIZE;

		ProcessBlock();
	}

	if(cbData){
		memcpy(m_pbRemainData, pbData, cbData);
		m_cbRemainData = cbData;
	}

	return 1;
}

void CMac::Final(unsigned char* pbMac)
{
	if(m_cbRemainData == 0){
		memset(m_pbRemainData, 0, BLOCKSIZE);
		m_pbRemainData[0] = 0x80;
		m_cbRemainData = BLOCKSIZE;
	}
	else{
		m_pbRemainData[m_cbRemainData] = 0x80;
		m_cbRemainData++;
		if(m_cbRemainData != BLOCKSIZE)
			memset(m_pbRemainData + m_cbRemainData, 0, BLOCKSIZE - m_cbRemainData);
		m_cbRemainData = BLOCKSIZE;
	}

	ProcessBlock();

	if(m_cbKey == BLOCKSIZE*2){
		DESDecryption d(m_pbKey + 8);
		DESEncryption e(m_pbKey);
		d.ProcessBlock(m_pbBuffer);
		e.ProcessBlock(m_pbBuffer);
	}

	memcpy(pbMac, m_pbBuffer, 8);

	Reset();
}

void diversify(unsigned char* pbMasterKey, unsigned char* pbFactor, unsigned char* pbTargetKey)
{
	DES_EDE_Encryption des2(pbMasterKey);
	des2.ProcessBlock(pbFactor, pbTargetKey);

	unsigned char pbFactorRev[8];
	for(unsigned long dwI = 0; dwI < sizeof(pbFactorRev); dwI++)
		pbFactorRev[dwI] = ~(pbFactor[dwI]);

	des2.ProcessBlock(pbFactorRev, pbTargetKey + 8);
}

void PadForDESCrypt(unsigned char* pbData, unsigned long* pcbData)
{
	unsigned char cPad = 8 - (unsigned char)((*pcbData) % 8);
	if(cPad == 0){
		cPad = 8;
	}

	pbData[(*pcbData)] = 0x80;
	if(cPad != 1)
		memset(pbData + (*pcbData) + 1, 0, cPad - 1);
	
	*pcbData += cPad;
}

void XorTac(unsigned char* pbTacKey, unsigned char* pbData, unsigned long cbData, unsigned char* pbTac)
{
	unsigned char pbTACSessKey[8];
	for(int i = 0; i < 8; i++)
		pbTACSessKey[i] = pbTacKey[i] ^ pbTacKey[i+8];

	CMac m;
	m.Init(pbTACSessKey, sizeof(pbTACSessKey), 0);
	m.Update(pbData, cbData);
	m.Final(pbTac);
}

static unsigned char pbLastSESPK[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char pbLastMoney[4] = {0x00, 0x00, 0x00, 0x00};
static unsigned char pbLastTermTTC[4] = {0x00, 0x00, 0x00, 0x01};

void CalcPurchaseMac1(unsigned char* pbMPK, unsigned char* pbCardId, unsigned char* pbRand, unsigned char* pbTTC, 
	unsigned char* pbMoney, unsigned char cType, unsigned char* pbTermNo, unsigned char* pbBCDDateTime, 
	unsigned char* pbTermTTC, unsigned char* pbMac1)
{
	unsigned char pbDPK[16] = {0};
	diversify(pbMPK, pbCardId, pbDPK);

	unsigned char pbD[8];
	memcpy(pbD, pbRand, 4);
	memcpy(pbD + 4, pbTTC, 2);
	memcpy(pbD + 6, pbLastTermTTC + 2, 2);
	DES_EDE_Encryption des2(pbDPK);
	des2.ProcessBlock(pbD, pbLastSESPK);

	CMac m;
	m.Init(pbLastSESPK, sizeof(pbLastSESPK), 0);
	m.Update(pbMoney, 4);
	m.Update(&cType, 1);
	m.Update(pbTermNo, 6);
	m.Update(pbBCDDateTime, 7);
	m.Final(pbMac1);

	memcpy(pbTermTTC, pbLastTermTTC, 4);
	memcpy(pbLastMoney, pbMoney, 4);
}

int VerifyPurchaseMac2(unsigned char* pbMac2)
{
	unsigned char pbCalcMac2[4];

	CMac m;
	m.Init(pbLastSESPK, sizeof(pbLastSESPK), 0);
	m.Update(pbLastMoney, 4);
	m.Final(pbCalcMac2);

	if(memcmp(pbCalcMac2, pbMac2, 4))
		return 0;
	else{
		pbLastTermTTC[3]++;
		if(pbLastTermTTC[3] == 0x00){
			pbLastTermTTC[2]++;
			if(pbLastTermTTC[2] == 0x00){
				pbLastTermTTC[1]++;
				if(pbLastTermTTC[1] == 0x00)
					pbLastTermTTC[0]++;
			}
		}
		
		return 1;
	}
}

int bin2hex(const unsigned char* pbData, unsigned long cbData, char* szHexStr)
{
	if(szHexStr == 0)
		return 0;

	if(pbData == 0 || cbData == 0){
		szHexStr[0] = '\0';
		return 1;
	}

	for(unsigned long dwI = 0; dwI < cbData; dwI++)
		sprintf(szHexStr + dwI*2, "%02X", pbData[dwI]);
	szHexStr[cbData*2] = '\0';

	return 1;
}

int hex2byte(char c, unsigned char& val)
{
	if(c >= 0x30 && c <= 0x39)
		val = c - 0x30;
	else if(c >= 0x41 && c <= 0x46)
		val = c - 0x41 + 10;
	else if(c >= 0x61 && c <= 0x66)
		val = c - 0x61 + 10;
	else
		return 0;

	return 1;
}

int hex2bin(const char* szHexChar, unsigned char* pbData, unsigned long& cbData)
{
	if(szHexChar == 0)
		return 0;

	int nLen = strlen(szHexChar);
	if(nLen % 2) return 0;
	if((int)cbData < nLen/2) return 0;

	for(int i = 0; i < nLen; i += 2){
		unsigned char val1, val2;
		if(!hex2byte(szHexChar[i], val1))
			return 0;
		if(!hex2byte(szHexChar[i + 1], val2))
			return 0;
		pbData[i/2] = val1*16 + val2;
	}

	cbData = nLen/2;
	return 1;
}

//将两个BYTE的数据合成为一个BYTE
#define  MAKEBYTE(high,low) (((high & 0x0f)<<4) | (low&0x0f))

int makehex(char *szHexChar,BYTE *pbData,DWORD &cbData)
{
	if ( NULL == szHexChar )
		return 0;

	int i = 0,nLen = cbData;
	BYTE bydataTmp1 = 0,bydataTmp2 = 0;
	
	int j = 0;
	
	
	if (nLen %2 != 0)
	{
		nLen++;
		*(szHexChar + nLen -1 ) = 'F';
	}
	
	for (i = 0;i<nLen;)
	{
		if (0x20 == *(szHexChar + i))
		{
			*pbData = 0x20;
			i++;
			j++;
			continue;
		}
		
		if (!hex2byte(*(szHexChar+i),bydataTmp1))
		{
			if (*(szHexChar+i) == '=')
			{
				*(szHexChar+i) = 'D';
				hex2byte(*(szHexChar+i),bydataTmp1);
			}
			else if (*(szHexChar+i) == 0x09)
			{
				*(szHexChar+i) = 'F';
				hex2byte(*(szHexChar+i),bydataTmp1);
			}
			else
				break;
		}
		
		i++;
		if (!hex2byte(*(szHexChar+i),bydataTmp2))
		{
			if (*(szHexChar+i) == '=')
			{
				*(szHexChar+i) = 'D';
				hex2byte(*(szHexChar+i),bydataTmp2);
			}
			else if (*(szHexChar+i) == 0x09)
			{
				*(szHexChar+i) = 'F';
				hex2byte(*(szHexChar+i),bydataTmp2);
			}
			else
				break;
		}
		pbData[j++]= MAKEBYTE(bydataTmp1,bydataTmp2);
		i++;
	}
	cbData = j;
	return 1;
}

void xorbuf(unsigned char* buf, unsigned char* mask, unsigned long count)
{
	for(unsigned long i=0; i<count; i++)
		buf[i] ^= mask[i];
}

void ByteReverse(unsigned char* pbBuf, unsigned long dwBufLen)
{
	if(pbBuf == NULL)
		return;

	for(unsigned long dwI = 0; dwI < dwBufLen / 2; dwI++){
		unsigned char t = pbBuf[dwI];
		pbBuf[dwI] = pbBuf[dwBufLen - dwI - 1];
		pbBuf[dwBufLen - dwI - 1] = t;
	}
}

int LittleEndian_ByteStream2DWord(const unsigned char* pbContent, const unsigned long cbContent, unsigned long& dwValue)
{
	if(pbContent == NULL)
		return FALSE;
	
	dwValue = 0;

	unsigned long dwByteNum = sizeof(unsigned long);
	if(cbContent < dwByteNum) dwByteNum = cbContent;

	for(unsigned long dwI = 0; dwI < dwByteNum; dwI++){
		unsigned char b = pbContent[dwI];
		dwValue += (b << (8*dwI));
	}

	return TRUE;
}

int BigEndian_ByteStream2DWord(const unsigned char* pbContent, const unsigned long cbContent, unsigned long& dwValue)
{
	if(pbContent == NULL)
		return FALSE;
	
	dwValue = 0;

	unsigned long dwByteNum = sizeof(unsigned long);
	if(cbContent < dwByteNum) dwByteNum = cbContent;

	for(unsigned long dwI = 0; dwI < dwByteNum; dwI++){
		unsigned char b = pbContent[cbContent - 1 - dwI];
		dwValue += (b << (8*dwI));
	}

	return TRUE;
}

int LittleEndian_ByteStream2Word(const unsigned char* pbContent, const unsigned long cbContent, unsigned short& wValue)
{
	if(pbContent == NULL)
		return FALSE;
	
	wValue = 0;

	unsigned long dwByteNum = sizeof(unsigned short);
	if(cbContent < dwByteNum) dwByteNum = cbContent;

	for(unsigned long dwI = 0; dwI < dwByteNum; dwI++){
		unsigned char b = pbContent[dwI];
		wValue += (b << (8*dwI));
	}

	return TRUE;
}

int BigEndian_ByteStream2Word(const unsigned char* pbContent, const unsigned long cbContent, unsigned short& wValue)
{
	if(pbContent == NULL)
		return FALSE;
	
	wValue = 0;

	unsigned long dwByteNum = sizeof(unsigned short);
	if(cbContent < dwByteNum) dwByteNum = cbContent;

	for(unsigned long dwI = 0; dwI < dwByteNum; dwI++){
		unsigned char b = pbContent[cbContent - 1 - dwI];
		wValue += (b << (8*dwI));
	}

	return TRUE;
}

int LittleEndian_DWord2ByteStream(const unsigned long dwValue, unsigned char* pbContent, unsigned long cbContent)
{
	if(pbContent == NULL)
		return FALSE;

	if(cbContent < sizeof(unsigned long)){
		//确保高位非0字节
		unsigned long dwIF = 0;
		while(dwIF < (sizeof(unsigned long) - cbContent)){
			unsigned char b = (unsigned char)((dwValue >> (8*(sizeof(unsigned long) - dwIF - 1))) & 0xFF);
			if(b) break;
			dwIF++;
		}
		for(unsigned long dwI = 0; dwI < cbContent; dwI++){
			unsigned char b = (unsigned char)((dwValue >> (8*dwI)) & 0xFF);
			pbContent[dwI] = b;
		}
	}
	else{
		memset(pbContent, 0, cbContent);
		for(unsigned long dwI = 0; dwI < sizeof(unsigned long); dwI++){
			unsigned char b = (unsigned char)((dwValue >> 8*dwI) & 0xFF);
			pbContent[dwI] = b;
		}
	}

	return TRUE;
}

int BigEndian_DWord2ByteStream(const unsigned long dwValue, unsigned char* pbContent, unsigned long cbContent)
{
	if(pbContent == NULL)
		return FALSE;

	if(cbContent < sizeof(unsigned long)){
		//确保高位非0字节
		unsigned long dwIF = 0;
		while(dwIF < (sizeof(unsigned long) - cbContent)){
			unsigned char b = (unsigned char)((dwValue >> (8*(sizeof(unsigned long) - dwIF - 1))) & 0xFF);
			if(b) break;
			dwIF++;
		}
		for(unsigned long dwI = 0; dwI < cbContent; dwI++){
			unsigned char b = (unsigned char)((dwValue >> (8*(sizeof(unsigned long) - dwIF- dwI - 1))) & 0xFF);
			pbContent[dwI] = b;
		}
	}
	else{
		memset(pbContent, 0, cbContent);
		for(unsigned long dwI = 0; dwI < sizeof(unsigned long); dwI++){
			unsigned char b = (unsigned char)((dwValue >> 8*dwI) & 0xFF);
			pbContent[cbContent - 1 - dwI] = b;
		}
	}

	return TRUE;
}

int LittleEndian_Word2ByteStream(const unsigned short wValue, unsigned char* pbContent, unsigned long cbContent)
{
	if(pbContent == NULL)
		return FALSE;

	memset(pbContent, 0, cbContent);

	if(cbContent < sizeof(unsigned short)){
		//确保高位非0字节
		unsigned long dwIF = 0;
		while(dwIF < (sizeof(unsigned short) - cbContent)){
			unsigned char b = (unsigned char)((wValue >> (8*(sizeof(unsigned short) - dwIF - 1))) & 0xFF);
			if(b) break;
			dwIF++;
		}
		for(unsigned long dwI = 0; dwI < cbContent; dwI++){
			unsigned char b = (unsigned char)((wValue >> (8*dwI)) & 0xFF);
			pbContent[dwI] = b;
		}
	}
	else{
		for(unsigned long dwI = 0; dwI < sizeof(unsigned short); dwI++){
			unsigned char b = (unsigned char)((wValue >> 8*dwI) & 0xFF);
			pbContent[dwI] = b;
		}
	}

	return TRUE;
}

int BigEndian_Word2ByteStream(const unsigned short wValue, unsigned char* pbContent, unsigned long cbContent)
{
	if(pbContent == NULL)
		return FALSE;

	memset(pbContent, 0, cbContent);

	if(cbContent < sizeof(unsigned short)){
		//确保高位非0字节
		unsigned long dwIF = 0;
		while(dwIF < (sizeof(unsigned short) - cbContent)){
			unsigned char b = (unsigned char)((wValue >> (8*(sizeof(unsigned short) - dwIF - 1))) & 0xFF);
			if(b) break;
			dwIF++;
		}
		for(unsigned long dwI = 0; dwI < cbContent; dwI++){
			unsigned char b = (unsigned char)((wValue >> (8*(sizeof(unsigned short) - dwIF- dwI - 1))) & 0xFF);
			pbContent[dwI] = b;
		}
	}
	else{
		for(unsigned long dwI = 0; dwI < sizeof(unsigned short); dwI++){
			unsigned char b = (unsigned char)((wValue >> 8*dwI) & 0xFF);
			pbContent[cbContent - 1 - dwI] = b;
		}
	}

	return TRUE;
}
NAMESPACE_END
