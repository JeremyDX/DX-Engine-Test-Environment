// Include the precompiled headers
#include "pch.h"

#include "Engine.h"
#include "GameTime.h"

// Use some common namespaces to simplify the code
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

using namespace Windows::UI::ViewManagement;


// the class definition for the core "framework" of our app
ref class Main sealed : public IFrameworkView
{
	private:
		bool WindowClosed;
		Engine engine;

	public:
	// some functions called by Windows
		virtual void Initialize(CoreApplicationView^ AppView)
		{

			AppView->Activated += ref new TypedEventHandler
				<CoreApplicationView^, IActivatedEventArgs^>(this, &Main::OnActivated);
			CoreApplication::Suspending +=
				ref new EventHandler<SuspendingEventArgs^>(this, &Main::Suspending);
			CoreApplication::Resuming +=
				ref new EventHandler<Object^>(this, &Main::Resuming);
			
		}

		virtual void SetWindow(CoreWindow^ Window)
		{
			Window->Closed += ref new TypedEventHandler
				<CoreWindow^, CoreWindowEventArgs^>(this, &Main::Closed);

			::Windows::UI::Input::PointerVisualizationSettings ^ h_visualization_settings{ ::Windows::UI::Input::PointerVisualizationSettings::GetForCurrentView() };
			h_visualization_settings->IsContactFeedbackEnabled = false;
			h_visualization_settings->IsBarrelButtonFeedbackEnabled = false;
			::Windows::UI::ViewManagement::ApplicationView ^ h_view { 
				::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView() 
			};
			h_view->FullScreenSystemOverlayMode = ::Windows::UI::ViewManagement::FullScreenSystemOverlayMode::Minimal;
		}

		virtual void Load(String^ EntryPoint) {}

		void Closed(CoreWindow^ sender, CoreWindowEventArgs^ args)
		{
			WindowClosed = true;
		}

		void Suspending(Object^ Sender, SuspendingEventArgs^ Args) {}
		void Resuming(Object^ Sender, Object^ Args) {}

		virtual void Run()
		{
			// Obtain a pointer to the window
			CoreWindow^ Window = CoreWindow::GetForCurrentThread();

			ScreenManager::UpdatePreferredCanvasSize(1080, 1920);

			engine.Initialize();

			while (!WindowClosed)
			{
				Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
				engine.Update();
				engine.Render();
				GameTime::Tick();
			}
		}

		virtual void Uninitialize() {}

		// an "event" that is called when the application window is ready to be activated
		void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args)
		{
			CoreWindow^ Window = CoreWindow::GetForCurrentThread();
			Window->Activate();
		}
};


// the class definition that creates an instance of our core framework class
ref class AppSource sealed : IFrameworkViewSource
{
	public:
		virtual IFrameworkView^ CreateView()
		{
			return ref new Main();    // create an App class and return it
		}
};

[MTAThread] 
int main(Array<String^>^ args)
{
	CoreApplication::Run(ref new AppSource());    // create and run a new AppSource class
	return 0;
}