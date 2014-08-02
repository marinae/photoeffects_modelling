#include <iostream>
using namespace std;
#include "arg_parser.h"
#include "../Engine/CEngine.h"

int main(int argc, char *argv[])
{
	CEngine Engine;
	typedef pair<char, double> command;
	string sSourcePath, sResultPath, sErrors;
	vector< command > vCommands;

	const Arg_parser::Option Options[] =
	{
		{ 'x', "grayscale",   Arg_parser::no    },
		{ 'g', "gaussian",    Arg_parser::yes    }
		// TODO: place your options here
	};

	const Arg_parser ArgParser( argc, argv, Options );
	if( ArgParser.error().size())
	{
		cerr<<"Command line error: "<<ArgParser.error()<<endl;
		return -1;
	}

	for (uint k = 0; k < ArgParser.arguments(); ++k)
	{
		int iCode = ArgParser.code(k);

		if (iCode != 0)
		{
			vCommands.push_back(command((char)iCode, atof(ArgParser.argument(k).data())));
		}
		else
		{
			if (sSourcePath.empty())
				sSourcePath = ArgParser.argument(k);
			else if (sResultPath.empty())
				sResultPath = ArgParser.argument(k);
			else
			{
				cerr<<"invalid unnamed command-line argument"<<endl;
				return -1;
			}
		}
	}

	if (sSourcePath.empty())
		sErrors += "\n* Source path was not specified.";
	if (sResultPath.empty())
		sErrors += "\n* Result path was not specified.";
	if (!Engine.load(sSourcePath))
		sSourcePath += "\n* Can't load source image";

	if (!sErrors.empty())
	{
		cerr<<"Some errors were found:";
		cerr<<sErrors<<endl;
		return -1;
	}

	for (uint i = 0; i < vCommands.size(); ++i)
	{
		switch (vCommands[i].first)
		{
		case 'x':
			Engine.toGrayscale(false);
			break;
		case 'g':
			cout<<"Gaussian; sigma = "<<vCommands[i].second<<endl;
			break;
		// TODO: place your handlers here
		default:
			cout<<"Unknown option: "<<vCommands[i].first<<"; its argument: "<<vCommands[i].second<<endl;
			break;
		}
	}

	if (!Engine.save(sResultPath))
	{
		cerr<<"Can't save output file"<<endl;
		return -1;
	}

	return 0;
}


/*
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	return a.exec();
}
*/