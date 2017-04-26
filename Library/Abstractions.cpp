
#include "Abstractions.h"

namespace libBasic
{

	void DummyTextLog::Write( const char * )
	{
		// sink the writes
	}

	AbstractConsoleInputStream::AbstractConsoleInputStream()
		: m_pOnIdleEvent(0)
	{
	}

}
