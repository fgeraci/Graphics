// Mandatory - all Windows dependencies - pre-compiled header
#include "pch.h"

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

ref class App sealed : public IFrameworkView {
private:

	CoreWindow^ m_CoreWindow;

protected:
	
	property CoreWindow^ MainWindow {
		CoreWindow^ get() {
			return this->m_CoreWindow;
		}
	}

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

		/* Set up Properties */
		this->m_CoreWindow = window;

		/* Windows event handlers */
		window->Closed += ref new TypedEventHandler
			<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

		window->SizeChanged += ref new TypedEventHandler
			<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowResized);

		/* IO events handlers */

	}

	virtual void Load(Platform::String ^entryPoint) { }

	virtual void Run();

	virtual void Uninitialize() {}

	/* Application - top level app's state handlers */
	
	void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args) {
		CoreWindow^ coreWnd = CoreWindow::GetForCurrentThread();
		coreWnd->Activate();
	}

	void OnSuspending(Object^ pSender, SuspendingEventArgs^ args) {
		
	}
	void OnResuming(Object^ pSender, Object^ args) {
		
	}
	void OnExiting(Object^ pSender, Object^ args) {
		
	}

	void OnWindowClosed(CoreWindow^ pWnd, CoreWindowEventArgs^ args) {
		this->IsWindowClosed = true;
	}

	void OnWindowResized(CoreWindow^ pWnd, WindowSizeChangedEventArgs^ args) {
		UINT width = args->Size.Width;
		UINT height = args->Size.Height;
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
	while (!(this->IsWindowClosed)) {

		/* Different types of Application's events processing
		ProcessUntilQuit <- is a blocking infinite loop until the Closed event is triggered. This is fully event drive rather than giving control to loop.
		alternative > coreWnd->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit); << infinite loop, not good.
		We want to be in control of the processing loop, so instead, we will use ProcessAllIfPresent
		*/
		this->MainWindow->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

	}
}