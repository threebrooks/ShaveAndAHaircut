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
    bool isEvent(Vector x);
    float mImpulseThreshold;
    float mFrameStepSizeMs;
    float mAvgBPM;
    Vector mTemplate;
    float mMaxRMSE;
    float mMaxSpeedup;
    float mMaxImpulseLengthMs;

    std::string mCommand;
    bool mDeviceOn;

    Vector mAccumEvents;
    int64_t mTotalFrameCount;
    int64_t mImpulseBeginIdx;

    bool mInsideEvent;
    float mInsideMaxIdx;
    float mInsideMaxEnergy;
};
