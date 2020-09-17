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
    mMaxImpulseLengthMs = configPt->get<float>("max_impulse_length_ms", "Maximum allowed length for an impulse");
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
    mInsideMaxIdx = -1;
    mInsideMaxEnergy = -FLT_MAX;
    mTotalFrameCount = 0;

    std::list<std::string> requiredOutputSlots;
    requiredOutputSlots.push_back(SlotFeatures);
    initOutputs(requiredOutputSlots);
}

bool SAAHDetectComponent::isEvent(Vector x) {
  std::cout << x.transpose() << std::endl;
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
    float y_val = y_tilde[x_idx];
    rmse += (x_val-y_val)*(x_val-y_val);
  }
  rmse /= N;
  rmse = sqrt(rmse);
  float speedup = (y[N-1]-y[0])/(x[N-1]-x[0]);
  std::cout << "rmse: " << rmse << " speedup: " << speedup << std::endl;
  if (rmse > mMaxRMSE) return false;
  if ((speedup > mMaxSpeedup) || (speedup < 1/mMaxSpeedup))  return false;
  return true;
}

void SAAHDetectComponent::ProcessMessage(const DecoderMessageBlock& msgBlock) {
    auto convStateMsg = msgBlock.get<ConversationStateDecoderMessage>(SlotConversationState);
    auto featsMsg = msgBlock.get<FeaturesDecoderMessage>(SlotFeatures);
    const Vector feats = featsMsg->mFeatures.row(0);

    double avg_bpf = (mAvgBPM/60.0)*(mFrameStepSizeMs/1000.0);
    for(int data_idx = 0; data_idx < feats.size(); data_idx++) {
      mTotalFrameCount++;
      float data_val = feats(data_idx);
      if (data_val > mImpulseThreshold) {
        if (!mInsideEvent) mImpulseBeginIdx = mTotalFrameCount;
        mInsideEvent = true;
        if (data_val > mInsideMaxEnergy) {
          mInsideMaxEnergy = data_val;
          mInsideMaxIdx = mTotalFrameCount;
        }
     } else if (data_val < 0.5*mImpulseThreshold) {
        if (mInsideEvent) {
          if ((mTotalFrameCount-mImpulseBeginIdx) < mMaxImpulseLengthMs/mFrameStepSizeMs) {
            mAccumEvents.conservativeResize(mAccumEvents.size()+1);
            mAccumEvents(mAccumEvents.size()-1) = (mInsideMaxIdx-1)*avg_bpf;
            //std::cout << "Added event at " << mAccumEvents(mAccumEvents.size()-1) << ", energy " << mInsideMaxEnergy << std::endl; 
          } else {
            std::cerr << "Too long, dropped. " << (mTotalFrameCount-mImpulseBeginIdx) << " vs " << (mMaxImpulseLengthMs/mFrameStepSizeMs) << std::endl;
          }
        }
        mInsideMaxEnergy = -FLT_MAX;
        mInsideEvent = false;
      }
    }
    
    while(mAccumEvents.size() >= mTemplate.size()) {
      if (isEvent(mAccumEvents.head(mTemplate.size()))) {
        std::cout << "Found event!" << std::endl;
        Matrix outMatrix(1,1);
        outMatrix(0,0) = 0.0;
        std::vector<uint64_t> outTimings;
        outTimings.push_back(convStateMsg->getTime());
        pushToOutputs(SlotFeatures, FeaturesDecoderMessage::create(
                      convStateMsg->getTime(), convStateMsg->mUtteranceId,
                      outMatrix, "dummy", outTimings));
      }
      mAccumEvents = mAccumEvents.tail(mAccumEvents.size()-1);
    }
}
