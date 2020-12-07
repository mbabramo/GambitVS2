//
// This file is part of Gambit
// Copyright (c) 1994-2016, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/gt/nfgipa.cc
// Gambit frontend to Gametracer IPA
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

//#include <unistd.h>
//#include <getopt.h>
#include <iostream>
#include <fstream>
#include <cerrno>
#include "gambit/gambit.h"
#include "gambit/nash/ipa.h"

using namespace Gambit;
using namespace Gambit::Nash;
using namespace Gambit::gametracer;


void PrintBanner(std::ostream &p_stream)
{
  p_stream << "Compute Nash equilibria using iterated polymatrix approximation\n";
  p_stream << "Gametracer version 0.2, Copyright (C) 2002, Ben Blum and Christian Shelton\n";
  p_stream << "Gambit version " VERSION ", Copyright (C) 1994-2016, The Gambit Project\n";
  p_stream << "This is free software, distributed under the GNU GPL\n\n";
}

void PrintHelp(char *progname)
{
  PrintBanner(std::cerr);
  std::cerr << "Usage: " << progname << " [OPTIONS] [file]\n";
  std::cerr << "If file is not specified, attempts to read game from standard input.\n";

  std::cerr << "Options:\n";
  std::cerr << "  -d DECIMALS      show equilibria as floating point with DECIMALS digits\n";
  std::cerr << "  -h, --help       print this help message\n";
  std::cerr << "  -q               quiet mode (suppresses banner)\n";
  std::cerr << "  -V, --verbose    verbose mode (shows intermediate output)\n";
  std::cerr << "  -v, --version    print version information\n";
  exit(1);
}


int main(int argc, char *argv[])
{
  bool quiet = false, verbose = false;
  int numDecimals = 6;

  int long_opt_index = 0;
  int optind = argc - 1;
  for (int i = 1; i < argc; i++)
  {
      if (argv[i][0] != '-')
          continue;
      const char* optarg = argv[i + 1];
      char optopt = argv[i][1];
      switch (optopt) {
      case 'v':
      PrintBanner(std::cerr); exit(1);
    case 'q':
      quiet = true;
      break;
    case 'V':
      verbose = true;
      break;
    case 'd':
      numDecimals = atoi(optarg);
      break;
    case 'S':
      break;
    case 'h':
      PrintHelp(argv[0]);
      break;
    case '?':
      if (isprint(optopt)) {
	std::cerr << argv[0] << ": Unknown option `-" << ((char) optopt) << "'.\n";
      }
      else {
	std::cerr << argv[0] << ": Unknown option character `\\x" << optopt << "`.\n";
      }
      return 1;
    default:
      abort();
    }
  }

  if (!quiet) {
    PrintBanner(std::cerr);
  }

  std::istream* input_stream = &std::cin;
  std::ifstream file_stream;
  if (optind < argc) {
    file_stream.open(argv[optind]);
    if (!file_stream.is_open()) {
      std::ostringstream error_message;
      error_message << argv[0] << ": " << argv[optind];
      perror(error_message.str().c_str());
      exit(1);
    }
    input_stream = &file_stream;
  }

  try {
    Game game = ReadGame(*input_stream);
    shared_ptr<StrategyProfileRenderer<double> > renderer;
    renderer = new MixedStrategyCSVRenderer<double>(std::cout,
						    numDecimals);
    NashIPAStrategySolver solver(renderer);
    solver.Solve(game);
    return 0;
  }
  catch (std::runtime_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
