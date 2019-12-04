#include "pch.h"

#include "Engine.h"

using namespace Platform;

using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;

ref class Main sealed : public IFrameworkView
{
	public:

		virtual void Run()
		{
			const int CRASH_ERROR_CODE = Engine::Boot(1080, 1920);
			{
				//UNSUPPORTED PROCESS.
			};
		}

		void Closed(CoreWindow^ sender, CoreWindowEventArgs^ args)
		{
			Engine::Stop();
		}

		virtual void Initialize(CoreApplicationView^ AppView)
		{
			AppView->Activated += ref new TypedEventHandler <CoreApplicationView^, IActivatedEventArgs^>(this, &Main::OnActivated);

			CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &Main::Suspending);
			CoreApplication::Resuming += ref new EventHandler<Object^>(this, &Main::Resuming);
		}

		virtual void SetWindow(CoreWindow^ Window)
		{
			Window->Closed += ref new TypedEventHandler
				<CoreWindow^, CoreWindowEventArgs^>(this, &Main::Closed);

			::Windows::UI::Input::PointerVisualizationSettings ^ h_visualization_settings
			{ 
				::Windows::UI::Input::PointerVisualizationSettings::GetForCurrentView() 
			};

			h_visualization_settings->IsContactFeedbackEnabled = false;
			h_visualization_settings->IsBarrelButtonFeedbackEnabled = false;
		
			::Windows::UI::ViewManagement::ApplicationView ^ h_view
			{
				::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()
			};

			h_view->FullScreenSystemOverlayMode = ::Windows::UI::ViewManagement::FullScreenSystemOverlayMode::Minimal;
		}

		void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args)
		{
			CoreWindow^ Window = CoreWindow::GetForCurrentThread();
			Window->Activate();
		}

		virtual void Load(String^ EntryPoint) { }
		virtual void Uninitialize() { }

		void Resuming(Object^ Sender, Object^ Args) { }
		void Suspending(Object^ Sender, SuspendingEventArgs^ Args) { }
};

// the class definition that creates an instance of our core framework class
ref class AppSource sealed : IFrameworkViewSource 
{ 
	public: virtual IFrameworkView^ CreateView() 
	{ 
		return ref new Main(); 
	}
};

[MTAThread]
int main(Array<String^>^ args)
{
	CoreApplication::Run(ref new AppSource());    // create and run a new AppSource class
	return 0;
}