#ifndef __SOFT_ALGO_H__
#define __SOFT_ALGO_H__


#ifdef NO_NAMESPACE
#define std
#define SOFTENC SoftEnc
#define USING_NAMESPACE(x)
#define NAMESPACE_BEGIN(x)
#define NAMESPACE_END
#define ANONYMOUS_NAMESPACE_BEGIN
#else
#define USING_NAMESPACE(x) using namespace x;
#define NAMESPACE_BEGIN(x) namespace x {
#define ANONYMOUS_NAMESPACE_BEGIN namespace {
#define NAMESPACE_END }
#endif

NAMESPACE_BEGIN(SOFTENC)

const unsigned char SHAdecoration[] = {0x30,0x21,0x30,0x09,0x06,0x05,0x2B,0x0E,0x03,0x02,0x1A,0x05,0x00,0x04,0x14};
const unsigned int SHAdecorationlen = sizeof(SHAdecoration);
const unsigned char MD5decoration[] = {0x30,0x20,0x30,0x0c,0x06,0x08,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x02,0x05,0x05,0x00,0x04,0x10};
const unsigned int MD5decorationlen = sizeof(MD5decoration);

//////////////////////////////////////////////////////////////////////
//	class CRandomGenerator

class CRandomGenerator{
public:
	CRandomGenerator(){ Seed(); }
	~CRandomGenerator(){}

public:
	void GetBlock(unsigned char* outString, unsigned long length);
	unsigned short GetShort(unsigned short wMin, unsigned short wMax);

private:
	void Seed();
};

//////////////////////////////////////////////////////////////////////
//	class CByteQueue

class CByteQueue{
public:
	CByteQueue();
	~CByteQueue();

public:
	unsigned long CurrentSize() const { return m_cbSize; }
	void Put(const unsigned char* inString, unsigned long length);
	unsigned long Get(unsigned char* outString, unsigned long getMax) const;
	unsigned char* GetData() const
		{ return m_pbBuffer; }
	CByteQueue& operator=(const CByteQueue& t);
	void Reverse();

private:
	unsigned char* m_pbBuffer;
	unsigned long m_cbSize;
};

//////////////////////////////////////////////////////////////////////
//	class Integer

class Integer{
public:
	Integer(){}
	Integer(unsigned long t);
	Integer(unsigned char* inString, unsigned long length)
		{ m_byteQueue.Put(inString, length); }
	Integer(const Integer& t);

public:
	long ConvertToLong() const;
	unsigned long Encode(unsigned char* output, unsigned long outputLen) const
		{ return m_byteQueue.Get(output, outputLen); }
	unsigned long ByteCount() const
		{ return m_byteQueue.CurrentSize(); }
	Integer& operator=(const Integer& t);
	void Reverse() { m_byteQueue.Reverse(); }
	unsigned char* GetData() const
		{ return m_byteQueue.GetData(); }

private:
	CByteQueue m_byteQueue;
};

//////////////////////////////////////////////////////////////////////
//	class PKCS_EncryptionPaddingScheme

class PKCS_EncryptionPaddingScheme
{
public:
	unsigned long MaxUnpaddedLength(unsigned long paddedLength) const;
	void Pad(CRandomGenerator &rng, const unsigned char *raw, unsigned long inputLength, unsigned char *padded, unsigned long paddedLength) const;
	unsigned long Unpad(const unsigned char *padded, unsigned long paddedLength, unsigned char *raw) const;
};

//////////////////////////////////////////////////////////////////////
//	class PKCS_SignaturePaddingScheme

class PKCS_SignaturePaddingScheme
{
public:
	unsigned long MaxUnpaddedLength(unsigned long paddedLength) const;
	void Pad(CRandomGenerator &rng, const unsigned char *raw, unsigned long inputLength, unsigned char *padded, unsigned long paddedLength) const;
	unsigned long Unpad(const unsigned char *padded, unsigned long paddedLength, unsigned char *raw) const;
};


//////////////////////////////////////////////////////////////////////
//	class RSAPubKeyHolder

class RSAPubKeyHolder{
public:
	RSAPubKeyHolder(const Integer &n, const Integer &e) : n(n), e(e) {}
	RSAPubKeyHolder(CByteQueue &bt);
	
public:
	const Integer& GetModulus() const { return n; }
	const Integer& GetExponent() const { return e; }

protected:
	RSAPubKeyHolder() {}
	Integer n, e;
};

//////////////////////////////////////////////////////////////////////
//	class RSAPriKeyHolder

class RSAPriKeyHolder : public RSAPubKeyHolder
{
public:
	RSAPriKeyHolder(const Integer &n, const Integer &e, const Integer &d,
		const Integer &p, const Integer &q, const Integer &dp, const Integer &dq, const Integer &u)
	: RSAPubKeyHolder(n, e), d(d), p(p), q(q), dp(dp), dq(dq), u(u) {};
	
	RSAPriKeyHolder(CByteQueue &bt);

public:
	void DEREncode(CByteQueue &bt) const;
	const Integer& GetPrime1() const { return p; }
	const Integer& GetPrime2() const { return q; }
	const Integer& GetDecryptionExponent() const { return d; }
	const Integer& GetExponent1() const { return dp; }
	const Integer& GetExponent2() const { return dq; }
	const Integer& GetCoefficient() const { return u; }

protected:
	Integer d, p, q, dp, dq, u;
};

//////////////////////////////////////////////////////////////////////
//	class BlockCipherBase

class BlockCipherBase{
public:
	virtual ~BlockCipherBase() {}

	virtual void ProcessBlock(unsigned char* inoutBlock, unsigned long length) const =0;
	virtual void ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length) const =0;
	virtual unsigned int BlockSize() const =0;
};

//////////////////////////////////////////////////////////////////////
//	class CBCPaddedEncryptor

class CBCPaddedEncryptor{
public:
	enum {MAXBLOCKSIZE=64};
	CBCPaddedEncryptor(BlockCipherBase* cipher, unsigned char* IV);

	void Put(unsigned char inByte);
	void Put(unsigned char* inString, unsigned long length);
	void InputFinished();
	unsigned long MaxRetrieveable() { return m_outputQueue.CurrentSize(); }
	unsigned long Get(unsigned char* outString, unsigned long getMax)
		{ return m_outputQueue.Get(outString, getMax); }

private:
	void ProcessBuf();
	
	BlockCipherBase* m_pCipher;
	unsigned char m_pbBuffer[MAXBLOCKSIZE];
	unsigned long m_cbBuffer;
	CByteQueue m_outputQueue;
};

//////////////////////////////////////////////////////////////////////
//	class CBCPaddedDecryptor

class CBCPaddedDecryptor{
public:
	enum {MAXBLOCKSIZE=64};
	CBCPaddedDecryptor(BlockCipherBase* cipher, unsigned char* IV);

	void Put(unsigned char inByte);
	void Put(unsigned char* inString, unsigned long length);
	void InputFinished();
	unsigned long MaxRetrieveable() { return m_outputQueue.CurrentSize(); }
	unsigned long Get(unsigned char* outString, unsigned long getMax)
		{ return m_outputQueue.Get(outString, getMax); }

private:
	void ProcessBuf();
	
	BlockCipherBase* m_pCipher;
	unsigned char m_pbIV[MAXBLOCKSIZE];
	unsigned char m_pbBuffer[MAXBLOCKSIZE];
	unsigned long m_cbBuffer;
	CByteQueue m_outputQueue;
};

//////////////////////////////////////////////////////////////////////
//	class DESEncryption

class DESEncryption : public BlockCipherBase{
public:
	enum {KEYLENGTH=8, BLOCKSIZE=8};
	DESEncryption(const unsigned char* key);
public:
	virtual void ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length = BLOCKSIZE) const;
	virtual void ProcessBlock(unsigned char* inBlock, unsigned long length = BLOCKSIZE) const { ProcessBlock(inBlock, inBlock, length); }
	virtual unsigned int BlockSize() const { return BLOCKSIZE; }
private:
	unsigned char m_pbKey[KEYLENGTH];
};

//////////////////////////////////////////////////////////////////////
//	class DESDecryption

class DESDecryption : public BlockCipherBase{
public:
	enum {KEYLENGTH=8, BLOCKSIZE=8};
	DESDecryption(const unsigned char* key);
public:
	virtual void ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length = BLOCKSIZE) const;
	virtual void ProcessBlock(unsigned char* inBlock, unsigned long length = BLOCKSIZE) const { ProcessBlock(inBlock, inBlock, length); }
	virtual unsigned int BlockSize() const { return BLOCKSIZE; }


private:
	unsigned char m_pbKey[KEYLENGTH];
};

//////////////////////////////////////////////////////////////////////
//	class DES_EDE_Encryption

class DES_EDE_Encryption : public BlockCipherBase{
public:
	enum {KEYLENGTH=16, BLOCKSIZE=8};
	DES_EDE_Encryption(const unsigned char* key);
public:
	virtual void ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length = BLOCKSIZE) const;
	virtual void ProcessBlock(unsigned char* inBlock, unsigned long length = BLOCKSIZE) const { ProcessBlock(inBlock, inBlock, length); }
	virtual unsigned int BlockSize() const { return BLOCKSIZE; }
	//begin:added by zhangheyong 2009/04/10
	virtual void SetKey(const unsigned char* key) ;
	//end:added by zhangheyong 2009/04/10
private:
	unsigned char m_pbKey[KEYLENGTH];
};

//////////////////////////////////////////////////////////////////////
//	class DES_EDE_Decryption

class DES_EDE_Decryption : public BlockCipherBase{
public:
	enum {KEYLENGTH=16, BLOCKSIZE=8};
	DES_EDE_Decryption(const unsigned char* key);
public:
	virtual void ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length = BLOCKSIZE) const;
	virtual void ProcessBlock(unsigned char* inBlock, unsigned long length = BLOCKSIZE) const { ProcessBlock(inBlock, inBlock, length); }
	virtual unsigned int BlockSize() const { return BLOCKSIZE; }
	//begin:added by zhangheyong 2009/06/04
	virtual void SetKey(const unsigned char* key) ;
	//end:added by zhangheyong 2009/06/04
private:
	unsigned char m_pbKey[KEYLENGTH];
};

//////////////////////////////////////////////////////////////////////
//	class TripleDES_Encryption

class TripleDES_Encryption : public BlockCipherBase{
public:
	enum {KEYLENGTH=24, BLOCKSIZE=8};
	TripleDES_Encryption(const unsigned char* key);
public:
	virtual void ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length = BLOCKSIZE) const;
	virtual void ProcessBlock(unsigned char* inBlock, unsigned long length = BLOCKSIZE) const { ProcessBlock(inBlock, inBlock, length); }
	virtual unsigned int BlockSize() const { return BLOCKSIZE; }
private:
	unsigned char m_pbKey[KEYLENGTH];
};

//////////////////////////////////////////////////////////////////////
//	class TripleDES_Decryption

class TripleDES_Decryption : public BlockCipherBase{
public:
	enum {KEYLENGTH=24, BLOCKSIZE=8};
	TripleDES_Decryption(const unsigned char* key);
public:
	virtual void ProcessBlock(unsigned char* inBlock, unsigned char* outBlock, unsigned long length = BLOCKSIZE) const;
	virtual void ProcessBlock(unsigned char* inBlock, unsigned long length = BLOCKSIZE) const { ProcessBlock(inBlock, inBlock, length); }
	virtual unsigned int BlockSize() const { return BLOCKSIZE; }
private:
	unsigned char m_pbKey[KEYLENGTH];
};

//////////////////////////////////////////////////////////////////////
//	class HashBase

class HashBase{
public:
	virtual unsigned int DigestSize() = 0;
	virtual void Update(const unsigned char *input, unsigned long length) = 0;
	virtual void Final(unsigned char *hash) = 0;
	virtual void CalculateDigest(unsigned char *digest, const unsigned char *input, unsigned long length)
		{Update(input, length); Final(digest);}
	virtual unsigned char* GetHashState() = 0;
	virtual void SetHashState(unsigned char*) = 0;
};

//////////////////////////////////////////////////////////////////////
//	class MD5

class MD5 : public HashBase{
public:
	enum {DIGESTSIZE = 16};
	MD5();
	~MD5();
	
public:
	virtual unsigned int DigestSize() {return DIGESTSIZE;}
	virtual void Update(const unsigned char *input, unsigned long length);
	virtual void Final(unsigned char *hash);
	virtual unsigned char* GetHashState() {return m_pCTX; }
	virtual void SetHashState(unsigned char*);

private:
	unsigned char* m_pCTX;
	void Init();
};

//////////////////////////////////////////////////////////////////////
//	class SHA

class SHA : public HashBase{
public:
	enum {DIGESTSIZE = 20};
	SHA();
	~SHA();
	
public:
	virtual unsigned int DigestSize() {return DIGESTSIZE;}
	virtual void Update(const unsigned char *input, unsigned long length);
	virtual void Final(unsigned char *hash);
	virtual unsigned char* GetHashState() {return m_pCTX; }
	virtual void SetHashState(unsigned char*);

private:
	unsigned char* m_pCTX;
	void Init();
};

//////////////////////////////////////////////////////////////////////
//	class CMac

class CMac{
public:
	enum{BLOCKSIZE=8};
	CMac();
	~CMac();

public:
	void Init(unsigned char* pbKey, unsigned long cbKey, unsigned char* pbInitVector);
	int Update(unsigned char* pbData, unsigned long cbData);
	void Final(unsigned char* pbMac);

protected:
	void Reset();
	int ProcessBlock();

private:
	unsigned char m_pbKey[BLOCKSIZE*2];
	unsigned long m_cbKey;
	unsigned char m_pbRemainData[BLOCKSIZE];
	unsigned long m_cbRemainData;
	unsigned char m_pbBuffer[BLOCKSIZE];
};

void crc16_init(unsigned short *uCrc16);
void crc16_update(unsigned short *uCrc16, unsigned char *pBuffer, unsigned long uBufSize);
void crc16_final(unsigned short *uCrc16);

void crc16ccitt_init(unsigned short *uCcitt16);
void crc16ccitt_update(unsigned short *uCcitt16, unsigned char *pBuffer, unsigned long uBufSize);
void crc16ccitt_final(unsigned short *uCcitt16);

unsigned short _calc_crc(unsigned char *pbData, unsigned long cbData);

void xorbuf(unsigned char* buf, unsigned char* mask, unsigned long count);
void diversify(unsigned char* pbMasterKey, unsigned char* pbFactor, unsigned char* pbTargetKey);
void PadForDESCrypt(unsigned char* pbData, unsigned long* pcbData);
void XorTac(unsigned char* pbTacKey, unsigned char* pbData, unsigned long cbData, unsigned char* pbTac);
void CalcPurchaseMac1(unsigned char* pbMPK, unsigned char* pbCardId, unsigned char* pbRand, unsigned char* pbTTC, 
	unsigned char* pbMoney, unsigned char cType, unsigned char* pbTermNo, unsigned char* pbBCDDateTime, 
	unsigned char* pbTermTTC, unsigned char* pbMac1);
int VerifyPurchaseMac2(unsigned char* pbMac2);

int bin2hex(const unsigned char* pbData, unsigned long cbData, char* szHexStr);
int hex2byte(char c, unsigned char& val);
int hex2bin(const char* szHexChar, unsigned char* pbData, unsigned long& cbData);
int makehex(char *szHexChar,BYTE *pbData,DWORD &cbData);

void ByteReverse(unsigned char* pbBuf, unsigned long dwBufLen);

int LittleEndian_ByteStream2DWord(const unsigned char* pbContent, const unsigned long cbContent, unsigned long& dwValue);
int LittleEndian_ByteStream2Word(const unsigned char* pbContent, const unsigned long cbContent, unsigned short& wValue);
int LittleEndian_DWord2ByteStream(const unsigned long dwValue, unsigned char* pbContent, unsigned long cbContent);
int LittleEndian_Word2ByteStream(const unsigned short wValue, unsigned char* pbContent, unsigned long cbContent);

int BigEndian_ByteStream2DWord(const unsigned char* pbContent, const unsigned long cbContent, unsigned long& dwValue);
int BigEndian_ByteStream2Word(const unsigned char* pbContent, const unsigned long cbContent, unsigned short& wValue);
int BigEndian_DWord2ByteStream(const unsigned long dwValue, unsigned char* pbContent, unsigned long cbContent);
int BigEndian_Word2ByteStream(const unsigned short wValue, unsigned char* pbContent, unsigned long cbContent);

NAMESPACE_END
#endif
