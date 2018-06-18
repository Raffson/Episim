//
// Created by robbe on 03.06.18.
//
#include "stdlib.h"

#ifdef USING_QT
#include "gui/QTBackEnd.h"
#endif

#include <boost/property_tree/xml_parser.hpp>
#include "boost/property_tree/ptree.hpp"
using namespace boost::property_tree;


int main(){
    ptree pt;
    read_xml("config/run_short.xml",pt);
    system("rm -r *_*");

#ifdef USING_QT

    stride::CliController ctrl(pt);
    ctrl.ControlGui();
#endif
}