#ifndef CUKE_GTESTDRIVER_HPP_
#define CUKE_GTESTDRIVER_HPP_

#include "../step/StepManager.hpp"

#include <iostream>

namespace cucumber {
namespace internal {

class GTestStep : public BasicStep {
public:
    virtual ~GTestStep();
protected:
    const InvokeResult invokeStepBody();

private:
    void initGTest();
    void initFlags();

protected:
    static bool initialized;
};

inline GTestStep::~GTestStep()
{
}

#define STEP_INHERITANCE(step_name) ::cucumber::internal::GTestStep

}
}

#endif /* CUKE_GTESTDRIVER_HPP_ */
