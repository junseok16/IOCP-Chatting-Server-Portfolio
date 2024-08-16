#pragma once

namespace ServerCore
{
	struct FIOCPEvent;

	class CIOCPObject : public std::enable_shared_from_this<CIOCPObject>
	{
	public:
		virtual HANDLE GetHandle() abstract;

		virtual void Dispatch(FIOCPEvent* _pIOCPEvent, int32 _iNumBytes = 0) abstract;
	};

	class CIOCPCore final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CIOCPCore();

		virtual ~CIOCPCore();
		
	/********************
		Getter/Setter
	********************/
	public:
		HANDLE GetHandle() const { return m_hIOCP; }

	/********************
		Methods
	********************/
	public:
		bool Register(std::shared_ptr<CIOCPObject> _spIOCPObject);

		bool Dispatch(uint32 _iTimeOut = INFINITE);
		
	/********************
		Data Members
	********************/
	private:
		HANDLE m_hIOCP = nullptr;
	};
}
