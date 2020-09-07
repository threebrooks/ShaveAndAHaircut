#pragma once

#include "godec/ChannelMessenger.h"
#include "godec/core_components/GodecMessages.h"

using namespace Godec;

class SAAHDetectComponent : public LoopProcessor {
  public:
    static LoopProcessor* make(std::string id, ComponentGraphConfig* configPt);
    static std::string describeThyself();
    SAAHDetectComponent(std::string id, ComponentGraphConfig* configPt);
    ~SAAHDetectComponent();

  private:
    void ProcessMessage(const DecoderMessageBlock& msgBlock);
    float mImpulseThreshold;
    float mFrameStepSizeMs;
    float mAvgBPM;
    std::vector<float> mTemplate;
    float mMaxRMSE;
    float mMaxSpeedup;
};
