// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "TasksPaneContent.h"
#include "PaneArgs.h"
#include "TasksPaneContent.g.cpp"
#include "TaskViewModel.g.cpp"
#include "FilteredTask.g.cpp"

using namespace winrt::Windows::Foundation;
using namespace winrt::Microsoft::Terminal::Settings;
using namespace winrt::Microsoft::Terminal::Settings::Model;

namespace winrt
{
    namespace WUX = Windows::UI::Xaml;
    namespace MUX = Microsoft::UI::Xaml;
    using IInspectable = Windows::Foundation::IInspectable;
}

namespace winrt::TerminalApp::implementation
{
    TasksPaneContent::TasksPaneContent()
    {
        InitializeComponent();

        auto res = Windows::UI::Xaml::Application::Current().Resources();
        auto bg = res.Lookup(winrt::box_value(L"UnfocusedBorderBrush"));
        Background(bg.try_as<WUX::Media::Brush>());
    }

    void TasksPaneContent::_updateFilteredCommands()
    {
        // const auto& queryString = _filterBox().Text();

        // You'd think that `FilterToSendInput(queryString)` would work. It
        // doesn't! That uses the queryString as the current command the user
        // has typed, then relies on the sxnui to _also_ filter with that
        // string.

        // In the fullness of time, we'll actually want to filter things here.
        // Probably with something that re-uses the suggestions control list
        //
        // huh. now that's a thought.

        const auto tasks = _settings.GlobalSettings().ActionMap().FilterToSendInput(L""); // IVector<Model::Command>
        auto itemSource = winrt::single_threaded_observable_vector<TerminalApp::FilteredTask>();
        for (const auto& t : tasks)
        {
            itemSource.Append(winrt::make<FilteredTask>(t));
        }

        _treeView().ItemsSource(itemSource);
    }

    void TasksPaneContent::UpdateSettings(const CascadiaSettings& settings)
    {
        _settings = settings;
        _updateFilteredCommands();
    }

    void TasksPaneContent::_filterTextChanged(const IInspectable& /*sender*/,
                                              const Windows::UI::Xaml::RoutedEventArgs& /*args*/)
    {
        _updateFilteredCommands();
    }

    winrt::Windows::UI::Xaml::FrameworkElement TasksPaneContent::GetRoot()
    {
        return *this;
    }
    winrt::Windows::Foundation::Size TasksPaneContent::MinSize()
    {
        return { 1, 1 };
    }
    void TasksPaneContent::Focus(winrt::Windows::UI::Xaml::FocusState reason)
    {
        reason;
        // _box.Focus(reason);
    }
    void TasksPaneContent::Close()
    {
        CloseRequested.raise(*this, nullptr);
    }

    NewTerminalArgs TasksPaneContent::GetNewTerminalArgs(const bool /* asContent */) const
    {
        return nullptr;
    }

    winrt::hstring TasksPaneContent::Icon() const
    {
        static constexpr std::wstring_view glyph{ L"\xe70b" }; // QuickNote
        return winrt::hstring{ glyph };
    }

    winrt::Windows::UI::Xaml::Media::Brush TasksPaneContent::BackgroundBrush()
    {
        return Background();
    }

    void TasksPaneContent::SetLastActiveControl(const Microsoft::Terminal::Control::TermControl& control)
    {
        _control = control;
    }

    void TasksPaneContent::_runCommandButtonClicked(const Windows::Foundation::IInspectable& sender,
                                                    const Windows::UI::Xaml::RoutedEventArgs&)
    {
        if (const auto& taskVM{ sender.try_as<WUX::Controls::Button>().DataContext().try_as<TaskViewModel>() })
        {
            if (const auto& strongControl{ _control.get() })
            {
                // By using the last active control as the sender here, the
                // actiopn dispatch will send this to the active control,
                // thinking that it is the control that requested this event.
                DispatchCommandRequested.raise(strongControl, taskVM->Command());
            }
        }
    }

}