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
    mTemplate = Vector(beats.size());
    for(int idx = 0; idx < beats.size(); idx++) {
      mTemplate(idx) = boost::lexical_cast<float>(beats[idx]);
    }
    addInputSlotAndUUID(SlotFeatures, UUID_FeaturesDecoderMessage);

    mMaxRMSE = configPt->get<float>("max_rmse", "max RMSE");
    mMaxSpeedup = configPt->get<float>("max_speedup", "max speedup");
    mInsideEvent = false;
    mTotalFrameCount = 0;

    std::list<std::string> requiredOutputSlots;
    initOutputs(requiredOutputSlots);
}

bool SAAHDetectComponent::isEvent(Vector x) {
  auto& y = mTemplate;
  int N = x.size();
  float S_x = x.sum();
  float S_y = y.sum();
  float S_xy = x.dot(y);
  float S_y2 = y.dot(y);
  float C = (S_x*S_y-N*S_xy)/(S_y*S_y-N*S_y2);
  float D = (C*S_y-S_x)/N;
  Vector y_tilde = C*y.array()-D;
  float rmse = 0;
  for(int x_idx = 0; x_idx < x.size(); x_idx++) {
    float x_val = x[x_idx];
    rmse += (x_val-y_tilde[x_idx])*(x_val-y_tilde[x_idx]);
  }
  rmse /= N;
  rmse = sqrt(rmse);
  if (rmse > mMaxRMSE) return false;
  float speedup = (y[-1]-y[0])/(x[-1]-x[0]);
  if ((speedup > mMaxSpeedup) || (speedup < 1/mMaxSpeedup))  return false;
  return true;
}

void SAAHDetectComponent::ProcessMessage(const DecoderMessageBlock& msgBlock) {
    auto convStateMsg = msgBlock.get<ConversationStateDecoderMessage>(SlotConversationState);
    auto featsMsg = msgBlock.get<FeaturesDecoderMessage>(SlotFeatures);
    const Vector feats = featsMsg->mFeatures;

    double avg_bpf = (mAvgBPM/60.0)*mFrameStepSizeMs;
    for(int data_idx = 0; data_idx < feats.size(); data_idx++) {
      mTotalFrameCount++;
      float data_val = feats(data_idx);
      if (data_val > mImpulseThreshold) {
        if (!mInsideEvent) {
          mAccumEvents.conservativeResize(mAccumEvents.size()+1);
          mAccumEvents(mAccumEvents.size()-1) = (mTotalFrameCount-1)*avg_bpf;
        }
        mInsideEvent = true;
      } else {
        mInsideEvent = false;
      }
    }
    
    while(mAccumEvents.size() >= mTemplate.size()) {
      if (isEvent(mAccumEvents.head(mTemplate.size()))) {
        std::cout << "Found event!" << std::endl;
      }
      mAccumEvents = mAccumEvents.tail(mAccumEvents.size()-1);
    }
}
