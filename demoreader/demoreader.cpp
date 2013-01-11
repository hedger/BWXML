// demoreader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "BWReader.h"
#include "BWWriter.h"

#include <boost/property_tree/xml_parser.hpp>

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
namespace bpo = boost::program_options;

#include <vector>
#include <algorithm>

void convert(std::string src, std::string dest, bool doPack)
{
	//std::cout << src << " -> " << dest << std::endl;
	static auto settings = boost::property_tree::xml_writer_make_settings('\t', 1);
	boost::property_tree::write_xml(dest, BWXMLReader(src).toPtree(), std::locale(), settings);
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
			return s1.substr(0, i);
	}
	return s1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//std::string PP = "d:\\out\\system\\data\\speedtree.xml"; //destructibles graphics_settings speedtree.xml
	//BWXMLWriter w(PP);
	//w.saveTo(PP+".o");
	//boost::property_tree::write_xml(PP+".o.xml", BWXMLReader(PP+".o").toPtree(), std::locale(), boost::property_tree::xml_writer_make_settings('\t', 1));

	//return 0;
	//int encryptionKey = 0;
	bpo::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("pack", bpo::value<bool>()->default_value(false), "pack files instead of unpacking")
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

	bool doPack = vm["pack"].as<bool>();

	auto inputPaths = vm["input"].as< std::vector<std::string> >();
	std::string srcfile = ""; //ret.options[0].value[0];
	//ret.description;

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
				std::cout << "Path '" << *it << "' not found or not accessible, skipping" << std::endl;
			if (is_directory(current))
				std::copy(recursive_directory_iterator(current), recursive_directory_iterator(), back_inserter(paths));
			if (is_regular_file(current))
				paths.push_back(current);
		}

		std::cout << "filtering... ";

		std::copy_if(paths.begin(), paths.end(), std::back_inserter(valid_paths), [](const path& p){return is_regular_file(p);});

		std::cout << "done. \nFound " << valid_paths.size() << " files, processing to " << destdir << std::endl;

		if (valid_paths.empty())
		{
			std::cout << "Nothing to do!" << std::endl;
			return 2;
		}

		std::string commonPrefix =  FindCommonPrefix(valid_paths);
		if (commonPrefix.empty())
			commonPrefix = path(valid_paths[0]).parent_path().string();

		for (auto it=valid_paths.begin(); it!=valid_paths.end(); ++it)
		{
			std::string rel_path = it->string().substr(commonPrefix.length());
			std::cout << rel_path << " : ";
			std::string target_path = destdir + rel_path;
			
			path _target_path = path(target_path).parent_path();
			if (!exists(_target_path))
				create_directories(_target_path);

			try
			{
				convert(it->string(), target_path, doPack);
			}
			catch (std::exception e)
			{
				std::cerr << "ERROR: " << e.what() << std::endl;
			}
		}
	}	 

	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return 0;
}