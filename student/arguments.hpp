/*!
 * @file
 * @brief This file contains class for command line parsing.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <ArgumentViewer/ArgumentViewer.h>
#include <iostream>
#include <string>

/**
 * @brief This class parses command line arguments
 */
class Arguments{
  public:
    /**
     * @brief Constructor
     *
     * @param argc number of arguments
     * @param argv[] arguments
     */
    Arguments(int argc,char*argv[]){
      args = std::make_shared<argumentViewer::ArgumentViewer>(argc,argv);
      windowSize = args->geti32v("--window-size",{500,500},"size of the window");
      runPerformanceTests = args->isPresent("-p","runs performance tests");
      runConformanceTests = args->isPresent("-c","runs conformance tests");
      takeScreenShot      = args->isPresent("-s","takes screenshot of app");
      method              = args->getu32   ("-m",0,"selects a rendering method");
      groundTruthFile     = args->gets     ("-g","../tests/output.bmp","specify groundTruth image");
      perfTests           = args->getu32   ("-f",10,"number of frames that are tests during performance tests");

      auto printHelp  = args->isPresent("-h"    ,"prints help");
      printHelp |= args->isPresent("--help","prints help");

      if(printHelp || !args->validate()){
        std::cerr << args->toStr() << std::endl;
        stop = true;
      }

    }
  std::shared_ptr<argumentViewer::ArgumentViewer>args;///< argument viewer
  std::vector<int32_t>windowSize;///< window size
  std::string groundTruthFile = "../tests/output.bmp";///< ground truth file
  uint32_t method = 0;///< start with this method
  bool runPerformanceTests;///< should we run performance tests
  bool runConformanceTests;///< sould we run conformance tests
  bool takeScreenShot;///< should we take a screnshot
  bool stop = false; ///< should we immediately stop
  uint32_t perfTests; ///< number of frames in performance tests
};

