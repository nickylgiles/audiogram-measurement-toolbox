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
        : mainComponent(mainComponentRef), soundEngine(soundEngineRef) {
    }

    virtual ~Test() = default;
    virtual void displayInfo() = 0;
    virtual void startTest() = 0;
    virtual void stopTest() = 0;

protected:
    MainComponent& mainComponent;
    SoundEngine& soundEngine;
};