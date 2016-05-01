#include <iostream>
#include <fstream>

using namespace std;

struct Blob
{
	char* m_Data;
	unsigned int m_Size;
};

size_t GetDirectoryStringPos(const string& a_Path)
{
	// Get last forward slash or backward slash
	size_t fwSlash = a_Path.find_last_of('/');
	size_t bwSlash = a_Path.find_last_of('\\');
	size_t lastSlash = fwSlash > bwSlash ? fwSlash : bwSlash;
	return lastSlash != a_Path.npos ? lastSlash : 0;
}

void ReplaceDavExtension(string& a_FileName)
{
	size_t lCase = a_FileName.find_last_of('d');
	lCase = lCase != a_FileName.npos ? lCase : 0;
	size_t uCase = a_FileName.find_last_of('D');
	uCase = uCase != a_FileName.npos ? uCase : 0;
	size_t lastCase = lCase > uCase ? lCase : uCase;
	a_FileName[lastCase] = 'w';
}

Blob ReadDavFile(string a_Path)
{
	Blob davBlob = {};
	ifstream davFile(a_Path, ios::in | ios::binary | ios::ate);
	if (davFile.is_open())
	{
		davBlob.m_Size = (unsigned int)davFile.tellg();
		davBlob.m_Data = new char[davBlob.m_Size];
		davFile.seekg(0, ios::beg);
		davFile.read(davBlob.m_Data, davBlob.m_Size);
		davFile.close();
	}
	else
	{
		throw 2;
	}
	return davBlob;
}

void ConvertDavToWav(Blob& a_DavBlob)
{
	for (unsigned int i = 0; i < a_DavBlob.m_Size; ++i)
	{
		a_DavBlob.m_Data[i] = a_DavBlob.m_Data[i] ^ 0x7B;
	}
}

void WriteWavFile(const string& a_FileName, Blob& a_WavBlob)
{
	ofstream wavFile(a_FileName, ios::out | ios::binary);
	if (wavFile.is_open())
	{
		wavFile.write(a_WavBlob.m_Data, a_WavBlob.m_Size);
		wavFile.close();
	}
	else
	{
		delete[] a_WavBlob.m_Data;
		throw 3;
	}
}

int main(int argc, char* argv[])
{
	cout << "WavToDav audio file converter" << endl << "Version 1.0" << endl << "Created by Jasper de Laat" << endl;
	if (argc > 1)
	{
		string fullPath = argv[1];
		size_t lastSlash = GetDirectoryStringPos(fullPath);
		string directory = fullPath.substr(0, lastSlash);
		string filename = fullPath.substr(lastSlash, string::npos);
		string wavFilename = filename;
		ReplaceDavExtension(wavFilename);
		try
		{
			Blob davBlob(ReadDavFile(fullPath));
			ConvertDavToWav(davBlob);
			WriteWavFile(wavFilename, davBlob);
			delete[] davBlob.m_Data;
		}
		catch (int e)
		{
			if (e == 2)
			{
				cout << "DAV file was not found!" << endl;
			}
			else if (e == 3)
			{
				cout << "WAV file could not be created!" << endl;
			}
			return e;
		}
	}
	else
	{
		cout << "No file parameter was given!" << endl;
		return 1;
	}
	cout << "File conversion success!" << endl;
	return 0;
}