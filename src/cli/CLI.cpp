#include "cli/CLI.h"
#include <atomic>

namespace CLI
{
    void Spinner(const std::atomic<bool> &is_processing)
    {
        const char spinner_chars[] = {'|', '/', '-', '\\'};
        int index = 0;

        while (is_processing.load())
        {
            // cout << "\rProcessing Query: " << spinner_chars[index] << flush;
            index = (index + 1) % 4;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        cout << "\r " << flush; // Clear spinner after processing
    }

    void ProcessQuery(MutationData &mutationData)
    {
        std::cout << "AS" << std::endl;
        Mutator mutator;
        mutator.Mutate(mutationData);
    }
} // namespace CLI
