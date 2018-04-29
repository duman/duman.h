#ifndef DUMAN_H
#define DUMAN_H

#include <iostream>
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
#include <cassert>
#include <iterator>
#include <sstream>
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
void any_to_exit(int exit_value, bool show_message = true)
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

// Encrypts a string
string encrypt(string msg, string const& key)
{
    if(!key.size())
        return msg;
    
    for (string::size_type i = 0; i < msg.size(); ++i)
        msg[i] ^= key[i%key.size()];
    return msg;
}

// Decrypts the string, correct key needed (during encryption)
string decrypt(string const& msg, string const& key)
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

//sha256
#ifndef DUMANSHA256_BUFFER_SIZE_FOR_INPUT_ITERATOR
#define DUMANSHA256_BUFFER_SIZE_FOR_INPUT_ITERATOR \
    1048576  //=1024*1024: default is 1MB memory
#endif

namespace DUMANSHA256
{
typedef unsigned long word_t;
typedef unsigned char byte_t;

static const size_t k_digest_size = 32;

namespace detail {
inline byte_t mask_8bit(byte_t x) { return x & 0xff; }

inline word_t mask_32bit(word_t x) { return x & 0xffffffff; }

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

inline word_t ch(word_t x, word_t y, word_t z) { return (x & y) ^ ((~x) & z); }

inline word_t maj(word_t x, word_t y, word_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

inline word_t rotr(word_t x, std::size_t n) {
    assert(n < 32);
    return mask_32bit((x >> n) | (x << (32 - n)));
}

inline word_t bsig0(word_t x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }

inline word_t bsig1(word_t x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }

inline word_t shr(word_t x, std::size_t n) {
    assert(n < 32);
    return x >> n;
}

inline word_t ssig0(word_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ shr(x, 3); }

inline word_t ssig1(word_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ shr(x, 10); }

template <typename RaIter1, typename RaIter2>
void hash256_block(RaIter1 message_digest, RaIter2 first, RaIter2 last) {
    assert(first + 64 == last);
    static_cast<void>(last);  // for avoiding unused-variable warning
    word_t w[64];
    std::fill(w, w + 64, 0);
    for (std::size_t i = 0; i < 16; ++i) {
        w[i] = (static_cast<word_t>(mask_8bit(*(first + i * 4))) << 24) |
               (static_cast<word_t>(mask_8bit(*(first + i * 4 + 1))) << 16) |
               (static_cast<word_t>(mask_8bit(*(first + i * 4 + 2))) << 8) |
               (static_cast<word_t>(mask_8bit(*(first + i * 4 + 3))));
    }
    for (std::size_t i = 16; i < 64; ++i) {
        w[i] = mask_32bit(ssig1(w[i - 2]) + w[i - 7] + ssig0(w[i - 15]) +
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

    for (std::size_t i = 0; i < 64; ++i) {
        word_t temp1 = h + bsig1(e) + ch(e, f, g) + add_constant[i] + w[i];
        word_t temp2 = bsig0(a) + maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = mask_32bit(d + temp1);
        d = c;
        c = b;
        b = a;
        a = mask_32bit(temp1 + temp2);
    }
    *message_digest += a;
    *(message_digest + 1) += b;
    *(message_digest + 2) += c;
    *(message_digest + 3) += d;
    *(message_digest + 4) += e;
    *(message_digest + 5) += f;
    *(message_digest + 6) += g;
    *(message_digest + 7) += h;
    for (std::size_t i = 0; i < 8; ++i) {
        *(message_digest + i) = mask_32bit(*(message_digest + i));
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

class hash256_one_by_one {
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
        add_to_data_length(std::distance(first, last));
        std::copy(first, last, std::back_inserter(buffer_));
        std::size_t i = 0;
        for (; i + 64 <= buffer_.size(); i += 64) {
            detail::hash256_block(h_, buffer_.begin() + i,
                                  buffer_.begin() + i + 64);
        }
        buffer_.erase(buffer_.begin(), buffer_.begin() + i);
    }

    void finish() {
        byte_t temp[64];
        std::fill(temp, temp + 64, 0);
        std::size_t remains = buffer_.size();
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
        for (const word_t* iter = h_; iter != h_ + 8; ++iter) {
            for (std::size_t i = 0; i < 4 && first != last; ++i) {
                *(first++) = detail::mask_8bit(
                    static_cast<byte_t>((*iter >> (24 - 8 * i))));
            }
        }
    }

   private:
    void add_to_data_length(word_t n) {
        word_t carry = 0;
        data_length_digits_[0] += n;
        for (std::size_t i = 0; i < 4; ++i) {
            data_length_digits_[i] += carry;
            if (data_length_digits_[i] >= 65536u) {
                carry = data_length_digits_[i] >> 16;
                data_length_digits_[i] &= 65535u;
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
        for (std::size_t i = 0; i < 4; ++i) {
            word_t before_val = data_bit_length_digits[i];
            data_bit_length_digits[i] <<= 3;
            data_bit_length_digits[i] |= carry;
            data_bit_length_digits[i] &= 65535u;
            carry = (before_val >> (16 - 3)) & 65535u;
        }

        // write data_bit_length
        for (int i = 3; i >= 0; --i) {
            (*begin++) = static_cast<byte_t>(data_bit_length_digits[i] >> 8);
            (*begin++) = static_cast<byte_t>(data_bit_length_digits[i]);
        }
    }
    std::vector<byte_t> buffer_;
    word_t data_length_digits_[4];  // as 64bit integer (16bit x 4 integer)
    word_t h_[8];
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
                  OutIter last2, int buffer_size, std::input_iterator_tag) {
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
    DUMANSHA256::impl::hash256_impl(
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
