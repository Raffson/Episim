//
// Created by Nishchal Shrestha on 20/05/2018.
//

#include <iostream>
#include <boost/optional/optional.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "TestSummarizer.h"

using namespace std;

namespace stride{

void TestSummarizer::CreateTopTags(ofstream& file)
{
    file << "<html> \n<head> <meta charset=\"UTF-8\" /> ";
    file << R"( <meta name="viewport" content="width=device-width, initial-scale=1">)";
    file << R"(<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css">)";
    file << "<style>#wrapper{padding: 40px;} </style> </head>" << endl;
    file << R"(<body><div id="wrapper" class="table-responsive">)";

}

void TestSummarizer::GenerateHtml(const boost::filesystem::path &testPath, const string &fname)
{
    boost::property_tree::ptree pTree;

    if(boost::filesystem::exists(testPath)){

        boost::property_tree::read_xml(testPath.string(), pTree);

        ofstream file;
        file.open(fname, ofstream::out);
        CreateTopTags(file);
        file << "<h2 style=\"text-align:center;\">"<< pTree.get<std::string>("testsuites.<xmlattr>.name", "") << " with "
             << pTree.get<std::string>("testsuites.<xmlattr>.tests", "") << " testcases. "
             << pTree.get<std::string>("testsuites.<xmlattr>.failures", "") << " failing tests. Generated at "
             << pTree.get<std::string>("testsuites.<xmlattr>.timestamp", "") << "</h4>";

        for(auto& node: pTree.get_child("testsuites")){
            auto subtree = node.second;
            string testsuiteName = subtree.get<std::string>("<xmlattr>.name", "");
            //int nr_failures = subtree.get<int>("<xmlattr>.failures", 0);

            if(testsuiteName.empty())
                continue;

            file << "<h3>" << testsuiteName << "</h3>" << endl;
            file << "<table class=\"table table-striped\">\n<tr><th>Testcases' name</th><th>Time</th><th>Status</th></tr>\n";


            for(auto& tc: subtree.get_child("")){
                string testcaseName = tc.second.get<std::string>("<xmlattr>.name", "");

                if(testcaseName.empty())
                    continue;

                file << "<tr><td>" << testcaseName << "</td>";
                file << "<td>" << tc.second.get<std::string>("<xmlattr>.time", "") << "</td>";

                boost::optional< boost::property_tree::ptree& > failures = tc.second.get_child_optional( "failure" );

                file << "<td>";
                if(failures){
                    file << "Failed";

                    for(auto& f:tc.second.get_child("failure") ){
                        string msg = f.second.get<std::string>("<xmlattr>.message", "");
                        file << "\n" << msg;
                    }
                }
                else{
                    file << "Passed";
                }
                file << "</td></tr>" << endl;
            }

            file << "</table>" << endl;

        }
        file << "</div> </body></html>" << endl;
        file.close();
    }

    else{
        cerr << testPath << " doesn't exist. Please run a test-executable with flag --gtest_output=xml:"
             <<testPath  << " and try again." <<endl;
    }


}

}//namespace stride