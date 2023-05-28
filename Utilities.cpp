#include "Utilities.h"


Utilities* Utilities::m_pInstance = nullptr;

Utilities* Utilities::getInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new Utilities();
	}

	return m_pInstance;
};

Utilities::Utilities() {};


std::string Utilities::lastClassPrinted = "";
std::string Utilities::lastMessagePrinted = "";


std::string Utilities::generateTimestamp_HH_MM_SS_mmm()
{
	using namespace std::chrono;

	// get current time
	auto now = system_clock::now();

	// get number of milliseconds for the current second
	// (remainder after division into seconds)
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

	// convert to std::time_t in order to convert to std::tm (broken time)
	auto timer = system_clock::to_time_t(now);

	// convert to broken time
#pragma warning(suppress : 4996)
	std::tm bt = *std::localtime(&timer);

	std::ostringstream oss;

	oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
	oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

	return oss.str();
};



std::vector<char> Utilities::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	};

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
};



// TODO: Implement this
/*
void Utilities::compileShaders(std::filesystem::path folderPath)
{
	
	// Get the .vert and .frag files in the folder
	std::filesystem::path vertShader = "";
	std::filesystem::path fragShader = "";

	for (const auto& file : folderPath)
	{
		std::cout << file.filename() << "\n";
		if (file.filename() == "shader.vert")
		{
			vertShader = file;
		}
	};
};
*/