//
// Created by Nishchal Shrestha on 20/05/2018.
//

#include <iostream>
#include <boost/optional/optional.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "TestSummarizer.h"
#include "util/FileSys.h"

using namespace std;

namespace stride{

void TestSummarizer::CreateTopTags(ofstream& file)
{
    file << "<html> \n<head> <meta charset=\"UTF-8\" /> ";
    file << " <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    file << "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
    file << "<style>#wrapper{padding: 40px;} </style> </head>" << endl;
    file << "<body><div id=\"wrapper\" class=\"table-responsive\">";

}

void TestSummarizer::GenerateHtml(const boost::filesystem::path &test_path, const string &fname)
{
    boost::property_tree::ptree p_tree;

    if(boost::filesystem::exists(test_path)){

        boost::property_tree::read_xml(test_path.string(), p_tree);

        ofstream file;
        file.open(fname, ofstream::out);
        CreateTopTags(file);
        file << "<h2 style=\"text-align:center;\">"<< p_tree.get<std::string>("testsuites.<xmlattr>.name", "") << " with "
             << p_tree.get<std::string>("testsuites.<xmlattr>.tests", "") << " testcases. "
             << p_tree.get<std::string>("testsuites.<xmlattr>.failures", "") << " failing tests. Generated at "
             << p_tree.get<std::string>("testsuites.<xmlattr>.timestamp", "") << "</h4>";

        for(auto& node: p_tree.get_child("testsuites")){
            auto subtree = node.second;
            string testsuite_name = subtree.get<std::string>("<xmlattr>.name", "");
            //int nr_failures = subtree.get<int>("<xmlattr>.failures", 0);

            if(testsuite_name == "")
                continue;

            file << "<h3>" << testsuite_name << "</h3>" << endl;
            file << "<table class=\"table table-striped\">\n<tr><th>Testcases' name</th><th>Time</th><th>Status</th></tr>\n";


            for(auto& tc: subtree.get_child("")){
                string testcase_name = tc.second.get<std::string>("<xmlattr>.name", "");

                if(testcase_name == "")
                    continue;

                file << "<tr><td>" << testcase_name << "</td>";
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
        cerr << test_path << " doesn't exist. Please run a test-executable with flag --gtest_output=xml:"
             <<test_path  << " and try again." <<endl;
    }


}

}//namespace stride