// Mandatory - all Windows dependencies - pre-compiled header
#include "pch.h"

/* Notes 

	For x86 systems (only): 
	
	All your classes must be 16-bytes aligned in the heap when using DirectXMath.
	For this, do not forget to declare your class as such and override new & delete operators for every class.

	__declspec(align(16)) class MyClass {
		public:
		
		DirectX::XMMATRIX           m_projectionMatrix;

		virtual ~MyClass() {
		}

		void* operator new(size_t i) {
			return _mm_malloc(i,16);
		}

		void operator delete(void* p) {
			_mm_free(p);
		}
	};
*/

// Add required namespaces to simplify code - manually
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;
using namespace Application;

ref class App sealed : public IFrameworkView {

private:

	Ticker^ g_Ticker;
	MyApp* g_MainApplication;

protected:

	property bool IsWindowClosed;

	

public:

	// Inherited via IFrameworkView
	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView ^AppView)
	{
		/* Register Window's event handlers */
		AppView->Activated += ref new TypedEventHandler		// pass OnActivate function address as the handler for the window's creation
															// to the Activated event handler - match its parameters with the generic <T,Ts>
			<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

		/* Application's state - CoreApplication::<event> handlers */
		CoreApplication::Suspending += ref new EventHandler
			<SuspendingEventArgs^>(this, &App::OnSuspending);

		CoreApplication::Resuming += ref new EventHandler
			<Object^>(this, &App::OnResuming);

		CoreApplication::Exiting += ref new EventHandler
			<Object^>(this, &App::OnExiting);
	}

	virtual void SetWindow(Windows::UI::Core::CoreWindow ^window)
	{

		/* Windows event handlers */

		window->Closed += ref new TypedEventHandler
			<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

		window->SizeChanged += ref new TypedEventHandler
			<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowResized);

		window->VisibilityChanged += ref new TypedEventHandler
			<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

		/* IO events handlers */

		window->KeyDown += ref new TypedEventHandler
			<CoreWindow^, KeyEventArgs^>(this, &App::OnKeyDown);

		window->PointerMoved += ref new TypedEventHandler
			<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerMoved);

		window->PointerPressed += ref new TypedEventHandler
			<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerPressed);

		window->PointerReleased += ref new TypedEventHandler
			<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerReleased);

		window->PointerWheelChanged += ref new TypedEventHandler
			<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerWheelChanged);
	}
	
	// Instantiate the application here
	virtual void Load(Platform::String ^entryPoint) { 
		try {
			g_MainApplication = new MyApp();
			g_Ticker = ref new Ticker(Application::m_GlobalFPS);
		} catch (Exception^ e) {
			MessageDialog(e->Message);
		}
	}

	virtual void Run();

	virtual void Uninitialize() {}

	/* Application - top level app's state handlers */
	
	void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args) {
		CoreWindow^ coreWnd = CoreWindow::GetForCurrentThread();
		coreWnd->Activate();
	}

	void OnSuspending(Object^ pSender, SuspendingEventArgs^ args) {
		g_MainApplication->SetPause(true);
		g_MainApplication->Terminate();
	}

	void OnResuming(Object^ pSender, Object^ args) {
		g_MainApplication->SetPause(false);
	}

	void OnExiting(Object^ pSender, Object^ args) { }

	void OnWindowClosed(CoreWindow^ pWnd, CoreWindowEventArgs^ args) {
		this->IsWindowClosed = true;
	}

	void OnWindowResized(CoreWindow^ pWnd, WindowSizeChangedEventArgs^ args) {
		// never let the buffers go below the minimum regardless of the window size
		g_MainApplication->Resize(
			args->Size.Width > m_MinWidth	? static_cast<UINT>(args->Size.Width)	: m_MinWidth,
			args->Size.Height > m_MinHeight ? static_cast<UINT>(args->Size.Height)	: m_MinHeight);
	}

	void OnVisibilityChanged(CoreWindow^ wnd, VisibilityChangedEventArgs^ args) {
		bool pause = false;
		if (!wnd->Visible) pause = true;
		g_MainApplication->SetPause(pause);
	}

	/* IO Handlers */

	void OnKeyDown(CoreWindow^ wnd, KeyEventArgs^ args) {
		// we will handle keystrokes asynchronously within the applications control loop
	}

	/* Identity the event, centralize and send from SendPointerEvent */

	void OnPointerMoved(CoreWindow^ wnd, PointerEventArgs^ args) {
	}

	void OnPointerPressed(CoreWindow^ wnd, PointerEventArgs^ args) {
	}

	void OnPointerReleased(CoreWindow^ wnd, PointerEventArgs^ args) {
	}

	void OnPointerWheelChanged(CoreWindow^ wnd, PointerEventArgs^ args) {
	}

};

ref class AppCreator sealed : IFrameworkViewSource {
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView ^ CreateView() {
		return ref new App();
	}
};

[MTAThread]
int main(Array<String^>^ args) {
	CoreApplication::Run(ref new AppCreator());
	return 0;
}

void App::Run() {
	LOGMESSAGE(L"App::Run\n");
	while (!(this->IsWindowClosed)) {

		/* Different types of Application's events processing
		ProcessUntilQuit <- is a blocking infinite loop until the Closed event is triggered. This is fully event drive rather than giving control to loop.
		alternative > coreWnd->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit); << infinite loop, not good.
		We want to be in control of the processing loop, so instead, we will use ProcessAllIfPresent
		*/
		CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
		
		if(!g_MainApplication->IsPaused())
			g_MainApplication->Update(g_Ticker);
		
	}
}