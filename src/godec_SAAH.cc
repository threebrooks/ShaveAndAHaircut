#include "SAAHDetect.h"

/*
 * This code is the "interface" code for Godec when it loaded the shared library containing the components. Use this as a template for creating your own shared library
 */

namespace Godec {

extern "C" {
    // When Godec loaded the shared library, this function gets called to instantiate a component.
    JNIEXPORT LoopProcessor* GodecGetComponent(std::string compString, std::string id, ComponentGraphConfig* configPt) {
        if (compString == "SAAHDetect") return SAAHDetectComponent::make(id,configPt);
        GODEC_ERR << "Godec SAAH library: Asked for unknown component " << compString;
        return NULL;
    }
    // Output for "godec list <library>"
    JNIEXPORT void GodecListComponents() {
        std::cout << "SAAHDetect: " << SAAHDetectComponent::describeThyself() << std::endl;
    }

    // For checking whether all libraries are of the same version
    JNIEXPORT std::string GodecVersion() {
        return GODEC_VERSION_STRING;
    }

    // For messages that you want to shuttle across the JNI layer. If you have no custom messages, still define it but just return nullptr
    JNIEXPORT DecoderMessage_ptr GodecJNIToMsg(JNIEnv* env, jobject jMsg) {
        return nullptr;
    }

    // The Python version of the same. Also return nullptr if you don't have a custom message
    JNIEXPORT DecoderMessage_ptr GodecPythonToMsg(PyObject* pMsg) {
        return nullptr;
    }

}

}
