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

    int64_t mTotalFrameCount;

    Vector mAccumEvents;
    std::vector<uint64_t> mAccumEventTimestamps;
    Vector mAccumGapEnergies;
    int64_t mAccumGapEnergyCount;

    bool mInsideEvent;
    int64_t mInsideMaxIdx;
    uint64_t mInsideMaxTimestamp;
    float mInsideMaxEnergy;
};
