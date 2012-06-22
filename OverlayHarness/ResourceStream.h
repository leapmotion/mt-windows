#pragma once

class CResourceStream:
	public IStream
{
public:
	CResourceStream(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType);
	~CResourceStream(void);

private:
	ULONG m_refCt;

	HRSRC m_hRsrc;
	const char* m_pData;
	unsigned long long m_len;
	unsigned long long m_offset;

public:
	virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead)
	{
		ULONG read = min(cb, (ULONG)(m_len - m_offset));
		memcpy(pv, m_pData + m_offset, read);
		if(pcbRead)
			*pcbRead = read;

		m_offset += read;
		return read < cb ? S_FALSE : S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten) {return STG_E_ACCESSDENIED;}

	virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
		switch(dwOrigin)
		{
		case STREAM_SEEK_SET:
			m_offset = dlibMove.QuadPart;
			break;
		case STREAM_SEEK_CUR:
			m_offset += dlibMove.QuadPart;
			break;
		case STREAM_SEEK_END:
			m_offset = m_len - dlibMove.QuadPart;
			break;
		default:
			return E_INVALIDARG;
		}

		if(plibNewPosition)
			plibNewPosition->QuadPart = m_offset;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) {return E_NOTIMPL;}

	virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
	{
		cb.QuadPart = min(cb.QuadPart, m_len - m_offset);
		HRESULT rs = pstm->Write(m_pData + m_offset, (ULONG)cb.QuadPart, &pcbWritten->LowPart);

		pcbWritten->HighPart = 0;
		*pcbRead = *pcbWritten;
		return rs;
	}

	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) {return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE Revert(void) {return S_OK;}
	virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion( ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		memset(pstatstg, 0, sizeof(*pstatstg));
		pstatstg->pwcsName = nullptr;
		pstatstg->type = STGTY_STREAM;
		pstatstg->cbSize.QuadPart = m_len;
		pstatstg->grfMode = STGM_READ;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm)
	{
		return E_NOTIMPL;
	}

	// IUnknown methods:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void**)
	{
		return E_NOTIMPL;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return ++m_refCt;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)
	{
		return
			!--m_refCt ?
			delete this, 0 :
			m_refCt;
	}
};

