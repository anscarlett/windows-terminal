// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "VtInputThread.hpp"
#include "PtySignalInputThread.hpp"

class ConsoleArguments;

namespace Microsoft::Console::VirtualTerminal
{
    class VtIo
    {
    public:
        VtIo();

        [[nodiscard]] HRESULT Initialize(const ConsoleArguments* const pArgs);

        [[nodiscard]] HRESULT CreateAndStartSignalThread() noexcept;
        [[nodiscard]] HRESULT CreateIoHandlers() noexcept;

        bool IsUsingVt() const;

        [[nodiscard]] HRESULT StartIfNeeded();

        [[nodiscard]] HRESULT SetCursorPosition(const til::point coordCursor);
        void SendCloseEvent();

        void CloseInput();
        void CloseOutput();

        [[nodiscard]] HRESULT ManuallyClearScrollback() const noexcept;
        [[nodiscard]] HRESULT RequestMouseMode(bool enable) const noexcept;

        void CreatePseudoWindow();
        void SetWindowVisibility(bool showOrHide) noexcept;

        void Write(const std::wstring_view& str);

    private:
        // After CreateIoHandlers is called, these will be invalid.
        wil::unique_hfile _hInput;
        wil::unique_hfile _hOutput;
        // After CreateAndStartSignalThread is called, this will be invalid.
        wil::unique_hfile _hSignal;

        bool _initialized;

        bool _lookingForCursorPosition;

        bool _closeEventSent{ false };

        std::unique_ptr<Microsoft::Console::VtInputThread> _pVtInputThread;
        std::unique_ptr<Microsoft::Console::PtySignalInputThread> _pPtySignalInputThread;

        [[nodiscard]] HRESULT _Initialize(const HANDLE InHandle, const HANDLE OutHandle, _In_opt_ const HANDLE SignalHandle);

#ifdef UNIT_TESTING
        friend class VtIoTests;
#endif
    };
}
