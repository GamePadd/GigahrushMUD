#include <locale>
#include <codecvt>
#include <thread>
#include <mutex>
#include <atomic>

#include "asio.hpp"
#include "Client.h"

std::atomic<bool> running = true;

std::string ConvertCP1251ToUTF8(const std::string& str)
{
	int len = MultiByteToWideChar(1251, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len];
	MultiByteToWideChar(1251, 0, str.c_str(), -1, wstr, len);

	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, 0, 0);
	char* utf8 = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, 0, 0);

	std::string result(utf8);
	delete[] wstr;
	delete[] utf8;

	return result;
}

std::string ConvertUTF8ToCP1251(const std::string& str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr, len);

	len = WideCharToMultiByte(1251, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* cp1251 = new char[len];
	WideCharToMultiByte(1251, 0, wstr, -1, cp1251, len, NULL, NULL);

	std::string result(cp1251);
	delete[] wstr;
	delete[] cp1251;

	return result;
}

int main()
{
	#ifdef _WIN32
		SetConsoleCP(1251); 
		SetConsoleOutputCP(1251);
	#else
		setlocale(LC_ALL, "ru_RU.UTF-8");
	#endif
	//Config

	char ip[] = "localhost";
	char port[] = "15001";

	//audiere::AudioDevicePtr device = audiere::OpenDevice();
	//audiere::SoundEffect* sound = audiere::OpenSoundEffect(device, "effect.mp3", audiere::SINGLE);

	std::string nick;
	do {
		std::cout << "> Введите ник: ";
		std::getline(std::cin, nick);
	} while (nick == "");

	while (running) {
		asio::io_context io_context;
		//sound->play();
		try {
			Client client(io_context, ip, port);
			client.Connect();

			std::string nickConv = ConvertCP1251ToUTF8(nick);
			std::string str = client.Send(nickConv);
			std::string response_local = ConvertUTF8ToCP1251(str);
			std::cout << response_local << std::endl;

			while (true) {
				try {
					std::cout << "> ";
					std::string msg;
					std::getline(std::cin, msg);
					if (msg == "") continue;
					if (msg == "exit") { running = false; break; };

					std::string msg_utf8 = ConvertCP1251ToUTF8(msg);
					std::string str = client.Send(msg_utf8);
					std::string response_local = ConvertUTF8ToCP1251(str);
					std::cout << response_local << std::endl;
				}
				catch (const std::exception& e) {
					std::cout << "Ошибка соединения подключится.\nПопробовать снова? (Y/N) :";
					std::string ans;
					std::getline(std::cin, ans);
					if (ans == "Y" || ans == "y") {
						client.Connect();
					}
					else {
						running = false;
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cout << "Невозможно подключится.\nПопробовать снова? (Y/N) :";
			std::string ans;
			std::getline(std::cin, ans);
			if (ans == "Y" || ans == "y") {
				continue;
			}
			else {
				running = false;
			}
		}
	}
	return 0;
}