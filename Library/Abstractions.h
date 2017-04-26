
#pragma once

#include <stdint.h>
#include <string.h> // for size_t  TODO: revisit

namespace libBasic
{
	typedef void (*AbstractCallbackFunction)( void *userData );



	class AbstractClosure
	{
	public:

        AbstractClosure()
            : _function(nullptr)
            , _userData(nullptr)
        {
        }

        AbstractClosure( AbstractCallbackFunction functionAddress, void *userData )
            : _function(functionAddress)
            , _userData(userData)
        {
        }

        inline void Invoke()
        {
            if( _function != nullptr )
            {
                _function( _userData );
            }
        }

    private:

        AbstractCallbackFunction  _function;
        void *_userData;

	};



	class AbstractEvent
	{
	public:
		virtual void OnEvent() = 0;
	};



	class AbstractDataSource
	{
	public:
		// An abstract interface through which we fetch data blocks,
		// where the nature of the source is not specified here.
		virtual bool Copy( void *destinationAddress, uintptr_t sourceLocation, uintptr_t lengthBytes ) = 0;
	};



	class AbstractTextOutputStream
	{
	public:
		// An abstract interface through which text can be reported.
		// Reminder: Does NOT apply a new-line.
		virtual void Write( const char *message ) = 0;
	};

	class AbstractBinaryOutputStream: public AbstractTextOutputStream
	{
	public:
		// An abstract interface through which binary can be written.
		// Also compatible with AbstractTextOutputStream for convenience.
		virtual void WriteBinary( const void *sourceData, size_t sizeBytes ) = 0;
	};

	class DummyTextLog: public AbstractTextOutputStream
	{
	public:
		// Sinks all messages
		virtual void Write( const char *message ) override;
	};








	class AbstractConsoleOutputStream: public AbstractTextOutputStream
	{
	public:

		// An abstract colour text console for output ONLY (no interactivity).

		//      bits[7..4] = Background colour (0-15)
		//      bits[3..0] = Foreground colour (0-15)
		virtual void SetColourAttributes( uint8_t colourAttributes ) = 0;
		virtual uint8_t GetColourAttributes() = 0;

		virtual void UpdateScreen() = 0;                                      // show updates (some consoles may buffer updates)
		virtual void GetCursor( int32_t *out_x, int32_t *out_y ) = 0;         // get cursor (character) position
		virtual void GetDimensions( int32_t *out_x, int32_t *out_y ) = 0;     // get console dimensions (characters)
		virtual void CLS() = 0;                                               // clear screen *without* doing UpdateScreen()!

	};


	struct Keystroke;

	class AbstractConsoleInputStream
	{
	public:

		AbstractConsoleInputStream();

		virtual bool GetKeyAsyncStateBySimonCode( uint8_t simonCode ) = 0;      // request key async state
		virtual void WaitKey( Keystroke *out_ks ) = 0;                          // wait indefinately for key press (calls OnIdle event if set)

		// Events
		void SetOnIdle( AbstractEvent *pEvent ) { m_pOnIdleEvent = pEvent; }

	protected:

		AbstractEvent *m_pOnIdleEvent;

	};

}



