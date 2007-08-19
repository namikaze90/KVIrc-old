#ifndef _KVI_SSL_H_
#define _KVI_SSL_H_
//
//   File : kvi_ssl.h
//   Creation date : Mon May 27 2002 21:36:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "kvi_settings.h"

#ifdef COMPILE_SSL_SUPPORT

#include "kvi_string.h"
#include "kvi_sockettype.h"

#include "kvi_asciidict.h"

#include <openssl/ssl.h>
#include <QHash>

class KVILIB_API KviSSLCertificate
{
public:
	KviSSLCertificate(X509 * x509);
	~KviSSLCertificate();
protected:
	X509 * m_pX509;
	QHash<QString,QString> * m_pSubject;
	QHash<QString,QString> * m_pIssuer;
	int                      m_iPubKeyBits;
	QString                  m_szPubKeyType;
	int                      m_iSerialNumber;
	int                      m_iVersion;
	QString                  m_szSignatureType;
	QString                  m_szSignatureContents;
private:
	void extractSubject();
	void extractIssuer();
	void extractPubKeyInfo();
	void extractSerialNumber();
	void extractSignature();
	const QString& dictEntry(QHash<QString,QString> * dict,const QString& entry);
	void splitX509String(QHash<QString,QString> * dict,const QString& t);
//	void getPKeyType(int type,KviStr &buffer);
public:
	void setX509(X509 * x509);

	const QString& signatureType(){ return m_szSignatureType; };
	const QString& signatureContents(){ return m_szSignatureContents; };

	const QString& subjectCountry(){ return dictEntry(m_pSubject,"C"); };
	const QString& subjectStateOrProvince(){ return dictEntry(m_pSubject,"ST"); };
	const QString& subjectLocality(){ return dictEntry(m_pSubject,"L"); };
	const QString& subjectOrganization(){ return dictEntry(m_pSubject,"O"); };
	const QString& subjectOrganizationalUnit(){ return dictEntry(m_pSubject,"OU"); };
	const QString& subjectCommonName(){ return dictEntry(m_pSubject,"CN"); };
	
	const QString& issuerCountry(){ return dictEntry(m_pIssuer,"C"); };
	const QString& issuerStateOrProvince(){ return dictEntry(m_pIssuer,"ST"); };
	const QString& issuerLocality(){ return dictEntry(m_pIssuer,"L"); };
	const QString& issuerOrganization(){ return dictEntry(m_pIssuer,"O"); };
	const QString& issuerOrganizationalUnit(){ return dictEntry(m_pIssuer,"OU"); };
	const QString& issuerCommonName(){ return dictEntry(m_pIssuer,"CN"); };

	int publicKeyBits(){ return m_iPubKeyBits; };
	const QString& publicKeyType(){ return m_szPubKeyType; };

	int serialNumber(){ return m_iSerialNumber; };

	int version(){ return m_iVersion; };
#ifdef COMPILE_ON_WINDOWS
	// On windows we need to override new and delete operators
	// to ensure that always the right new/delete pair is called for an object instance
	// This bug is present in all the classes exported by a module that
	// can be instantiated/destroyed from external modules.
	// (this is a well known bug described in Q122675 of MSDN)
	void       * operator new(size_t tSize);
	void         operator delete(void * p);
#endif
};

class KVILIB_API KviSSLCipherInfo
{
public:
	KviSSLCipherInfo(SSL_CIPHER * c);
	~KviSSLCipherInfo();
protected:
	QString       m_szVersion;
	int           m_iNumBits;
	int           m_iNumBitsUsed;
	QString       m_szName;
	QString       m_szDescription;
public:
	const QString& name(){ return m_szName; };
	const QString& description(){ return m_szDescription; };
	int bits(){ return m_iNumBits; };
	int bitsUsed(){ return m_iNumBitsUsed; };
	const QString& version(){ return m_szVersion; };
#ifdef COMPILE_ON_WINDOWS
	// On windows we need to override new and delete operators
	// to ensure that always the right new/delete pair is called for an object instance
	// This bug is present in all the classes exported by a module that
	// can be instantiated/destroyed from external modules.
	// (this is a well known bug described in Q122675 of MSDN)
	void       * operator new(size_t tSize);
	void         operator delete(void * p);
#endif
};

#ifdef Success
	#undef Success
#endif


class KVILIB_API KviSSL
{
public:
	enum Method { Client , Server };
	enum Result { Success , NotInitialized , WantRead , WantWrite , ZeroReturn , FileIoError ,
					UnknownError , ObscureError , SSLError , SyscallError , RemoteEndClosedConnection };
public:
	KviSSL();
	~KviSSL();
public:
	SSL        * m_pSSL;
	SSL_CTX    * m_pSSLCtx;
	QString      m_szPass;
public:
	static void globalInit();
	static void globalDestroy();
public:
	bool initSocket(kvi_socket_t fd);
	bool initContext(KviSSL::Method m);
	void shutdown();
	KviSSL::Result connect();
	KviSSL::Result accept();
	int read(char * buffer,int len);
	int write(const char * buffer,int len);
	// SSL ERRORS
	unsigned long getLastError(bool bPeek = false);
	bool getLastErrorString(KviStr &buffer,bool bPeek = false);
	// Protocol error
	KviSSL::Result getProtocolError(int ret);
	KviSSLCertificate * getPeerCertificate();
	KviSSLCipherInfo * getCurrentCipherInfo();
	KviSSL::Result useCertificateFile(const char * cert,const char * pass);
	KviSSL::Result usePrivateKeyFile(const char * key,const char * pass);
#ifdef COMPILE_ON_WINDOWS
	// On windows we need to override new and delete operators
	// to ensure that always the right new/delete pair is called for an object instance
	// This bug is present in all the classes exported by a module that
	// can be instantiated/destroyed from external modules.
	// (this is a well known bug described in Q122675 of MSDN)
	void       * operator new(size_t tSize);
	void         operator delete(void * p);
#endif
private:
	KviSSL::Result connectOrAcceptError(int ret);
};


#endif //COMPILE_SSL_SUPPORT

#endif //_KVI_SSL_H_
