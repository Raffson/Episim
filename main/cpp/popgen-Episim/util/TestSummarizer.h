//
// Created by Nishchal Shrestha on 20/05/2018.
//

#ifndef STRIDE_TESTSUMMARIZER_H
#define STRIDE_TESTSUMMARIZER_H

#include <boost/filesystem.hpp>
#include <string>
#include <fstream>

namespace stride {

class TestSummarizer {

public:

    /// Generates a summary of all tests.
    /// @param testPath: Path to location of tests.
    /// @param fname: Name of outputfile.
    void GenerateHtml(const boost::filesystem::path &testPath, const std::string &fname);
private:
    /// Generate header for the summary file.
    /// @param file: Name of outputfile.
    void CreateTopTags(std::ofstream& file);
};

}//namespace stride
#endif //STRIDE_TESTSUMMARIZER_H
