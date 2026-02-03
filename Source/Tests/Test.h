/*
  ==============================================================================

    Test.h
    Created: 3 Feb 2026 9:30:53pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "../MainComponent.h"

class Test {
public:
    Test(MainComponent& mainComponentRef, SoundEngine& soundEngineRef)
        : mainComponent(mainComponentRef) {
    }

    virtual ~Test() = default;
    virtual void displayInfo() = 0;
    virtual void startTest() = 0;
    virtual void exitTest() = 0;

    virtual void onTestFinish() = 0;

protected:
    MainComponent& mainComponent;
};