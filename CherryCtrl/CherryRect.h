#pragma once

///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherryRect
/// \brief		width, height ����� ���ϰ� �ϱ� ���� ���� Ŭ����
///				CRect�� ��ӹ޾� �����Ǿ���.
/// \author		ogoons
/// \date		2013-12-06
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CCherryRect : public CRect
{
public:
	CCherryRect();
	CCherryRect(int left, int top, int width, int height);
	//CCherryRect(const CCherryRect &cherryRect);
	virtual ~CCherryRect();

	void SetRectangle(int left, int top, int width, int height) throw();
};

