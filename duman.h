#ifndef DUMAN_H
#define DUMAN_H

#include <iostream>
#include <conio.h>
#include <sapi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <WinInet.h>
#include <random>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <experimental/filesystem>
#include <Windows.h>
#include <shellapi.h>
#include <functional>
#include <future>
#include <cassert>
#include <iterator>
#include <sstream>
#include <fstream>
#pragma comment(lib, "Wininet")
#define VARIABLE_NAME(variable) (string)#variable
namespace fs = std::experimental::filesystem;

void replace_str(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

bool path_file_exists(TCHAR* file)
{
	WIN32_FIND_DATA find_file_data;
	const auto handle = FindFirstFile(file, &find_file_data);
	const auto found = handle != INVALID_HANDLE_VALUE;
	if (found)
		FindClose(handle);
	return found;
}

int response_code(const std::string& url, const bool display_text = true)
{
	const auto h_open = InternetOpenA("DumanSTUDIOS", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
	const auto h_file = InternetOpenUrlA(h_open, url.c_str(), nullptr, 0,INTERNET_FLAG_RELOAD, 0);
	char response_text[256];
	DWORD response_text_size = sizeof(response_text);

	if(!HttpQueryInfoA(h_file, HTTP_QUERY_STATUS_CODE, &response_text, &response_text_size, nullptr))
	{
		if(display_text)
			std::cout << "Remote server is offline or unreachable.\n";
		InternetCloseHandle(h_open);
		InternetCloseHandle(h_file);
		return -1;
	}

	if(display_text)
	{
		std::cout << url << "\t" << "Response: " << response_text << " ";
		// 100s
		if(static_cast<std::string>(response_text) == "100") { std::cout << "Continue"; }
		else if(static_cast<std::string>(response_text) == "101") { std::cout << "Switching Protocols"; }
		else if(static_cast<std::string>(response_text) == "102") { std::cout << "Processing"; }
		// 200s
		else if(static_cast<std::string>(response_text) == "200") { std::cout << "OK"; }
		else if(static_cast<std::string>(response_text) == "201") { std::cout << "Created"; }
		else if(static_cast<std::string>(response_text) == "202") { std::cout << "Accepted"; }
		else if(static_cast<std::string>(response_text) == "203") { std::cout << "Non-Authoritative Information"; }
		else if(static_cast<std::string>(response_text) == "204") { std::cout << "No Content"; }
		else if(static_cast<std::string>(response_text) == "205") { std::cout << "Reset Content"; }
		else if(static_cast<std::string>(response_text) == "206") { std::cout << "Partial Content"; }
		else if(static_cast<std::string>(response_text) == "207") { std::cout << "Multi-Status"; }
		else if(static_cast<std::string>(response_text) == "208") { std::cout << "Already Reported"; }
		else if(static_cast<std::string>(response_text) == "226") { std::cout << "IM Used"; }
		// 300s
		else if(static_cast<std::string>(response_text) == "300") { std::cout << "Multiple Choices"; }
		else if(static_cast<std::string>(response_text) == "301") { std::cout << "Moved Permanently"; }
		else if(static_cast<std::string>(response_text) == "302") { std::cout << "Found"; }
		else if(static_cast<std::string>(response_text) == "303") { std::cout << "See Other"; }
		else if(static_cast<std::string>(response_text) == "304") { std::cout << "Not Modified"; }
		else if(static_cast<std::string>(response_text) == "305") { std::cout << "Use Proxy"; }
		else if(static_cast<std::string>(response_text) == "306") { std::cout << "Switch Proxy"; }
		else if(static_cast<std::string>(response_text) == "307") { std::cout << "Temporary Redirect"; }
		else if(static_cast<std::string>(response_text) == "308") { std::cout << "Permanent Redirect"; }
		// 400s
		else if(static_cast<std::string>(response_text) == "400") { std::cout << "Bad Request"; }
		else if(static_cast<std::string>(response_text) == "401") { std::cout << "Unauthorized"; }
		else if(static_cast<std::string>(response_text) == "402") { std::cout << "Payment Required"; }
		else if(static_cast<std::string>(response_text) == "403") { std::cout << "Forbidden"; }
		else if(static_cast<std::string>(response_text) == "404") { std::cout << "Not Found"; }
		else if(static_cast<std::string>(response_text) == "405") { std::cout << "Method Not Allowed"; }
		else if(static_cast<std::string>(response_text) == "406") { std::cout << "Not Acceptable"; }
		else if(static_cast<std::string>(response_text) == "407") { std::cout << "Proxy Authentication Required"; }
		else if(static_cast<std::string>(response_text) == "408") { std::cout << "Request Timeout"; }
		else if(static_cast<std::string>(response_text) == "409") { std::cout << "Conflict"; }
		else if(static_cast<std::string>(response_text) == "410") { std::cout << "Gone"; }
		else if(static_cast<std::string>(response_text) == "411") { std::cout << "Length Required"; }
		else if(static_cast<std::string>(response_text) == "412") { std::cout << "Precondition Failed"; }
		else if(static_cast<std::string>(response_text) == "413") { std::cout << "Request Entity Too Large"; }
		else if(static_cast<std::string>(response_text) == "414") { std::cout << "Request-URI Too Long"; }
		else if(static_cast<std::string>(response_text) == "415") { std::cout << "Unsupported Media Type"; }
		else if(static_cast<std::string>(response_text) == "416") { std::cout << "Requested Range Not Satisfiable"; }
		else if(static_cast<std::string>(response_text) == "417") { std::cout << "Expectation Failed"; }
		else if(static_cast<std::string>(response_text) == "418") { std::cout << "I\'m a teapot"; } // :)
		else if(static_cast<std::string>(response_text) == "419") { std::cout << "Authentication Timeout"; }
		else if(static_cast<std::string>(response_text) == "420") { std::cout << "Method Failure"; }
		else if(static_cast<std::string>(response_text) == "421") { std::cout << "Misdirected Request"; }
		else if(static_cast<std::string>(response_text) == "422") { std::cout << "Unprocessable Entity"; }
		else if(static_cast<std::string>(response_text) == "423") { std::cout << "Locked"; }
		else if(static_cast<std::string>(response_text) == "424") { std::cout << "Failed Dependency"; }
		else if(static_cast<std::string>(response_text) == "426") { std::cout << "Upgrade Required"; }
		else if(static_cast<std::string>(response_text) == "428") { std::cout << "Precondition Required"; }
		else if(static_cast<std::string>(response_text) == "429") { std::cout << "Too Many Requests"; }
		else if(static_cast<std::string>(response_text) == "431") { std::cout << "Request Header Fields Too Large"; }
		else if(static_cast<std::string>(response_text) == "440") { std::cout << "Login Timeout"; }
		else if(static_cast<std::string>(response_text) == "444") { std::cout << "No Response"; }
		else if(static_cast<std::string>(response_text) == "449") { std::cout << "Retry With"; }
		else if(static_cast<std::string>(response_text) == "450") { std::cout << "Blocked by Windows Parental Controls"; }
		else if(static_cast<std::string>(response_text) == "451") { std::cout << "Unavailable For Legal Reasons"; }
		else if(static_cast<std::string>(response_text) == "494") { std::cout << "Request Header Too Large"; }
		else if(static_cast<std::string>(response_text) == "495") { std::cout << "Cert Error"; }
		else if(static_cast<std::string>(response_text) == "496") { std::cout << "No Cert"; }
		else if(static_cast<std::string>(response_text) == "497") { std::cout << "HTTP to HTTPS"; }
		else if(static_cast<std::string>(response_text) == "498") { std::cout << "Token Expired/Invalid"; }
		else if(static_cast<std::string>(response_text) == "499") { std::cout << "Client Closed Request"; }
		// 500s
		else if(static_cast<std::string>(response_text) == "500") { std::cout << "Internal Server Error"; }
		else if(static_cast<std::string>(response_text) == "501") { std::cout << "Not Implemented"; }
		else if(static_cast<std::string>(response_text) == "502") { std::cout << "Bad Gateway"; }
		else if(static_cast<std::string>(response_text) == "503") { std::cout << "Service Unavailable"; }
		else if(static_cast<std::string>(response_text) == "504") { std::cout << "Gateway Timeout"; }
		else if(static_cast<std::string>(response_text) == "505") { std::cout << "HTTP Version Not Supported"; }
		else if(static_cast<std::string>(response_text) == "506") { std::cout << "Variant Also Negotiates"; }
		else if(static_cast<std::string>(response_text) == "507") { std::cout << "Insufficient Storage"; }
		else if(static_cast<std::string>(response_text) == "508") { std::cout << "Loop Detected"; }
		else if(static_cast<std::string>(response_text) == "509") { std::cout << "Bandwidth Limit Exceeded"; }
		else if(static_cast<std::string>(response_text) == "510") { std::cout << "Not Extended"; }
		else if(static_cast<std::string>(response_text) == "511") { std::cout << "Network Authentication Required"; }
		else if(static_cast<std::string>(response_text) == "520") { std::cout << "Unknown Error"; }
		else if(static_cast<std::string>(response_text) == "598") { std::cout << "Network Read Timeout Error"; }
		else if(static_cast<std::string>(response_text) == "599") { std::cout << "Network Connect Timeout Error"; }
		std::cout << "\n";
	}
	InternetCloseHandle(h_open);
	InternetCloseHandle(h_file);
	return strtol(response_text, nullptr, 0);
}

std::string xor_encrypt_decrypt(std::string to_encrypt, const char key) {
	auto output = to_encrypt;
    
    for (size_t i = 0; i < to_encrypt.size(); i++)
        output[i] = to_encrypt[i] ^ key;
    
    return output;
}

// Usage: timer timer_1(time_in_milliseconds, true_if_async, &function_name, argument_1, arg_2, ...); true makes it multithreaded
class timer
{
public:
    template <class Callable, class... Arguments>
    timer(int after, const bool async, Callable&& f, Arguments&&... args)
    {
	    std::function<typename std::result_of<Callable(Arguments...)>::type()> task(bind(forward<Callable>(f), forward<Arguments>(args)...));

        if (async)
        {
            thread([after, task]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
            }).detach();
        }
        else
        {
	        std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }
    }
};

template <typename T>
// Format: 'delay milliseconds, quit character, exit value'
T key_to_exit(const int delay_time_in_ms, T char_for_quit, const int exit_value, const bool show_message)
{
	if(show_message)
		std::cout << "\nPress " << char_for_quit << " to exit!\n";
	while (true)
	{
		if (_kbhit())
		{
			const T ch = _getch();
			if(ch == toupper(char_for_quit) || ch == tolower(char_for_quit))
			{
				exit(exit_value);
			}
		} Sleep(delay_time_in_ms);
	}
}

template <typename T>
// Format: 'quit character'
T key_to_exit(T char_for_quit = 'q', const bool show_message = true)
{
	if(show_message)
		std::cout << "\nPress " << char_for_quit << " to exit!\n";
	while (true)
	{
		if (_kbhit())
		{
			const T ch = _getch();
			if (ch == toupper(char_for_quit) || ch == tolower(char_for_quit))
			{
				exit(0);
			}
		} Sleep(200);
	}
}

// Format: Just call it.
void esc_to_exit(const bool show_message = true)
{
	if (show_message)
		std::cout << "\nPress ESC to exit!";
	while (true)
	{
		if (_kbhit())
		{
			const char ch = _getch();
			if(ch == 27)
			{
				exit(0);
			}
		} Sleep(200);
	}
}

// Format: 'any_to_exit(0, true);'
void any_to_exit(const int exit_value = 0, const bool show_message = true)
{
	if (show_message)
		std::cout << "\nPress any key to exit!";
	while (true)
	{
		if (_kbhit())
			exit(exit_value);
		Sleep(200);
	}
}

// Helper for FTP connection
__int64 get_file_size(const LPCSTR name)
{
	const auto h_file = CreateFile(name, GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, nullptr);
	if (h_file == INVALID_HANDLE_VALUE)
		return -1; // error condition, could call GetLastError to find out more

	LARGE_INTEGER size;
	if (!GetFileSizeEx(h_file, &size))
	{
		CloseHandle(h_file);
		return -1; // error condition, could call GetLastError to find out more
	}

	CloseHandle(h_file);
	return size.QuadPart;
}

// Grabs file name from path.
std::string get_file_name_fs(const std::string& s) {
	const auto sep = '\\';

	const auto i = s.rfind(sep, s.length());

	if (i != std::string::npos)
		return(s.substr(i+1, s.length() - i));

	return("");
}

std::string get_file_name_bs(const std::string& s) {
	const auto sep = '/';

	const auto i = s.rfind(sep, s.length());

	if (i != std::string::npos)
		return(s.substr(i+1, s.length() - i));

	return("");
}

// Encrypts a string
std::string encrypt(std::string msg, std::string const& key)
{
    if(key.empty())
        return msg;
    
    for (std::string::size_type i = 0; i < msg.size(); ++i)
        msg[i] ^= key[i%key.size()];

    return msg;
}

// Decrypts the string, correct key needed (during encryption)
std::string decrypt(std::string const& msg, std::string const& key)
{
    return encrypt(msg, key);
}

// Creates a FTP connection and allows you to upload a single file
void upload_single_file(const LPCSTR url, const LPCSTR ftp_username, const LPCSTR ftp_password, const LPCSTR local_file_location, const LPCSTR remote_file_location)
{
	const auto file_name = fs::path(local_file_location).filename();
	const auto file_size = get_file_size(local_file_location);

	if (file_size < 0)
	{
		std::cout << "File doesn't exist or path is wrong!\n";
		std::cout << "Please check and try again.\n";
		esc_to_exit(true);
	}

	const auto h_internet = InternetOpen(nullptr, INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
	const auto start = std::chrono::steady_clock::now();
	const auto h_ftp_session = InternetConnect(h_internet, url, INTERNET_DEFAULT_FTP_PORT, ftp_username, ftp_password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);

	if (!h_ftp_session)
	{
		std::cout << "Connection to " << url << " FTP server(s) is failed!\n";
		std::cout << "Please check URL, username and password\n";
		std::cout << "variable values and the internet connection.\n\n";
		esc_to_exit(true);
	}
	else
	{
		std::cout << "Connection to " << url << " FTP server(s) is successful!\n\n";
		if(file_size < 1024)
			std::cout << "Uploading " << file_name << " (" << file_size << " B)\n";
		if(file_size >= 1024 && file_size < 1048576)
			std::cout << "Uploading " << file_name << " (" << file_size / 1024.0 << " KB)\n";
		if(file_size >= 1048576)
			std::cout << "Uploading " << file_name << " (" << file_size / 1048576.0 << " MB)\n";
		if(file_size > 1048576)
		{
			std::cout << "------------------------------------\n";
			std::cout << "It may take some time to upload this file.\n";
			std::cout << "You will get the detailed report after it's completed.\n";
		}
	}
	FtpPutFile(h_ftp_session, local_file_location, remote_file_location, FTP_TRANSFER_TYPE_BINARY, 0);
	InternetCloseHandle(h_ftp_session);
	InternetCloseHandle(h_internet);
	const auto end = std::chrono::steady_clock::now();
	const auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "\nUpload successful!\n";
	std::cout << "------------------------------------\n";
	std::cout << file_name << std::setprecision(2) << std::fixed << " @ " << static_cast<double>(file_size) / static_cast<double>(duration_in_ms) << " kb/s\n";
	std::cout << "------------------------------------\n";
	std::cout << "Total file size   : " << file_size << " bytes\n";
	if(duration_in_ms < 1000)
		std::cout << "Total time elapsed: " << duration_in_ms << " millisecond(s)\n";
	if(duration_in_ms >= 1000 && duration_in_ms < 60000)
		std::cout << "Total time elapsed: " << duration_in_ms / 1000.0 << " second(s)\n";
	if(duration_in_ms >= 60000)
		std::cout << "Total time elapsed: " << duration_in_ms / 60000.0 << " minute(s)\n";
	std::cout << "------------------------------------\n";
	std::cout << "File uploaded from: " << local_file_location << "\n";
	std::cout << "File uploaded to  : " << remote_file_location << "\n";
	std::cout << "------------------------------------\n";
}

// convert string to LPCWSTR
std::wstring s2_ws(const std::string& s)
{
	const auto slength = static_cast<int>(s.length()) + 1;
	const auto len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, nullptr, 0);
	const auto buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
    delete[] buf;
    return r;
}

// Format: 'L"Speak whatever is written here"'
bool text_to_speech(const std::string& input_text)
{
	auto stemp = s2_ws(input_text);
	const auto sentence = stemp.c_str();

	ISpVoice * p_voice = nullptr;

	if (FAILED(::CoInitialize(NULL)))
		return FALSE;

	const auto hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, reinterpret_cast<void **>(&p_voice));
	if (SUCCEEDED(hr))
	{
		p_voice->Speak(sentence, SPF_IS_XML, nullptr);
		p_voice->Release();
		p_voice = nullptr;
	}

	::CoUninitialize();
	return TRUE;
}

void open_url(const LPCSTR url)
{
	ShellExecute(nullptr, nullptr, url, nullptr, nullptr , SW_SHOW);
}

void open_url_silent(const LPCSTR url)
{
	ShellExecute(nullptr, nullptr, url, nullptr, nullptr , SW_HIDE);
}

/* Format: 'sinput('*', true) to display ****'
EXAMPLE CODE:

int main()
{
	const auto visible_password = sinput(false); // You can also just type sinput();
	const auto masked_password = sinput(true);
	cout << "Value of visible password = " << visible_password << "\n";
	cout << "Value of masked password  = " << masked_password << "\n";
	any_to_exit(0, true);
}

ninput stands for number input.
*/
std::string sinput(const char to_display = '*', const bool show_asterisk = false)
{
	const char backspace_key = 8;
	const char enter_key = 13;

	std::string sinput_field;
	unsigned char ch = 0;

	DWORD con_mode;
	DWORD dw_read;

	const auto h_in = GetStdHandle(STD_INPUT_HANDLE);

	GetConsoleMode(h_in, &con_mode);
	SetConsoleMode(h_in, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

	while (ReadConsoleA(h_in, &ch, 1, &dw_read, nullptr) && ch != enter_key)
	{
		if (ch == backspace_key)
		{
			if (sinput_field.length() != 0)
			{
				std::cout << "\b \b";
				sinput_field.resize(sinput_field.length() - 1);
			}
		}
		else
		{
			sinput_field += ch;
			if (show_asterisk)
				std::cout << to_display;
			else
				std::cout << ch;
		}
	}
	std::cout << "\n";
	return sinput_field;
}

/* Format: 'ninput(true) to display ****'
EXAMPLE CODE:

int main()
{
	const auto test = ninput(false); // You can also just type ninput();
	const auto test2 = 3;
	cout << "Value of test  = " << test << "\n";
	cout << "Value of test2 = " << test2 << "\n";
	cout << "Value of test + test2 = " << test + test2 << "\n";
	any_to_exit(0, true);
}

ninput stands for number input.
*/
double ninput(const char to_display = '*', const bool show_asterisk = false)
{
	const char backspace_key = 8;
	const char enter_key = 13;

	std::string sinput_field;
	unsigned char ch = 0;

	DWORD con_mode;
	DWORD dw_read;

	const auto h_in = GetStdHandle(STD_INPUT_HANDLE);

	GetConsoleMode(h_in, &con_mode);
	SetConsoleMode(h_in, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

	while (ReadConsoleA(h_in, &ch, 1, &dw_read, nullptr) && ch != enter_key)
	{
		if (ch == backspace_key)
		{
			if (sinput_field.length() != 0)
			{
				std::cout << "\b \b";
				sinput_field.resize(sinput_field.length() - 1);
			}
		}
		else
		{
			sinput_field += ch;
			if (show_asterisk)
				std::cout << to_display;
			else
				std::cout << ch;
		}
	}
	std::cout << "\n";
	return stod(sinput_field);
}

// Format: "string_analyzer_eng("Your string")"
void string_analyzer_eng(std::string value)
{
	auto dsa_vowels = 0;
	auto dsa_consonants = 0;
	auto dsa_digits = 0;
	auto dsa_symbols = 0;
	auto dsa_lowercase_count = 0;
	auto dsa_uppercase_count = 0;  // NOLINT(modernize-loop-convert)

	for (auto i : value)
	{
		if (i >= 48 && i <= 57)
		{
			dsa_digits++;
			continue;
		}
		if(!(i >= 65 && i <= 90) && !(i >= 97 && i <= 122) && !(i >= 48 && i <= 57))
		{
			dsa_symbols++;
			continue;
		}
		if(i >= 65 && i <= 90)
		{
			dsa_uppercase_count++;
		}
		if(i >= 97 && i <= 122)
		{
			dsa_lowercase_count++;
		}
		if (i == 'a' || i == 'e' || i == 'u' ||
			i == 'o' || i == 'i' || i == 'A' ||
			i == 'E' || i == 'U' || i == 'O' ||
			i == 'I')
		{
			dsa_vowels++;
		}
		else
			dsa_consonants++;
	}
	if(dsa_vowels + dsa_consonants != dsa_uppercase_count + dsa_lowercase_count)
	{
		std::cout << "Something went wrong while analyzing!\n";
		std::cout << "Error: 0001A, send this error code to your system administrator.\n";
		std::cout << "\nPress q to end the program.";
		key_to_exit(200, 'q', 1, false);
	} const auto dsa_letters = dsa_uppercase_count + dsa_lowercase_count;
	std::cout << "-----------------\n";
	std::cout << "Analyzed string  : " << value << "\n";
	std::cout << "-----------------\n";
	std::cout << "Vowels           : " << dsa_vowels << "\n";
	std::cout << "Consonant        : " << dsa_consonants << "\n";
	std::cout << "-----------------\n";
	std::cout << "Uppercase letters: " << dsa_uppercase_count << "\n";
	std::cout << "Lowercase letters: " << dsa_lowercase_count << "\n";
	std::cout << "-----------------\n";
	std::cout << "Letter           : " << dsa_letters << "\n";
	std::cout << "Digit            : " << dsa_digits << "\n";
	std::cout << "Special Character: " << dsa_symbols << "\n";
	std::cout << "-----------------\n";
	std::cout << "Length           : " << value.length() << "\n";
	std::cout << "-----------------\n";
}

// Format: 'ping_url("dumanstudios.com");'
void ping_url(const std::string& url)
{
	std::cout << "Pinging " << url;
	std::cout << ", please wait..." << "\n";
	auto ping_command = "ping " + url;

	system(ping_command.c_str());
}

// Format: 'binary_to_decimal(100001)' PS: This is a bit buggy on large numbers. Will fix it later on.
void binary_to_decimal(__int64 binary_number)
{
	auto decimal_value = 0;
	auto base = 1;

	while(binary_number)
	{
		const int last_digit = binary_number % 10;
		binary_number = binary_number / 10;
		decimal_value += last_digit*base;
		base = base*2;
	}
	std::cout << decimal_value << "\n";
}

void to_clipboard(const std::string& s){
	OpenClipboard(nullptr);
	EmptyClipboard();
	const auto hg=GlobalAlloc(GMEM_MOVEABLE,s.size()+1);
	if (!hg){
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg),s.c_str(),s.size()+1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT,hg);
	CloseClipboard();
	GlobalFree(hg);
}

// Format: ''
int password_score(std::string value)
{
	auto dsa_vowels = 0;
	auto dsa_consonants = 0;
	auto dsa_digits = 0;
	auto dsa_symbols = 0;
	auto dsa_lowercase_count = 0;
	auto dsa_uppercase_count = 0;

	for (auto i : value)
	{
		if (i >= 48 && i <= 57)
		{
			dsa_digits++;
			continue;
		}
		if(!(i >= 65 && i <= 90) && !(i >= 97 && i <= 122) && !(i >= 48 && i <= 57))
		{
			dsa_symbols++;
			continue;
		}
		if(i >= 65 && i <= 90)
		{
			dsa_uppercase_count++;
		}
		if(i >= 97 && i <= 122)
		{
			dsa_lowercase_count++;
		}
		if (i == 'a' || i == 'e' || i == 'u' ||
			i == 'o' || i == 'i' || i == 'A' ||
			i == 'E' || i == 'U' || i == 'O' ||
			i == 'I')
		{
			dsa_vowels++;
		}
		else
			dsa_consonants++;
	}

	const auto dsa_letters = dsa_uppercase_count + dsa_lowercase_count;
	const auto dsa_total_length = dsa_letters + dsa_digits + dsa_symbols;

	auto length_is_greater_than_eight = false;
	auto amount_of_correct_stuff = 0;

	auto dsa_total_score = 0; // 5~N0v{2a
	dsa_total_score += dsa_total_length * 4;
	if(dsa_uppercase_count > 0)
	{
		dsa_total_score += (dsa_total_length - dsa_uppercase_count) * 2;
		amount_of_correct_stuff++;
	}
	if(dsa_lowercase_count > 0)
	{
		dsa_total_score += (dsa_total_length - dsa_lowercase_count) * 2;
		amount_of_correct_stuff++;
	}
	if(dsa_digits > 0)
	{
		dsa_total_score += dsa_digits * 4;
		amount_of_correct_stuff++;
	}
	if(dsa_symbols > 0)
	{
		dsa_total_score += dsa_symbols * 6;
		amount_of_correct_stuff++;
	}
	if(dsa_total_length >= 8)
	{
		length_is_greater_than_eight = true;
		amount_of_correct_stuff++;
	}
	if(length_is_greater_than_eight)
		dsa_total_score += amount_of_correct_stuff * 2;
	if(dsa_total_score > 100)
		dsa_total_score = 100;

	return dsa_total_score;
}

// Format: 'string password = password_generator(50, true, true);'
std::string password_generator(const int length_of_password = 12, const bool enable_symbols = false, const bool copy_to_clipboard = false)
{
	std::vector<char> password;
	srand (GetTickCount64());  // NOLINT(cert-msc32-c)

	//generates lowercase letters
	for(auto c = 1; c <= length_of_password; c = c + 4)
	{
		const auto v1 = rand() % 26;
		password.push_back(v1 + 'a');
	}

	//generates uppercase letters
	for(auto g = 3; g <= length_of_password; g = g + 4)
	{
		const auto v2 = rand() % 26;
		password.push_back(v2 + 'A');
	}

	//generates numbers
	for(auto k = 0; k <= length_of_password; k = k + 2)
	{
		const auto v3 = rand() % 10;
		password.push_back(v3 + '0');
	}

	if(enable_symbols)
	{
		//generates symbols
		for(auto g = 1; g <= length_of_password; g = g + 4)
		{
			const auto choice = rand() % 3;
			
			if(choice == 0)
			{
				const auto v4 = rand() % 14;
				password.push_back(v4 + '!');
			}

			if(choice == 1)
			{
				const auto v5 = rand() % 5;
				password.push_back(v5 + '[');
			}

			if(choice == 2)
			{
				const auto v6 = rand() % 4;
				password.push_back(v6 + '{');
			}
		}
	}

	std::random_device r;
	shuffle(password.begin(), password.end(), std::default_random_engine(r()));

	std::string returning_password;

	for(auto i = 0; i < length_of_password; i++)
	{
		returning_password.push_back(password[i]);
	}

	if(copy_to_clipboard)
		to_clipboard(returning_password);

	return returning_password;
}

//sha256
#ifndef DUMANSHA256_BUFFER_SIZE_FOR_INPUT_ITERATOR
#define DUMANSHA256_BUFFER_SIZE_FOR_INPUT_ITERATOR \
    1048576  //=1024*1024: default is 1MB memory
#endif

namespace dumansha256
{
typedef unsigned long word_t;
typedef unsigned char byte_t;

static const size_t k_digest_size = 32;

namespace detail {
inline byte_t mask_8_bit(const byte_t x) { return x & 0xff; }

inline word_t mask_32_bit(const word_t x) { return x & 0xffffffff; }

const word_t add_constant[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

const word_t initial_message_digest[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372,
                                          0xa54ff53a, 0x510e527f, 0x9b05688c,
                                          0x1f83d9ab, 0x5be0cd19};

inline word_t ch(const word_t x, const word_t y, const word_t z) { return (x & y) ^ ((~x) & z); }

inline word_t maj(const word_t x, const word_t y, const word_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

inline word_t rotr(const word_t x, const size_t n) {
    assert(n < 32);
    return mask_32_bit((x >> n) | (x << (32 - n)));
}

inline word_t bsig0(const word_t x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }

inline word_t bsig1(const word_t x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }

inline word_t shr(const word_t x, const size_t n) {
    assert(n < 32);
    return x >> n;
}

inline word_t ssig0(const word_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ shr(x, 3); }

inline word_t ssig1(const word_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ shr(x, 10); }

template <typename RaIter1, typename RaIter2>
void hash256_block(RaIter1 message_digest, RaIter2 first, RaIter2 last) {
    assert(first + 64 == last);
    static_cast<void>(last);  // for avoiding unused-variable warning
    word_t w[64];
    std::fill(w, w + 64, 0);
    for (size_t i = 0; i < 16; ++i) {
        w[i] = (static_cast<word_t>(mask_8_bit(*(first + i * 4))) << 24) |
               (static_cast<word_t>(mask_8_bit(*(first + i * 4 + 1))) << 16) |
               (static_cast<word_t>(mask_8_bit(*(first + i * 4 + 2))) << 8) |
               (static_cast<word_t>(mask_8_bit(*(first + i * 4 + 3))));
    }
    for (size_t i = 16; i < 64; ++i) {
        w[i] = mask_32_bit(ssig1(w[i - 2]) + w[i - 7] + ssig0(w[i - 15]) +
                          w[i - 16]);
    }

    word_t a = *message_digest;
    word_t b = *(message_digest + 1);
    word_t c = *(message_digest + 2);
    word_t d = *(message_digest + 3);
    word_t e = *(message_digest + 4);
    word_t f = *(message_digest + 5);
    word_t g = *(message_digest + 6);
    word_t h = *(message_digest + 7);

    for (size_t i = 0; i < 64; ++i) {
	    const auto temp1 = h + bsig1(e) + ch(e, f, g) + add_constant[i] + w[i];
	    const auto temp2 = bsig0(a) + maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = mask_32_bit(d + temp1);
        d = c;
        c = b;
        b = a;
        a = mask_32_bit(temp1 + temp2);
    }
    *message_digest += a;
    *(message_digest + 1) += b;
    *(message_digest + 2) += c;
    *(message_digest + 3) += d;
    *(message_digest + 4) += e;
    *(message_digest + 5) += f;
    *(message_digest + 6) += g;
    *(message_digest + 7) += h;
    for (size_t i = 0; i < 8; ++i) {
        *(message_digest + i) = mask_32_bit(*(message_digest + i));
    }
}

}  // namespace detail

template <typename InIter>
void output_hex(InIter first, InIter last, std::ostream& os) {
    os.setf(std::ios::hex, std::ios::basefield);
    while (first != last) {
        os.width(2);
        os.fill('0');
        os << static_cast<unsigned int>(*first);
        ++first;
    }
    os.setf(std::ios::dec, std::ios::basefield);
}

template <typename InIter>
void bytes_to_hex_string(InIter first, InIter last, std::string& hex_str) {
	std::ostringstream oss;
    output_hex(first, last, oss);
    hex_str.assign(oss.str());
}

template <typename InContainer>
void bytes_to_hex_string(const InContainer& bytes, std::string& hex_str) {
    bytes_to_hex_string(bytes.begin(), bytes.end(), hex_str);
}

template <typename InIter>
std::string bytes_to_hex_string(InIter first, InIter last) {
    std::string hex_str;
    bytes_to_hex_string(first, last, hex_str);
    return hex_str;
}

template <typename InContainer>
std::string bytes_to_hex_string(const InContainer& bytes) {
    std::string hex_str;
    bytes_to_hex_string(bytes, hex_str);
    return hex_str;
}

class hash256_one_by_one final
{
   public:
    hash256_one_by_one() { init(); }

    void init() {
        buffer_.clear();
        std::fill(data_length_digits_, data_length_digits_ + 4, 0);
        std::copy(detail::initial_message_digest,
                  detail::initial_message_digest + 8, h_);
    }

    template <typename RaIter>
    void process(RaIter first, RaIter last) {
        add_to_data_length(static_cast<word_t>(distance(first, last)));
        copy(first, last, back_inserter(buffer_));
        size_t i = 0;
        for (; i + 64 <= buffer_.size(); i += 64) {
            detail::hash256_block(h_, buffer_.begin() + i,
                                  buffer_.begin() + i + 64);
        }
        buffer_.erase(buffer_.begin(), buffer_.begin() + i);
    }

    void finish() {
        byte_t temp[64];
        std::fill(temp, temp + 64, 0);
	    const auto remains = buffer_.size();
        std::copy(buffer_.begin(), buffer_.end(), temp);
        temp[remains] = 0x80;

        if (remains > 55) {
	        std::fill(temp + remains + 1, temp + 64, 0);
            detail::hash256_block(h_, temp, temp + 64);
	        std::fill(temp, temp + 64 - 4, 0);
        } else {
	        std::fill(temp + remains + 1, temp + 64 - 4, 0);
        }

        write_data_bit_length(&(temp[56]));
        detail::hash256_block(h_, temp, temp + 64);
    }

    template <typename OutIter>
    void get_hash_bytes(OutIter first, OutIter last) const {
        for (auto iter = h_; iter != h_ + 8; ++iter) {
            for (size_t i = 0; i < 4 && first != last; ++i) {
                *(first++) = detail::mask_8_bit(
                    static_cast<byte_t>((*iter >> (24 - 8 * i))));
            }
        }
    }

   private:
    void add_to_data_length(word_t n) {
        word_t carry = 0;
        data_length_digits_[0] += n;
        for (auto& data_length_digit : data_length_digits_)
        {
	        data_length_digit += carry;
            if (data_length_digit >= 65536u) {
                carry = data_length_digit >> 16;
	            data_length_digit &= 65535u;
            } else {
                break;
            }
        }
    }
    void write_data_bit_length(byte_t* begin) {
        word_t data_bit_length_digits[4];
        std::copy(data_length_digits_, data_length_digits_ + 4,
                  data_bit_length_digits);

        // convert byte length to bit length (multiply 8 or shift 3 times left)
        word_t carry = 0;
        for (auto& data_bit_length_digit : data_bit_length_digits)
        {
	        const auto before_val = data_bit_length_digit;
	        data_bit_length_digit <<= 3;
	        data_bit_length_digit |= carry;
	        data_bit_length_digit &= 65535u;
            carry = (before_val >> (16 - 3)) & 65535u;
        }

        // write data_bit_length
        for (auto i = 3; i >= 0; --i) {
            (*begin++) = static_cast<byte_t>(data_bit_length_digits[i] >> 8);
            (*begin++) = static_cast<byte_t>(data_bit_length_digits[i]);
        }
    }

    std::vector<byte_t> buffer_;
    word_t data_length_digits_[4]{};  // as 64bit integer (16bit x 4 integer)
    word_t h_[8]{};
};

inline void get_hash_hex_string(const hash256_one_by_one& hasher,
                                std::string& hex_str) {
    byte_t hash[k_digest_size];
    hasher.get_hash_bytes(hash, hash + k_digest_size);
    return bytes_to_hex_string(hash, hash + k_digest_size, hex_str);
}

inline std::string get_hash_hex_string(const hash256_one_by_one& hasher) {
    std::string hex_str;
    get_hash_hex_string(hasher, hex_str);
    return hex_str;
}

namespace impl {
template <typename RaIter, typename OutIter>
void hash256_impl(RaIter first, RaIter last, OutIter first2, OutIter last2, int,
                  std::random_access_iterator_tag) {
    hash256_one_by_one hasher;
    // hasher.init();
    hasher.process(first, last);
    hasher.finish();
    hasher.get_hash_bytes(first2, last2);
}

template <typename InputIter, typename OutIter>
void hash256_impl(InputIter first, InputIter last, OutIter first2,
                  OutIter last2, const int buffer_size, std::input_iterator_tag) {
	std::vector<byte_t> buffer(buffer_size);
    hash256_one_by_one hasher;
    // hasher.init();
    while (first != last) {
        int size = buffer_size;
        for (int i = 0; i != buffer_size; ++i, ++first) {
            if (first == last) {
                size = i;
                break;
            }
            buffer[i] = *first;
        }
        hasher.process(buffer.begin(), buffer.begin() + size);
    }
    hasher.finish();
    hasher.get_hash_bytes(first2, last2);
}
}

template <typename InIter, typename OutIter>
void hash256(InIter first, InIter last, OutIter first2, OutIter last2,
             int buffer_size = DUMANSHA256_BUFFER_SIZE_FOR_INPUT_ITERATOR) {
    dumansha256::impl::hash256_impl(
        first, last, first2, last2, buffer_size,
        typename std::iterator_traits<InIter>::iterator_category());
}

template <typename InIter, typename OutContainer>
void hash256(InIter first, InIter last, OutContainer& dst) {
    hash256(first, last, dst.begin(), dst.end());
}

template <typename InContainer, typename OutIter>
void hash256(const InContainer& src, OutIter first, OutIter last) {
    hash256(src.begin(), src.end(), first, last);
}

template <typename InContainer, typename OutContainer>
void hash256(const InContainer& src, OutContainer& dst) {
    hash256(src.begin(), src.end(), dst.begin(), dst.end());
}

template <typename InIter>
void hash256_hex_string(InIter first, InIter last, std::string& hex_str) {
    byte_t hashed[k_digest_size];
    hash256(first, last, hashed, hashed + k_digest_size);
    std::ostringstream oss;
    output_hex(hashed, hashed + k_digest_size, oss);
    hex_str.assign(oss.str());
}

template <typename InIter>
std::string hash256_hex_string(InIter first, InIter last) {
    std::string hex_str;
    hash256_hex_string(first, last, hex_str);
    return hex_str;
}

inline void hash256_hex_string(const std::string& src, std::string& hex_str) {
    hash256_hex_string(src.begin(), src.end(), hex_str);
}

template <typename InContainer>
void hash256_hex_string(const InContainer& src, std::string& hex_str) {
    hash256_hex_string(src.begin(), src.end(), hex_str);
}

template <typename InContainer>
std::string hash256_hex_string(const InContainer& src) {
    return hash256_hex_string(src.begin(), src.end());
}

}

#endif
