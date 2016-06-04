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
public:
	// Inherited via IFrameworkView
	virtual void Initialize(CoreApplicationView^ AppView) {}
	virtual void SetWindow(CoreWindow^ wnd) {}
	virtual void Load(Platform::String ^entryPoint) {}
	virtual void Run() {}
	virtual void Uninitialize() {}
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