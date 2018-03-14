#ifndef DUMAN_H
#define DUMAN_H

#include "stdafx.h"
#include <conio.h>
#include <sapi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include <wininet.h>
#include <random>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <experimental/filesystem>
#include <Windows.h>
#include <ShellAPI.h>
#include <functional>
#include <future>
#pragma comment(lib, "Wininet")
#define VARIABLE_NAME(variable) (string)#variable
namespace fs = std::experimental::filesystem;
using namespace std;

// Usage: timer timer_1(time_in_milliseconds, true_if_async, &function_name, argument_1, arg_2, ...); true makes it multithreaded
class timer
{
public:
    template <class Callable, class... Arguments>
    timer(int after, const bool async, Callable&& f, Arguments&&... args)
    {
        std::function<typename std::result_of<Callable(Arguments...)>::type()> task(std::bind(std::forward<Callable>(f), std::forward<Arguments>(args)...));

        if (async)
        {
            std::thread([after, task]()
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
T key_to_exit(int delay_time_in_ms, T char_for_quit, int exit_value, bool show_message)
{
	if(show_message)
		cout << "\nPress " << char_for_quit << " to exit!\n";
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
T key_to_exit(T char_for_quit, bool show_message)
{
	if(show_message)
		cout << "\nPress " << char_for_quit << " to exit!\n";
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
void esc_to_exit(bool show_message)
{
	if (show_message)
		cout << "\nPress ESC to exit!";
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

// Format: 'any_to_exit(true, 0);'
void any_to_exit(int exit_value, bool show_message)
{
	if (show_message)
		cout << "\nPress any key to exit!";
	while (true)
	{
		if (_kbhit())
			exit(exit_value);
	} Sleep(200);
}

// Helper for FTP connection
__int64 get_file_size(LPCSTR name)
{
	HANDLE hFile = CreateFile(name, GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE)
		return -1; // error condition, could call GetLastError to find out more

	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size))
	{
		CloseHandle(hFile);
		return -1; // error condition, could call GetLastError to find out more
	}

	CloseHandle(hFile);
	return size.QuadPart;
}

// Grabs file name from path.
string get_file_name(const string& s) {

	char sep = '\\';

	size_t i = s.rfind(sep, s.length());

	if (i != string::npos)
		return(s.substr(i+1, s.length() - i));

	return("");
}

// Creates a FTP connection and allows you to upload a single file
void upload_single_file(const LPCSTR url, const LPCSTR ftp_username, const LPCSTR ftp_password, const LPCSTR local_file_location, const LPCSTR remote_file_location)
{
	const auto file_name = fs::path(local_file_location).filename();
	const auto file_size = get_file_size(local_file_location);

	if (file_size < 0)
	{
		cout << "File doesn't exist or path is wrong!\n";
		cout << "Please check and try again.\n";
		esc_to_exit(true);
	}

	const auto h_internet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	auto start = chrono::steady_clock::now();
	const auto h_ftp_session = InternetConnect(h_internet, url, INTERNET_DEFAULT_FTP_PORT, ftp_username, ftp_password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);

	if (!h_ftp_session)
	{
		cout << "Connection to " << url << " FTP server(s) is failed!\n";
		cout << "Please check URL, username and password\n";
		cout << "variable values and the internet connection.\n\n";
		esc_to_exit(true);
	}
	else
	{
		cout << "Connection to " << url << " FTP server(s) is successful!\n\n";
		if(file_size < 1024)
			cout << "Uploading " << file_name << " (" << file_size << " B)\n";
		if(file_size >= 1024 && file_size < 1048576)
			cout << "Uploading " << file_name << " (" << file_size / 1024.0 << " KB)\n";
		if(file_size >= 1048576)
			cout << "Uploading " << file_name << " (" << file_size / 1048576.0 << " MB)\n";
		if(file_size > 1048576)
		{
			cout << "------------------------------------\n";
			cout << "It may take some time to upload this file.\n";
			cout << "You will get the detailed report after it's completed.\n";
		}
	}
	FtpPutFile(h_ftp_session, local_file_location, remote_file_location, FTP_TRANSFER_TYPE_BINARY, 0);
	InternetCloseHandle(h_ftp_session);
	InternetCloseHandle(h_internet);
	auto end = chrono::steady_clock::now();
	const auto duration_in_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	const auto duration_in_s = chrono::duration_cast<chrono::seconds>(end - start).count();
	cout << "\nUpload successful!\n";
	cout << "------------------------------------\n";
	cout << file_name << setprecision (2) << fixed << " @ " << static_cast<double>(file_size) / static_cast<double>(duration_in_ms) << " kb/s\n";
	cout << "------------------------------------\n";
	cout << "Total file size   : " << file_size << " bytes\n";
	if(duration_in_ms < 1000)
		cout << "Total time elapsed: " << duration_in_ms << " millisecond(s)\n";
	if(duration_in_ms >= 1000 && duration_in_ms < 60000)
		cout << "Total time elapsed: " << duration_in_ms / 1000.0 << " second(s)\n";
	if(duration_in_ms >= 60000)
		cout << "Total time elapsed: " << duration_in_ms / 60000.0 << " minute(s)\n";
	cout << "------------------------------------\n";
	cout << "File uploaded from: " << local_file_location << "\n";
	cout << "File uploaded to  : " << remote_file_location << "\n";
	cout << "------------------------------------\n";
}

// convert string to LPCWSTR
wstring s2ws(const string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    wstring r(buf);
    delete[] buf;
    return r;
}

// Format: 'L"Speak whatever is written here"'
bool text_to_speech(string input_text)
{
	wstring stemp = s2ws(input_text);
	LPCWSTR sentence = stemp.c_str();

	ISpVoice * pVoice = NULL;

	if (FAILED(::CoInitialize(NULL)))
		return FALSE;

	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	if (SUCCEEDED(hr))
	{
		hr = pVoice->Speak(sentence, SPF_IS_XML, NULL);
		pVoice->Release();
		pVoice = NULL;
	}

	::CoUninitialize();
	return TRUE;
}

void open_url(LPCSTR url)
{
	ShellExecute(0, 0, url, 0, 0 , SW_SHOW);
}

/* Format: 'sinput(true) to display ****'
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
string sinput(const char to_display = '*', const bool show_asterisk = false)
{
	const char backspace_key = 8;
	const char enter_key = 13;

	string sinput_field;
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
				cout << "\b \b";
				sinput_field.resize(sinput_field.length() - 1);
			}
		}
		else
		{
			sinput_field += ch;
			if (show_asterisk)
				cout << to_display;
			else
				cout << ch;
		}
	}
	cout << "\n";
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

	string sinput_field;
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
				cout << "\b \b";
				sinput_field.resize(sinput_field.length() - 1);
			}
		}
		else
		{
			sinput_field += ch;
			if (show_asterisk)
				cout << to_display;
			else
				cout << ch;
		}
	}
	cout << "\n";
	return stod(sinput_field);
}

// Format: "string_analyzer_eng("Your string")"
void string_analyzer_eng(string value)
{
	auto dsa_vowels = 0;
	auto dsa_consonants = 0;
	auto dsa_digits = 0;
	auto dsa_symbols = 0;
	auto dsa_lowercase_count = 0;
	auto dsa_uppercase_count = 0;

	for (size_t i = 0; i < value.size(); i++)
	{
		if (value[i] >= 48 && value[i] <= 57)
		{
			dsa_digits++;
			continue;
		}
		if(!(value[i] >= 65 && value[i] <= 90) && !(value[i] >= 97 && value[i] <= 122) && !(value[i] >= 48 && value[i] <= 57))
		{
			dsa_symbols++;
			continue;
		}
		if(value[i] >= 65 && value[i] <= 90)
		{
			dsa_uppercase_count++;
		}
		if(value[i] >= 97 && value[i] <= 122)
		{
			dsa_lowercase_count++;
		}
		if (value[i] == 'a' || value[i] == 'e' || value[i] == 'u' ||
			value[i] == 'o' || value[i] == 'i' || value[i] == 'A' ||
			value[i] == 'E' || value[i] == 'U' || value[i] == 'O' ||
			value[i] == 'I')
		{
			dsa_vowels++;
		}
		else
			dsa_consonants++;
	}
	if(dsa_vowels + dsa_consonants != dsa_uppercase_count + dsa_lowercase_count)
	{
		cout << "Something went wrong while analyzing!\n";
		cout << "Error: 0001A, send this error code to your system administrator.\n";
		cout << "\nPress q to end the program.";
		key_to_exit(200, 'q', 1, false);
	} const auto dsa_letters = dsa_uppercase_count + dsa_lowercase_count;
	cout << "-----------------\n";
	cout << "Analyzed string  : " << value << "\n";
	cout << "-----------------\n";
	cout << "Vowels           : " << dsa_vowels << "\n";
	cout << "Consonant        : " << dsa_consonants << "\n";
	cout << "-----------------\n";
	cout << "Uppercase letters: " << dsa_uppercase_count << "\n";
	cout << "Lowercase letters: " << dsa_lowercase_count << "\n";
	cout << "-----------------\n";
	cout << "Letter           : " << dsa_letters << "\n";
	cout << "Digit            : " << dsa_digits << "\n";
	cout << "Special Character: " << dsa_symbols << "\n";
	cout << "-----------------\n";
	cout << "Length           : " << value.length() << "\n";
	cout << "-----------------\n";
}

// Format: 'ping_url("dumanstudios.com");'
void ping_url(const string& url)
{
	cout << "Pinging " << url;
	cout << ", please wait..." << "\n";
	auto ping_command = "ping " + url;

	auto flag = system(ping_command.c_str());
}

// Format: 'binary_to_decimal(100001)' PS: This is a bit buggy on large numbers. Will fix it later on.
void binary_to_decimal(__int64 binary_number)
{
	auto decimal_value = 0;
	auto base = 1;

	while(binary_number)
	{
		int last_digit = binary_number % 10;
		binary_number = binary_number / 10;
		decimal_value += last_digit*base;
		base = base*2;
	}
	cout << decimal_value << "\n";
}

void to_clipboard(const string& s){
	OpenClipboard(0);
	EmptyClipboard();	
	HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,s.size()+1);
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
int password_score(string value)
{
	auto dsa_vowels = 0;
	auto dsa_consonants = 0;
	auto dsa_digits = 0;
	auto dsa_symbols = 0;
	auto dsa_lowercase_count = 0;
	auto dsa_uppercase_count = 0;

	for (size_t i = 0; i < value.size(); i++)
	{
		if (value[i] >= 48 && value[i] <= 57)
		{
			dsa_digits++;
			continue;
		}
		if(!(value[i] >= 65 && value[i] <= 90) && !(value[i] >= 97 && value[i] <= 122) && !(value[i] >= 48 && value[i] <= 57))
		{
			dsa_symbols++;
			continue;
		}
		if(value[i] >= 65 && value[i] <= 90)
		{
			dsa_uppercase_count++;
		}
		if(value[i] >= 97 && value[i] <= 122)
		{
			dsa_lowercase_count++;
		}
		if (value[i] == 'a' || value[i] == 'e' || value[i] == 'u' ||
			value[i] == 'o' || value[i] == 'i' || value[i] == 'A' ||
			value[i] == 'E' || value[i] == 'U' || value[i] == 'O' ||
			value[i] == 'I')
		{
			dsa_vowels++;
		}
		else
			dsa_consonants++;
	}

	const auto dsa_letters = dsa_uppercase_count + dsa_lowercase_count;
	const auto dsa_total_length = dsa_letters + dsa_digits + dsa_symbols;

	bool has_uppercase = false;
	bool has_lowercase = false;
	bool has_digits = false;
	bool has_symbols = false;
	bool length_is_greater_than_eight = false;
	auto amount_of_correct_stuff = 0;

	auto dsa_total_score = 0; // 5~N0v{2a
	dsa_total_score += dsa_total_length * 4;
	if(dsa_uppercase_count > 0)
	{
		dsa_total_score += (dsa_total_length - dsa_uppercase_count) * 2;
		has_uppercase = true;
		amount_of_correct_stuff++;
	}
	if(dsa_lowercase_count > 0)
	{
		dsa_total_score += (dsa_total_length - dsa_lowercase_count) * 2;
		has_lowercase = true;
		amount_of_correct_stuff++;
	}
	if(dsa_digits > 0)
	{
		dsa_total_score += dsa_digits * 4;
		has_digits = true;
		amount_of_correct_stuff++;
	}
	if(dsa_symbols > 0)
	{
		dsa_total_score += dsa_symbols * 6;
		has_symbols = true;
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
string password_generator(const int length_of_password = 12, bool enable_symbols = false, bool copy_to_clipboard = false)
{
	vector<char> password;
	srand (static_cast<unsigned int>(time(nullptr)));

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

	random_device r;
	shuffle(password.begin(), password.end(), default_random_engine(r()));

	string returning_password;

	for(auto i = 0; i < length_of_password; i++)
	{
		returning_password.push_back(password[i]);
	}

	if(copy_to_clipboard)
		to_clipboard(returning_password);

	return returning_password;
}

#endif
