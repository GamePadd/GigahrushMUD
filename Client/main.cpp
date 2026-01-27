#include <locale>
#include <codecvt>
#include <thread>
#include <mutex>
#include <atomic>
#include <deque>

#include "asio.hpp"
#include "Client.h"

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/component_options.hpp"

#include "nlohmann/json.hpp"

#include "Config.h"
#include "Parser.h"

/*
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
}*/

enum class State {CONNECTED, DISCONNECTED};

std::atomic<bool> running = true;
std::atomic<bool> bgRunning = false;

std::mutex mtx;

State state = State::DISCONNECTED;

std::vector<ftxui::Element> logs;
std::vector<ftxui::Element> serverMessages;

std::string map;

std::string ip;
std::string port;
std::string nick;

asio::io_context io_context;
Client client(io_context, ip, port);

std::string lastCommand;

std::thread bg;

auto screen = ftxui::ScreenInteractive::Fullscreen();

void UpdateMsgThread();

void Connect() {
	std::lock_guard<std::mutex> lock(mtx);

	if (state == State::CONNECTED) { return; }

	try {
		client.ip = ip;
		client.port = port;

		client.Connect();
		state = State::CONNECTED;
		bgRunning = true;
		bg = std::thread(UpdateMsgThread);
		client.Send(nick);
	}
	catch (const std::exception& ec) {
		state = State::DISCONNECTED;
	}
}

void SendServ(std::string request) {
	std::lock_guard<std::mutex> lock(mtx);

	try {
		client.Send(request);
	}
	catch (const std::exception& e) {
		return;
	}
}

void UpdateMsgThread() {
	while (bgRunning) {
		asio::error_code ec;
		client.recv_buffer_server.resize(4096);
		if (client.socket.is_open()) {
			size_t br = client.socket.read_some(asio::buffer(client.recv_buffer_server), ec);

			if (ec) { continue; }

			client.recv_buffer_server.resize(br);

			try {
				nlohmann::json js = nlohmann::json::parse(client.recv_buffer_server);
				if (js["type"] == "ANSWER") {
					addLog(logs, js);
				}
				else if (js["type"] == "MAP") {
					map = js["content"];
				}
				else if (js["type"] == "SERVER") {
					addServerMsg(serverMessages, js);
				}
			}
			catch (std::exception& er) {
				//Пока уберу для релиза
				//logs.push_back(ftxui::text(er.what()) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));
			}

			screen.PostEvent(ftxui::Event::Special("refresh"));
		}
	}
	return;
}

void MainThread() {
	//FTXUI

	//First time box elements

	ftxui::Component ipInput = ftxui::Input(&ip);
	ftxui::Component portInput = ftxui::Input(&port);
	ftxui::Component nickInput = ftxui::Input(&nick);

	ftxui::Component ipInputHandler = ftxui::CatchEvent(ipInput, [&](ftxui::Event event) {
		if (event == ftxui::Event::Return) { return true; }
		return false;
		});

	ftxui::Component portInputHandler = ftxui::CatchEvent(portInput, [&](ftxui::Event event) {
		if (event == ftxui::Event::Return) { return true; }
		return false;
		});

	ftxui::Component nickInputHandler = ftxui::CatchEvent(nickInput, [&](ftxui::Event event) {
		if (event == ftxui::Event::Return) { Connect();  return true; }
		return false;
		});

	ftxui::Component connectButton = ftxui::Button("Подключиться", Connect);

	ftxui::Component firstField = ftxui::Container::Vertical({
		ipInputHandler,
		portInputHandler,
		nickInputHandler,
		connectButton
		});

	//Main box elements
	std::string userCommand;
	ftxui::Component commandInput = ftxui::Input(&userCommand);
	int selected_log = 0;

	ftxui::Component logWindow = ftxui::Renderer([&] {
		return ftxui::vbox(logs) | ftxui::focusPositionRelative(0.0f, 1.0f);
	});

	/*ftxui::Component serverWindow = ftxui::Renderer([&] {
		std::vector<ftxui::Element> elements;

		for (const auto& log : serverMessages) {
			elements.push_back(
				ftxui::paragraph(log)
			);
		}

		return ftxui::vbox(elements) | ftxui::frame |
			ftxui::vscroll_indicator | ftxui::focusPositionRelative(0.0f, 1.0f);
	});*/

	ftxui::Component serverWindow = ftxui::Renderer([&] {
		return ftxui::vbox(serverMessages) | ftxui::focusPositionRelative(0.0f, 1.0f);
	});

	ftxui::Component mapWindow = ftxui::Renderer([&] {
		std::vector<ftxui::Element> elements;

		elements.push_back(ftxui::paragraph(map));

		return ftxui::vbox(elements) | ftxui::focusPositionRelative(0.0f, 1.0f);
	});

	ftxui::Component mainInputHandler = ftxui::CatchEvent(commandInput, [&](ftxui::Event event) {
		if (event == ftxui::Event::Return) {
			if (userCommand == "") { return true; }

			logs.push_back(ftxui::text(""));
			logs.push_back(ftxui::text("---------------------"));
			logs.push_back(ftxui::text(""));

			logs.push_back(ftxui::text(userCommand) | ftxui::color(DECORATE_COLOR) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 50));

			SendServ(userCommand);
			lastCommand = userCommand;
			userCommand = "";
			return true;
		}

		if (event == ftxui::Event::ArrowUp) {
			userCommand = lastCommand;
			return true;
		}

		return false;
		});

	ftxui::Component mainBox = ftxui::Container::Vertical({
			firstField,
			logWindow,
			mainInputHandler,
			serverWindow,
			mapWindow
		});

	ftxui::Component renderer = ftxui::Renderer(mainBox, [&] {
		//Logs

		auto login_form = ftxui::vbox({
			ftxui::text("IP: "), ipInput->Render(), ftxui::separator(),
			ftxui::text("Порт: "), portInput->Render(), ftxui::separator(),
			ftxui::text("Имя игрока: "), nickInput->Render(), ftxui::separator(),
			connectButton->Render() | ftxui::center
			});

		auto centered_content = ftxui::vbox({
			ftxui::filler() | ftxui::flex,
			login_form,
			ftxui::filler() | ftxui::flex
			}) | ftxui::flex;

		if (state == State::DISCONNECTED) {
			return ftxui::window(ftxui::text("Вход") | ftxui::center | ftxui::bold, centered_content)
				| ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 35)
				| ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 14)
				| ftxui::center
				| ftxui::color(MAIN_COLOR);
		}

		auto game_box = ftxui::vbox({
			logWindow->Render() | ftxui::frame | ftxui::border | ftxui::flex,
			ftxui::hbox(ftxui::text("Команда: "), commandInput->Render()) }) | ftxui::flex;
		auto game_window = ftxui::window(ftxui::text("Гигахрущ"), game_box) | ftxui::color(MAIN_COLOR);

		auto server_box = ftxui::vbox({ serverWindow->Render() | ftxui::frame | ftxui::flex }) | ftxui::flex;
		auto server_window = ftxui::window(ftxui::text("Оповещения сервера"), server_box) | ftxui::color(ADD_COLOR2) | ftxui::flex;

		auto map_box = ftxui::vbox({ mapWindow->Render() | ftxui::frame | ftxui::flex }) | ftxui::flex;
		auto map_window = ftxui::window(ftxui::text("Карта"), map_box) | ftxui::color(ADD_COLOR1) | ftxui::flex;

		auto right_column = ftxui::vbox({
			server_window | ftxui::flex_grow | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 9),
			map_window | ftxui::flex_grow
		}) | ftxui::flex;  

		auto main_layout = ftxui::hbox({
			game_window | ftxui::flex_grow,
			right_column | ftxui::flex_grow  
		}) | ftxui::flex; 

		return main_layout;

		//return ftxui::window(ftxui::text("Гигахрущ"), game_box) | ftxui::flex | ftxui::color(ftxui::Color::Green);
		});
	screen.Loop(renderer);

	//EFTXUI

	return;
}

int main()
{
	std::thread mt(MainThread);

	if (mt.joinable()) {
		mt.join();
	}

	if (bg.joinable()) {
		bg.join();
	}

	/*
	#ifdef _WIN32
		SetConsoleCP(1251); 
		SetConsoleOutputCP(1251);
	#else
		setlocale(LC_ALL, "ru_RU.UTF-8");
	#endif
	//Config

	std::string ip;
	std::string port;

	std::cout << "Введите IP: ";
	std::getline(std::cin, ip);
	std::cout << "Введите порт: ";
	std::getline(std::cin, port);

	//audiere::AudioDevicePtr device = audiere::OpenDevice();
	//audiere::SoundEffect* sound = audiere::OpenSoundEffect(device, "effect.mp3", audiere::SINGLE);

	std::string nick;
	do {
		std::cout << "Введите ник: \n> ";
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
						break;
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
	*/
	return 0;
}