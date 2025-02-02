/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <rigidbody/sequencer/LoopElementCallback.h>
#include <rigidbody/sequencer/LoopElement.h>
#include <rigidbody/sequencer/BodySelectElement.h>
#include <rigidbody/sequencer/TransformElement.h>
#include <rigidbody/sequencer/ParameterElement.h>

using namespace rigidbody::sequencer;

LoopElementCallback::LoopElementCallback(LoopElement* caller) : caller(caller) {}

LoopElementCallback::~LoopElementCallback() = default;

LoopElement& LoopElementCallback::loop(unsigned int repeats) {
    return caller->loop(repeats);
}

ParameterElement& LoopElementCallback::parameter_strategy(settings::rigidbody::ParameterGenerationStrategyChoice strategy) {
    return caller->parameter_strategy(strategy);
}

BodySelectElement& LoopElementCallback::body_select_strategy(settings::rigidbody::BodySelectStrategyChoice strategy) {
    return caller->body_select_strategy(strategy);
}

TransformElement& LoopElementCallback::transform_strategy(settings::rigidbody::TransformationStrategyChoice strategy) {
    return caller->transform_strategy(strategy);
}

void LoopElementCallback::execute() {
    caller->execute();
}

LoopElement& LoopElementCallback::end() {
    return caller->end();
}