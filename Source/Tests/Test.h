/*
  ==============================================================================

    Test.h
    Created: 3 Feb 2026 9:30:53pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

class MainComponent;
class SoundEngine;

class Test {
public:
    Test(MainComponent& mainComponentRef, SoundEngine&)
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