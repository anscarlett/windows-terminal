---
author: Leonard Hecker @lhecker
created on: 2024-05-21
issue id: 13000
---

# In-process ConPTY

## tl;dr

Why?
* ConPTY runs out-of-process which fundamentally leads to out-of-sync issues with the terminal.
* ConPTY and in turn Windows Terminal are not fully compatible with all Console APIs as some don't have a VT equivalent.
* ConPTY is layered on top of conhost with significant boundary decay over the years. This makes maintenance difficult and adding features time consuming.

How?
1. Get rid of the `VtEngine` class which "renders" the console buffer into VT. Instead, algorithmically translate console API calls directly to VT.
2. Move all console API related code from the `Host` to the `Server` project. Narrow the `IApiRoutines` interface down to only the few things `Server` needs to know (= Write text, Read text, Get viewport size, Get cursor position, etc.). Replace singletons with instances while doing so.
3. Make `Server` a standalone library with `IApiRoutines` as its primary callback interface. Integrate the library in Windows Terminal.
4. (After API stabilization:) Ship the `Server` library as part of Windows and offer it as an official API. Build conhost itself on top of this API to demonstrate its capabilities.

## Why?

* ConPTY runs out-of-process, outside the hosting terminal, which leads to an entirely unsolvable issue:
The buffer contents between ConPTY and the terminal can go out of sync.
  * The terminal and ConPTY may implement escape sequences differently.
  * ...may implement text processing differently.
  * ...may implement text reflow (resize) differently.
  * Resizing the terminal and ConPTY is asynchronous and there may be concurrent text output.
  * ...and it may uncover text from the scrollback, which ConPTY doesn't know about.
* Since ConPTY communicates with the hosting terminal exclusively via escape sequences, it fails to cover all console API methods. The most basic example of this is the lack of LVB gridlines in VT.
* As the prototype that ConPTY initially was it has fulfilled our needs a thousand times over, but it being just another layer on top of conhost is in effect basic software decay: ConPTY's architecture is difficult to maintain and nigh impossible to debug. The layer boundary with existing conhost code has slowly blurred over the years which has negatively affected dozens if not hundreds of existing places. Its performance is poor and subject to much debate. It's now on us to finally pay our debt, undo the blur, and remove a layer of the onion again: ConPTY should be its own proper component that both conhost and Windows Terminal are built on top of.

## Step 1: Remove VtEngine

## Step 2: Move Console API implementations to Server

## Step 3: Productize Server

## Step 4: Ship Server in Windows
