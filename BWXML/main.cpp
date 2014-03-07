/*
Copyright 2013-2014 hedger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "BWReader.h"
#include "BWWriter.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <boost/thread.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
namespace bpo = boost::program_options;

#include <vector>
#include <algorithm>

void convert(std::string src, std::string dest, bool doPack)
{
	//std::cout << src << " -> " << dest << std::endl;
	if (doPack)
		BWPack::BWXMLWriter(src).saveTo(dest);
	else
		BWPack::BWXMLReader(src).saveTo(dest);
}

std::string FindCommonPrefix(const std::vector<path>& paths)
{
	std::string s1, s2;
	if (paths.size() < 2)
		return "";
	auto lenCmp = [](const path& p1, const path& p2){return p1.string().length() < p2.string().length(); };
	s1 = (*std::min_element(paths.begin(), paths.end(), lenCmp)).string();
	s2 = (*std::max_element(paths.begin(), paths.end(), lenCmp)).string();

	for (size_t i=0; i<s1.length(); ++i)
	{
		if (s1[i] != s2[i])
		{
			s1 = s1.substr(0, i);
			break;
		}
	}

	path s1p(s1);
	if (!exists(s1p))
		s1 = s1p.parent_path().string();
	return s1;
}

int main(int argc, char* argv[])
{
	std::cout << "BWXML v1.03 by hedger" << std::endl;

	bpo::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("pack", "pack files instead of unpacking")
#ifndef _DEBUG
		("verbose", "print information about each file")
#endif
		("selftest", "perform reversed operation on produced files")
		("threads", bpo::value<int>()->default_value(boost::thread::hardware_concurrency() + 1), "sets the size of a worker pool. Default = n_cpu_cores + 1")
		//("key", bpo::value<int>(&encryptionKey)->default_value(10), "encryption key")
		("input", bpo::value< std::vector<std::string> >(), "input files/directories")
		("output", bpo::value< std::string >()->default_value("decrypted/"), "directory to output files")
		;

	bpo::positional_options_description po;
	po.add("input", -1);

	bpo::variables_map vm;
	try
	{
		bpo::store(bpo::command_line_parser(argc, argv).
			options(desc).positional(po).run(), vm);
		bpo::notify(vm);
	}
	catch (const std::exception&)
	{
		// ignoring arg parsing errors
	}

	if (vm.count("help") || vm["input"].empty()) {
		std::cout << "Usage: " << argv[0] << " [options] list_of_files_or_directories\n";
		std::cout << desc;
		return 0;
	}

	bool doPack = (vm.count("pack") != 0);
#ifndef _DEBUG
	bool verbose = (vm.count("verbose") != 0);
#else
	bool verbose = true;
#endif
	bool selfTest = (vm.count("selftest") != 0);

	auto inputPaths = vm["input"].as< std::vector<std::string> >();
	std::string srcfile = ""; 

	std::string destdir = vm["output"].as<std::string>();
	destdir.append("/");

	std::vector<path>	paths, valid_paths;
	std::cout << "Collecting files... " ;
	try
	{
		for (auto it=inputPaths.begin(); it != inputPaths.end(); ++it)
		{
			path current = path(*it);
			if (!exists(current))
				std::cout << "Path '" << *it << "' is not found or not accessible, skipping" << std::endl;
			if (is_directory(current))
				std::copy(recursive_directory_iterator(current, symlink_option::recurse), 
				recursive_directory_iterator(), back_inserter(paths));
			if (is_regular_file(current))
				paths.push_back(current);
		}

		std::cout << "filtering... ";

		std::copy_if(paths.begin(), paths.end(), std::back_inserter(valid_paths),
			[](const path& p){return is_regular_file(p);});

		std::cout << "done. \nFound " << valid_paths.size() << " file(s), processing to " << destdir << std::endl;

		if (valid_paths.empty())
		{
			std::cout << "Nothing to do!" << std::endl;
			return 2;
		}


		std::string commonPrefix =	FindCommonPrefix(valid_paths);
		if (commonPrefix.empty())
			commonPrefix = path(valid_paths[0]).parent_path().string();

		int nThreads = vm["threads"].as<int>();
		std::cout << "Starting a pool with " << nThreads << " workers" << std::endl;

		auto workerThread = [&](int num)
		{
			for (size_t i=num; i < valid_paths.size(); i += nThreads)
			{
				std::string rel_path = valid_paths.at(i).string().substr(commonPrefix.length());
				if (verbose)
					std::cout << rel_path << " : ";
				std::string target_path = destdir + rel_path;

				path _target_path = path(target_path);
				_target_path = _target_path.parent_path();
				if (!exists(_target_path))
					create_directories(_target_path);

				try
				{
					convert(valid_paths.at(i).string(), target_path, doPack);
					if (selfTest)
						convert(target_path, target_path+".test", !doPack);
					std::cout << "+";
				}
				catch (const std::exception& e)
				{
					if (verbose)
						std::cout << "ERROR: " << e.what();
					else
						std::cout << "!";
				}
				if (verbose)
					std::cerr << std::endl;
			}
		};

		boost::thread_group pool;
		for (int i=0; i<nThreads; ++i)
			pool.create_thread(boost::bind<void>(workerThread, i));

		pool.join_all();
	}

	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return -1;
	}

	std::cout << std::endl << "Done." << std::endl;
	return 0;
}