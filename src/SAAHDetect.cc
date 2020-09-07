#include "SAAHDetect.h"
#include "godec/ComponentGraph.h"
#include <boost/format.hpp>

SAAHDetectComponent::~SAAHDetectComponent() {
}

LoopProcessor* SAAHDetectComponent::make(std::string id, ComponentGraphConfig* configPt) {
    return new SAAHDetectComponent(id, configPt);
}
std::string SAAHDetectComponent::describeThyself() {
    return "Sums incoming features";
}
SAAHDetectComponent::SAAHDetectComponent(std::string id, ComponentGraphConfig* configPt) :
    LoopProcessor(id,configPt) {

    mImpulseThreshold = configPt->get<float>("impulse_threshold", "Threshold for detecting impulse");
    mFrameStepSizeMs = configPt->get<float>("frame_step_size_ms", "Frame step size in ms");
    mAvgBPM = configPt->get<float>("expected_bpm", "Expected BPM of the pattern");
    std::string templateString = configPt->get<std::string>("pattern_template", "csv of pattern"); 
    std::vector<std::string> beats;
    boost::split(beats, templateString, boost::is_any_of(","));
    for(auto beat : beats) {
      mTemplate.push_back(boost::lexical_cast<float>(beat));
    }
    addInputSlotAndUUID(SlotFeatures, UUID_FeaturesDecoderMessage);

    mMaxRMSE = configPt->get<float>("max_rmse", "max RMSE");
    mMaxSpeedup = configPt->get<float>("max_speedup", "max speedup");

    std::list<std::string> requiredOutputSlots;
    initOutputs(requiredOutputSlots);
}

void SAAHDetectComponent::ProcessMessage(const DecoderMessageBlock& msgBlock) {
    auto convStateMsg = msgBlock.get<ConversationStateDecoderMessage>(SlotConversationState);
    auto featsMsg = msgBlock.get<FeaturesDecoderMessage>(SlotFeatures);
    const Matrix& feats = featsMsg->mFeatures;
}
