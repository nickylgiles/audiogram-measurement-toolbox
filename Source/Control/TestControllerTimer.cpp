/*
  ==============================================================================

    TestControllerTimer.cpp
    Created: 2 Feb 2026 11:08:43pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "TestControllerTimer.h"

TestControllerTimer::TestControllerTimer(std::function<void()> callbackFunction) 
    : callback(callbackFunction)
{}

void TestControllerTimer::timerCallback() {
    stopTimer(); // Timer does not repeat
    if (callback) {
        callback();
    }
}

void TestControllerTimer::setCallback(std::function<void()> newCallback)  {
    callback = newCallback;
}
