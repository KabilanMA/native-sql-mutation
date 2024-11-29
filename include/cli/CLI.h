#ifndef CLI_H
#define CLI_H

#include <string>
#include "core/Mutator.h"

namespace CLI
{
    void Spinner(const std::atomic<bool> &is_processing);
    void ProcessQuery(MutationData &mutationData);

};

#endif