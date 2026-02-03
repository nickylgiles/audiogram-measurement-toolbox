/*
  ==============================================================================

    PureToneTest.h
    Created: 3 Feb 2026 9:31:18pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "Test.h"

class PureToneTest : public Test {
public:
    PureToneTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef);
    void displayInfo() override;
    void startTest() override;
    void stopTest() override;
private:
    
};