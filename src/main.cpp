#include <core/core.h>
#include <core/api.h>
#include <core/parser.h>
#include <glog/logging.h>

int main(int argc, char *argv[])
{
    using namespace pbrt;

    google::InitGoogleLogging(argv[0]);
    FLAGS_stderrthreshold = 1; // Warning and above.

#ifndef NDEBUG
    LOG(INFO) << "Running debug build";
    printf("*** DEBUG BUILD ***\n");
#endif // !NDEBUG
    fflush(stdout);

    Options options;
    pbrtInit(options);
    std::vector<std::string> filenames;
    for (int i = 1; i < argc; i++)
        filenames.push_back(argv[i]);
    for (const std::string &f : filenames)
        pbrtParseFile(f);
    pbrtCleanup();

	return 0;
}
