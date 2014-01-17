#	include "ThreadTask.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTask::ThreadTask()
		: m_complete(false)
		, m_successful(false)
		, m_cancel(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadTask::~ThreadTask()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::main()
	{
		if( m_cancel == true )
		{
			m_successful = false;
			m_complete = true;

			return;
		}

		m_successful = this->_onMain();
		m_complete = true;
	}
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::join()
    {
        this->_onJoin();
    }
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onJoin()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::run()
	{
		bool state = this->_onRun();

		return state;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::isComplete() const
	{
		return m_complete;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::isCancel() const
	{
		return m_cancel;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::_onMain()
	{
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::_onRun()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::cancel()
	{
		m_cancel = true;

		this->_onCancel();
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onCancel()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::update()
	{
		this->_onUpdate();

		if( m_complete == true )
		{
			this->_onComplete( m_successful );
		}

		return m_complete;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onUpdate()
	{
	}
	//////////////////////////////////////////////////////////////////////////	
	void ThreadTask::_onComplete( bool _successful )
	{
		//Empty
	}
}