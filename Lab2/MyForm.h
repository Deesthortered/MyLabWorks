#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <queue>
#include <string>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

namespace Lab2 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace System::Runtime::InteropServices;

	public ref class MyForm : public System::Windows::Forms::Form
	{
		ref class WinSocket
		{
			static WSADATA *ws;
			SOCKET main_socket;
			sockaddr_in *adr;
			std::vector<SOCKET> *sockets;

			char* ip;
			u_short port;
			size_t max_client_count;
			size_t client_count;
		public:
			WinSocket()
			{
				this->sockets = new std::vector<SOCKET>();
				this->adr = new sockaddr_in;
				ZeroMemory(this->adr, sizeof(this->adr));
				this->ip = nullptr;
				this->max_client_count = SOMAXCONN;
				this->client_count = 0;
			}
			~WinSocket()
			{
				CloseSocket();
				delete this->sockets;
				delete this->adr;
				if (this->ip) delete[] this->ip;
			}

			static bool InitializeLibrary()
			{
				ws = new WSADATA;
				if (FAILED(WSAStartup(MAKEWORD(2, 0), ws))) return false;
				return true;
			}
			static bool CloseLibrary()
			{
				if (FAILED(WSACleanup())) return false;
				delete ws;
				return true;
			}
			bool InitializeSocket()
			{
				if (INVALID_SOCKET == (main_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)))
				{
					closesocket(this->main_socket);
					return false;
				}
				return true;
			}
			void CloseSocket()
			{
				for (size_t i = 0; i < this->sockets->size(); i++)
				{
					shutdown(this->sockets->at(i), 0);
					closesocket(this->sockets->at(i));
				}
				this->sockets->clear();
				shutdown(this->main_socket, 0);
				closesocket(this->main_socket);
			}

			String^ GetIP()
			{
				return gcnew String(this->ip);
			}
			size_t GetPort()
			{
				return this->port;
			}
			void Set_IP(String ^ip)
			{
				if (this->ip) delete[] this->ip;
				this->ip = new char[16];
				char *k = (char*)(void*)Marshal::StringToHGlobalAnsi(ip);
				strcpy(this->ip, k);
				this->ip[15] = '\0';

				ZeroMemory(this->adr, sizeof(this->adr));
				adr->sin_family = AF_INET;
				adr->sin_addr.S_un.S_addr = inet_addr(this->ip);
			}
			void Set_Port(u_short port)
			{
				this->port = port;
				adr->sin_port = htons(port);
			}
			void Set_IPAuto()
			{
				ZeroMemory(this->adr, sizeof(this->adr));
				adr->sin_family = AF_INET;
				hostent* hn;
				hn = gethostbyname(INADDR_ANY);
				this->adr->sin_addr.S_un.S_addr = *(DWORD*)hn->h_addr_list[0];
			}
			void Set_PortAuto()
			{
				this->adr->sin_port = htons(0);
			}
			size_t GetClientCount()
			{
				return this->max_client_count;
			}
			void SetClientCount(size_t c)
			{
				this->max_client_count = c;
				this->sockets->reserve(c);
			}
			bool Bind()
			{
				sockaddr_in a = *this->adr;
				if (bind(this->main_socket, (sockaddr*)&a, sizeof(a)) == SOCKET_ERROR) return false;
				this->updateIpPort();
				return true;
			}
			bool Connect()
			{
				sockaddr_in a = *this->adr;
				if (SOCKET_ERROR == (connect(main_socket, (sockaddr *)&a, sizeof(a)))) 
					return false;
				return true;
			}
			bool Disconnect()
			{
				if (SOCKET_ERROR == (shutdown(main_socket, 0))) return false;
				closesocket(this->main_socket);
				return true;
			}
			bool DisconnectClient(SOCKET sock)
			{
				if (SOCKET_ERROR == (shutdown(sock, 0))) return false;
				closesocket(sock);
				for (size_t i = 0; i < this->sockets->size(); i++)
					if (this->sockets->at(i) == sock)
					{
						this->sockets->erase(this->sockets->begin() + i);
						return true;
					}
				return false;
			}
			bool Listen()
			{
				if (FAILED(listen(this->main_socket, this->max_client_count))) 
					return false;
				return true;
			}
			SOCKET Accept()
			{
				this->sockets->push_back(accept(this->main_socket, 0, 0));
				if (FAILED(this->sockets->back()))
				{
					this->sockets->pop_back();
					return SOCKET_ERROR;
				}
				return this->sockets->back();
			}
			bool GetData(char *data, size_t data_size)
			{
				int len = recv(this->main_socket, data, data_size, 0);
				if ((len == SOCKET_ERROR) || (len == 0)) return false;
				return true;
			}
			bool SendData(char *data, size_t data_size)
			{
				if (SOCKET_ERROR == send(this->main_socket, data, data_size, 0)) return false;
				return true;
			}
			bool GetData(char *data, size_t data_size, SOCKET s)
			{
				int len = recv(s, data, data_size, 0);
				if ((len == SOCKET_ERROR) || (len == 0)) return false;
				return true;
			}
			bool SendData(char *data, size_t data_size, SOCKET s)
			{
				if (SOCKET_ERROR == send(s, data, data_size, 0)) return false;
				return true;
			}
		private:
			void updateIpPort()
			{
				sockaddr_in name;
				int n_l = sizeof(name);
				ZeroMemory(&name, sizeof(name));
				getsockname(this->main_socket, (sockaddr*)&name, &n_l);
				if (this->ip) delete[] this->ip;
				this->ip = new char[16];
				ip[15] = '\0';
				strcpy(this->ip, inet_ntoa((in_addr)name.sin_addr));
				this->port = ntohs(name.sin_port);
			}
		};
		ref class Interface
		{
			Label ^infoline;
			ListBox ^members;
			RichTextBox ^allMessages;
			RichTextBox ^currMessage;
		public:
			Interface(Label ^%infoline, ListBox ^%members, RichTextBox ^%allMessages, RichTextBox ^%currMessage)
			{
				this->infoline = infoline;
				this->members = members;
				this->allMessages = allMessages;
				this->currMessage = currMessage;
			}
			void InfoLine(String ^s)
			{
				infoline->Text = s;
			}
			void AddMemberName(String ^s)
			{
				this->members->Items->Add(s + "\n");
			}
			void DeleteMemberName(String ^s)
			{
				this->members->Items->Remove(s);
			}
			void ClearMemberBox()
			{
				this->members->Items->Clear();
			}

			void Message(String ^s)
			{
				this->allMessages->Text += (s + "\n");
			}
			void NewMemberConnected(String ^who)
			{
				this->allMessages->Text += (who + " подключился к чату.\n");
			}
			void NewUserMessage(String ^who, String ^what)
			{
				this->allMessages->Text += (who + ": \t" + what + "\n");
			}
			void ClearMessages()
			{
				this->allMessages->Text = "";
			}
		};
		ref class Engine
		{
		protected:
			Interface ^face;
			String ^name;
			String ^ip;
			String ^port;

			WinSocket ^Socket;
			bool is_active;
			bool is_server;

			const size_t buff_len = 1024;
			char *buff = nullptr;
			char* r_hello1 =		"Hey!\0";
			char* r_hello2 =		"You!\0";
			char* r_ok =			"OK\0";
			char* r_end_names =		"ENDNAMES\0";
			char* r_update =		"UPDATE\0";
			char* r_end_update =	"ENDUPDATE\0";
			char* r_disconnect =	"DISCONNECT\0";

		public:
			Engine()
			{
				this->is_active = false;
				this->buff = nullptr;
			}
			~Engine()
			{
				if (buff) delete[] buff;
			}
			void SetInterface(Interface ^f)
			{
				this->face = f;
			}

			String^ GetIP()
			{
				return this->ip;
			}
			String^ GetPort()
			{
				return this->port;
			}
			bool IsActive()
			{
				return this->is_active;
			}
			bool IsServer()
			{
				return this->is_server;
			}
			static void ErrorMessage(int k)
			{
				switch (k)
				{
				case 0: { MessageBox::Show("Не удалось проинициализировать библиотеку WinSock. Функция WSAStartup выполнена некорректно.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 1: { MessageBox::Show("Не удалось выгрузить библиотеку WinSock. Функция WSACleanup выполнена некорректно.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 2: { MessageBox::Show("Не удалось проинициализировать серверный сокет.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 3: { MessageBox::Show("Не удалось забиндить сокет сервера", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 4: { MessageBox::Show("Не удалось поставить сокет сервера на слушающий режим", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 5: { MessageBox::Show("Не удалось проинициализировать клиентский сокет.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 6: { MessageBox::Show("Не удалось подключится к указаному адресу.", "Ошибка подключения!", MessageBoxButtons::OK, MessageBoxIcon::Error); } break;
				case 7: { MessageBox::Show("Подключение произошло, но не было подтверждено.", "Ошибка подключения!", MessageBoxButtons::OK, MessageBoxIcon::Warning); } break;
				case 8: { MessageBox::Show("Поле IP и порта не может быть пустым. Введите данные.", "Внимание!", MessageBoxButtons::OK, MessageBoxIcon::Warning); } break;
				case 10: {} break;
				case 11: {} break;
				case 12: {} break;
				case 13: {} break;
				default: MessageBox::Show("Неизвестная фатально-летальная ошибка №" + Convert::ToString(k), "ERRORЩИНА!!!", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}

			bool virtual Start() { return false; }
			void virtual Terminate() {}
			void virtual SendMessage() {}
		protected:
			void ClearBuffer()
			{
				memset(buff, 0, buff_len);
			}
		};
		ref class Server : public Engine
		{
			Thread^ listen_thr;
			List<Thread^> client_threads;
			List<SOCKET> client_sockets;
			List<String^> names;
			std::vector<std::queue<std::string>> *client_data;

		public:
			Server(String ^name, String ^ip, String ^port)
			{
				this->name = name;
				this->ip = ip;
				this->port = port;
				this->is_server = true;
				this->client_data = new std::vector<std::queue<std::string>>();
			}
			~Server()
			{
				delete this->client_data;
			}

			virtual bool Start() override
			{
				this->is_active = true;
				Socket = gcnew WinSocket();
				buff = new char[buff_len];
				ClearBuffer();
				face->InfoLine("Инициализация серверного сокета...");
				if (!Socket->InitializeSocket())
				{
					ErrorMessage(2);
					Terminate();
					return false;
				}
				Socket->Set_IPAuto();
				Socket->Set_PortAuto();
				face->InfoLine("Биндим сокет на автоматический адресс...");
				if (!Socket->Bind())
				{
					ErrorMessage(3);
					Terminate();
					return false;
				}
				this->ip = Socket->GetIP();
				this->port = Socket->GetPort().ToString();
				face->InfoLine("Ставим серверный сокет в слушающий режим...");
				if (!Socket->Listen())
				{
					ErrorMessage(4);
					Terminate();
					return false;
				}
				face->InfoLine("Запускаем поток обработки клиентских соединений.");
				listen_thr = gcnew Thread(gcnew ThreadStart(this, &Server::Listening));
				listen_thr->Start();
				face->InfoLine("Готово!");
				face->Message("Сервер создан. Имя сервера: " + name);
				face->AddMemberName(name);
				this->names.Add(name);
				return true;
			}
			virtual void Terminate() override
			{
				if (listen_thr != nullptr)
					listen_thr->Abort();
				client_sockets.Clear();
				Socket->~WinSocket();
				delete[] buff;
				is_active = false;
				face->InfoLine("Выберите режим работы программы.");
				face->ClearMemberBox();
				face->ClearMessages();
			}
			void Listening()
			{
				while (true)
				{
					SOCKET s = Socket->Accept();
					client_sockets.Add(s);
					if (s == SOCKET_ERROR) continue;
					client_threads.Add(gcnew Thread(gcnew ParameterizedThreadStart(this, &Server::ConnectionFunc)));
					int ind = client_threads.Count - 1;
					this->client_data->push_back(std::queue<std::string>());
					client_threads[ind]->Start(ind);
				}
			}
			void ConnectionFunc(Object^ index)
			{
				SOCKET sock = client_sockets[(int)index];
				Socket->SendData(r_hello1, strlen(r_hello1), sock);
				Socket->GetData(buff, buff_len, sock);
				if (strcmp(r_hello2, buff))
				{
					Socket->DisconnectClient(sock);
					return;
				}
				// Поключение с клиентом полностью установлено
				// Получаем имя клиента
				ClearBuffer();
				Socket->GetData(buff, buff_len, sock);
				String ^client_name = gcnew String(buff);
				this->names.Add(client_name);
				face->AddMemberName(client_name);
				face->NewMemberConnected(client_name);

				// Отсылаем ему список всех участников
				for (int i = 0; i < names.Count; i++)
				{
					char *k = (char*)(void*)Marshal::StringToHGlobalAnsi(this->names[i]);
					Socket->SendData(k, this->names[i]->Length, sock);
					ClearBuffer();
					Socket->GetData(buff, buff_len, sock);
					if (strcmp(buff, r_ok)) throw 1;
				}
				Socket->SendData(r_end_names, strlen(r_end_names), sock);
				ClearBuffer();
				
				while (true)
				{
					ClearBuffer();
					Socket->GetData(buff, buff_len, sock);
					if (!strcmp(buff, r_update)) RequestUpdate((int)index);
					else if (!strcmp(buff, r_disconnect))
					{
						RequestDisconect((int)index);
						return;
					}		
				}
			}

			void RequestUpdate(int ind)
			{
				while (!this->client_data->at(ind).empty())
				{
					int l = this->client_data->at(ind).front().length();
					char *k = new char[l+1];
					strcpy(k, this->client_data->at(ind).front().c_str());
					this->client_data->at(ind).pop();
					Socket->SendData(k, l, this->client_sockets[ind]);
					delete[] k;
					ClearBuffer();
					Socket->GetData(buff, buff_len);
					if (!strcmp(buff, r_ok)) throw 1;
				}
				Socket->SendData(r_end_update, strlen(r_end_update), this->client_sockets[ind]);
			}
			void RequestDisconect(int ind)
			{
				Socket->SendData(r_ok, strlen(r_ok), this->client_sockets[ind]);
				face->Message(this->names[ind] + " покинул чат.\n");
				face->DeleteMemberName(this->names[ind]);
				Socket->DisconnectClient(this->client_sockets[ind]);
				this->client_sockets.RemoveAt(ind);
				this->names.RemoveAt(ind);
				this->client_data->erase(this->client_data->begin() + ind);
			}

			virtual void SendMessage() override
			{
				for (size_t i = 0; i < this->client_data->size(); i++)
					this->client_data->at(i).push("kokoko");
			}
		};
		ref class Client : public Engine
		{
			enum class ClientStates
			{
				Update, Disconect
			};
			ClientStates state;
			const size_t delay_ms = 100;
			Thread^ connect_thr;

		public:
			Client(String ^name, String ^ip, String ^port)
			{
				this->name = name;
				this->ip = ip;
				this->port = port;
				this->is_server = false;
				this->state = ClientStates::Update;
			}

			virtual bool Start() override
			{
				this->is_active = true;
				Socket = gcnew WinSocket();
				buff = new char[buff_len];
				ClearBuffer();
				face->InfoLine("Инициализация клиентского сокета...");
				if (!Socket->InitializeSocket())
				{
					ErrorMessage(5);
					Terminate();
					return false;
				}
				Socket->Set_IP(this->ip);
				Socket->Set_Port(Convert::ToUInt16(this->port));
				face->InfoLine("Подключаемся по указаному адресу...");
				if (!Socket->Connect())
				{
					ErrorMessage(6);
					Terminate();
					return false;
				}
				Socket->GetData(buff, buff_len);
				if (strcmp(r_hello1, buff))
				{
					ErrorMessage(7);
					Terminate();
					return false;
				}
				Socket->SendData(r_hello2, strlen(r_hello2));
				face->InfoLine("Соединение установлено. Создаем поток для приема/передачи данных...");
				connect_thr = gcnew Thread(gcnew ThreadStart(this, &Client::ConnectionFunc));
				connect_thr->Start();
				face->InfoLine("Готово!");
				return true;
			}
			virtual void Terminate() override
			{
				if (connect_thr != nullptr)
					connect_thr->Abort();
				Socket->~WinSocket();
				delete[] buff;
				is_active = false;
				face->InfoLine("Введите IP и порт сервера для подключения.");
				face->ClearMemberBox();
				face->ClearMessages();
			}
			void ConnectionFunc()
			{
				// Подключение с сервером полностью установлено
				// Отправляем ему свое имя
				char *name = (char*)(void*)Marshal::StringToHGlobalAnsi(this->name);
				Socket->SendData(name, this->name->Length);
				ClearBuffer();

				// Принимаем имена всех учасников
				Socket->GetData(buff, buff_len);
				Socket->SendData(r_ok, strlen(r_ok));
				String ^serv; bool b = true;
				while (strcmp(buff, r_end_names))
				{
					face->AddMemberName(gcnew String(buff));
					if (b) { serv = gcnew String(buff); b = false; }
					ClearBuffer();
					Socket->GetData(buff, buff_len);
					Socket->SendData(r_ok, strlen(r_ok));
				}
				ClearBuffer();
				face->Message("Вы поключились к серверу " + serv + ".\n");
				// Запускаем конечный автомат связи с сервером.
				while (true)
				{
					Sleep(this->delay_ms);
					switch (this->state)
					{
					case ClientStates::Update: { RequestUpdate(); } break;
					case ClientStates::Disconect: { RequestDisconect(); return; } break;
					}
				}
			}

			void RequestUpdate()
			{
				ClearBuffer();
				Socket->SendData(r_update, strlen(r_update));
				Socket->GetData(buff, buff_len);
				while (strcmp(buff, r_end_update))
				{
					// action
					face->NewUserMessage(this->name, gcnew String(buff));
					
					ClearBuffer();
					Socket->SendData(r_ok, strlen(r_ok));
					Socket->GetData(buff, buff_len);
				}
				ClearBuffer();
			}
			void RequestDisconect()
			{
				Socket->SendData(r_disconnect, strlen(r_disconnect));
				ClearBuffer();
				Socket->GetData(buff, buff_len);
				if (strcmp(buff, r_ok)) throw 1;
				Socket->~WinSocket();
				delete[] buff;
				is_active = false;
				face->InfoLine("Введите IP и порт сервера для подключения.");
				face->ClearMemberBox();
				face->ClearMessages();
			}

			virtual void SendMessage() override
			{

			}
		};

		Engine ^engine;
		Interface ^face;
		void StartServer()
		{
			engine = gcnew Server(tb_name->Text, tb_ip->Text, tb_port->Text);
			engine->SetInterface(face);
		}
		void StartClient()
		{
			engine = gcnew Client(tb_name->Text, tb_ip->Text, tb_port->Text);
			engine->SetInterface(face);
		}

	public:	MyForm(void)
		{
			InitializeComponent();
		}
	protected: ~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  lb_members;
	private: System::Windows::Forms::Label^  label_members;
	private: System::Windows::Forms::Panel^  panel_top;
	private: System::Windows::Forms::TextBox^  tb_name;
	private: System::Windows::Forms::Label^  label_name;
	private: System::Windows::Forms::Button^  b_ok_name;
	private: System::Windows::Forms::Button^  b_create_room;
	private: System::Windows::Forms::Button^  b_connet_room;
	private: System::Windows::Forms::Panel^  panel_main;
	private: System::Windows::Forms::RichTextBox^  rbt_my_msg;
	private: System::Windows::Forms::Button^  b_send;
	private: System::Windows::Forms::Button^  b_att;
	private: System::Windows::Forms::Label^  label_info;
	private: System::Windows::Forms::RichTextBox^  rtb_all_msg;
	private: System::Windows::Forms::Panel^  panel_info;
	private: System::Windows::Forms::Label^  label_server_info;
	private: System::Windows::Forms::Label^  label_port;
	private: System::Windows::Forms::Label^  label_ip;
	private: System::Windows::Forms::TextBox^  tb_ip;
	private: System::Windows::Forms::TextBox^  tb_port;
	private: System::Windows::Forms::Button^  b_connect;
	private: System::ComponentModel::Container ^components;
#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->panel_top = (gcnew System::Windows::Forms::Panel());
			this->b_create_room = (gcnew System::Windows::Forms::Button());
			this->b_connet_room = (gcnew System::Windows::Forms::Button());
			this->b_ok_name = (gcnew System::Windows::Forms::Button());
			this->tb_name = (gcnew System::Windows::Forms::TextBox());
			this->label_name = (gcnew System::Windows::Forms::Label());
			this->panel_main = (gcnew System::Windows::Forms::Panel());
			this->lb_members = (gcnew System::Windows::Forms::ListBox());
			this->label_members = (gcnew System::Windows::Forms::Label());
			this->rtb_all_msg = (gcnew System::Windows::Forms::RichTextBox());
			this->b_send = (gcnew System::Windows::Forms::Button());
			this->b_att = (gcnew System::Windows::Forms::Button());
			this->rbt_my_msg = (gcnew System::Windows::Forms::RichTextBox());
			this->label_info = (gcnew System::Windows::Forms::Label());
			this->panel_info = (gcnew System::Windows::Forms::Panel());
			this->b_connect = (gcnew System::Windows::Forms::Button());
			this->tb_ip = (gcnew System::Windows::Forms::TextBox());
			this->tb_port = (gcnew System::Windows::Forms::TextBox());
			this->label_port = (gcnew System::Windows::Forms::Label());
			this->label_ip = (gcnew System::Windows::Forms::Label());
			this->label_server_info = (gcnew System::Windows::Forms::Label());
			this->panel_top->SuspendLayout();
			this->panel_main->SuspendLayout();
			this->panel_info->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_top
			// 
			this->panel_top->Controls->Add(this->b_create_room);
			this->panel_top->Controls->Add(this->b_connet_room);
			this->panel_top->Location = System::Drawing::Point(0, 38);
			this->panel_top->Name = L"panel_top";
			this->panel_top->Size = System::Drawing::Size(354, 76);
			this->panel_top->TabIndex = 0;
			// 
			// b_create_room
			// 
			this->b_create_room->Enabled = false;
			this->b_create_room->Location = System::Drawing::Point(12, 3);
			this->b_create_room->Name = L"b_create_room";
			this->b_create_room->Size = System::Drawing::Size(168, 61);
			this->b_create_room->TabIndex = 4;
			this->b_create_room->Text = L"Создать комнату";
			this->b_create_room->UseVisualStyleBackColor = true;
			this->b_create_room->Click += gcnew System::EventHandler(this, &MyForm::b_create_room_Click);
			// 
			// b_connet_room
			// 
			this->b_connet_room->Enabled = false;
			this->b_connet_room->Location = System::Drawing::Point(186, 3);
			this->b_connet_room->Name = L"b_connet_room";
			this->b_connet_room->Size = System::Drawing::Size(163, 61);
			this->b_connet_room->TabIndex = 3;
			this->b_connet_room->Text = L"Подключится к комнате";
			this->b_connet_room->UseVisualStyleBackColor = true;
			this->b_connet_room->Click += gcnew System::EventHandler(this, &MyForm::b_connet_room_Click);
			// 
			// b_ok_name
			// 
			this->b_ok_name->Location = System::Drawing::Point(298, 9);
			this->b_ok_name->Name = L"b_ok_name";
			this->b_ok_name->Size = System::Drawing::Size(56, 23);
			this->b_ok_name->TabIndex = 2;
			this->b_ok_name->Text = L"OK";
			this->b_ok_name->UseVisualStyleBackColor = true;
			this->b_ok_name->Click += gcnew System::EventHandler(this, &MyForm::b_ok_name_Click);
			// 
			// tb_name
			// 
			this->tb_name->Location = System::Drawing::Point(110, 9);
			this->tb_name->Name = L"tb_name";
			this->tb_name->Size = System::Drawing::Size(182, 22);
			this->tb_name->TabIndex = 1;
			this->tb_name->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_name_KeyPress);
			// 
			// label_name
			// 
			this->label_name->AutoSize = true;
			this->label_name->Location = System::Drawing::Point(8, 9);
			this->label_name->Name = L"label_name";
			this->label_name->Size = System::Drawing::Size(96, 17);
			this->label_name->TabIndex = 0;
			this->label_name->Text = L"Введите имя:";
			// 
			// panel_main
			// 
			this->panel_main->Controls->Add(this->lb_members);
			this->panel_main->Controls->Add(this->label_members);
			this->panel_main->Controls->Add(this->rtb_all_msg);
			this->panel_main->Controls->Add(this->b_send);
			this->panel_main->Controls->Add(this->b_att);
			this->panel_main->Controls->Add(this->rbt_my_msg);
			this->panel_main->Enabled = false;
			this->panel_main->Location = System::Drawing::Point(0, 110);
			this->panel_main->Name = L"panel_main";
			this->panel_main->Size = System::Drawing::Size(885, 512);
			this->panel_main->TabIndex = 1;
			// 
			// lb_members
			// 
			this->lb_members->FormattingEnabled = true;
			this->lb_members->ItemHeight = 16;
			this->lb_members->Location = System::Drawing::Point(673, 31);
			this->lb_members->Name = L"lb_members";
			this->lb_members->Size = System::Drawing::Size(199, 420);
			this->lb_members->TabIndex = 6;
			// 
			// label_members
			// 
			this->label_members->AutoSize = true;
			this->label_members->Location = System::Drawing::Point(670, 11);
			this->label_members->Name = L"label_members";
			this->label_members->Size = System::Drawing::Size(82, 17);
			this->label_members->TabIndex = 5;
			this->label_members->Text = L"Участники:";
			// 
			// rtb_all_msg
			// 
			this->rtb_all_msg->Location = System::Drawing::Point(11, 3);
			this->rtb_all_msg->Name = L"rtb_all_msg";
			this->rtb_all_msg->ReadOnly = true;
			this->rtb_all_msg->Size = System::Drawing::Size(652, 449);
			this->rtb_all_msg->TabIndex = 4;
			this->rtb_all_msg->Text = L"";
			// 
			// b_send
			// 
			this->b_send->Location = System::Drawing::Point(776, 458);
			this->b_send->Name = L"b_send";
			this->b_send->Size = System::Drawing::Size(96, 53);
			this->b_send->TabIndex = 2;
			this->b_send->Text = L"Отправить";
			this->b_send->UseVisualStyleBackColor = true;
			this->b_send->Click += gcnew System::EventHandler(this, &MyForm::b_send_Click);
			// 
			// b_att
			// 
			this->b_att->Location = System::Drawing::Point(669, 458);
			this->b_att->Name = L"b_att";
			this->b_att->Size = System::Drawing::Size(101, 53);
			this->b_att->TabIndex = 1;
			this->b_att->Text = L"Прикрепить";
			this->b_att->UseVisualStyleBackColor = true;
			// 
			// rbt_my_msg
			// 
			this->rbt_my_msg->Location = System::Drawing::Point(12, 458);
			this->rbt_my_msg->Name = L"rbt_my_msg";
			this->rbt_my_msg->Size = System::Drawing::Size(651, 53);
			this->rbt_my_msg->TabIndex = 0;
			this->rbt_my_msg->Text = L"";
			// 
			// label_info
			// 
			this->label_info->AutoSize = true;
			this->label_info->Location = System::Drawing::Point(12, 625);
			this->label_info->Name = L"label_info";
			this->label_info->Size = System::Drawing::Size(222, 17);
			this->label_info->TabIndex = 3;
			this->label_info->Text = L"Введите ваше имя или никнейм.";
			// 
			// panel_info
			// 
			this->panel_info->Controls->Add(this->b_connect);
			this->panel_info->Controls->Add(this->tb_ip);
			this->panel_info->Controls->Add(this->tb_port);
			this->panel_info->Controls->Add(this->label_port);
			this->panel_info->Controls->Add(this->label_ip);
			this->panel_info->Controls->Add(this->label_server_info);
			this->panel_info->Enabled = false;
			this->panel_info->Location = System::Drawing::Point(355, -2);
			this->panel_info->Name = L"panel_info";
			this->panel_info->Size = System::Drawing::Size(530, 116);
			this->panel_info->TabIndex = 2;
			// 
			// b_connect
			// 
			this->b_connect->Location = System::Drawing::Point(64, 85);
			this->b_connect->Name = L"b_connect";
			this->b_connect->Size = System::Drawing::Size(100, 24);
			this->b_connect->TabIndex = 5;
			this->b_connect->Text = L"Connect";
			this->b_connect->UseVisualStyleBackColor = true;
			this->b_connect->Visible = false;
			this->b_connect->Click += gcnew System::EventHandler(this, &MyForm::b_connect_Click);
			// 
			// tb_ip
			// 
			this->tb_ip->Location = System::Drawing::Point(64, 29);
			this->tb_ip->Name = L"tb_ip";
			this->tb_ip->ReadOnly = true;
			this->tb_ip->Size = System::Drawing::Size(116, 22);
			this->tb_ip->TabIndex = 4;
			this->tb_ip->Text = L"192.168.1.165";
			this->tb_ip->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_ip_KeyPress);
			// 
			// tb_port
			// 
			this->tb_port->Location = System::Drawing::Point(64, 57);
			this->tb_port->Name = L"tb_port";
			this->tb_port->ReadOnly = true;
			this->tb_port->Size = System::Drawing::Size(116, 22);
			this->tb_port->TabIndex = 3;
			this->tb_port->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_port_KeyPress);
			// 
			// label_port
			// 
			this->label_port->AutoSize = true;
			this->label_port->Location = System::Drawing::Point(20, 60);
			this->label_port->Name = L"label_port";
			this->label_port->Size = System::Drawing::Size(38, 17);
			this->label_port->TabIndex = 2;
			this->label_port->Text = L"Port:";
			// 
			// label_ip
			// 
			this->label_ip->AutoSize = true;
			this->label_ip->Location = System::Drawing::Point(34, 34);
			this->label_ip->Name = L"label_ip";
			this->label_ip->Size = System::Drawing::Size(24, 17);
			this->label_ip->TabIndex = 1;
			this->label_ip->Text = L"IP:";
			// 
			// label_server_info
			// 
			this->label_server_info->AutoSize = true;
			this->label_server_info->Location = System::Drawing::Point(7, 11);
			this->label_server_info->Name = L"label_server_info";
			this->label_server_info->Size = System::Drawing::Size(193, 17);
			this->label_server_info->TabIndex = 0;
			this->label_server_info->Text = L"Информация о соединении.";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(884, 646);
			this->Controls->Add(this->panel_info);
			this->Controls->Add(this->label_info);
			this->Controls->Add(this->b_ok_name);
			this->Controls->Add(this->tb_name);
			this->Controls->Add(this->panel_main);
			this->Controls->Add(this->panel_top);
			this->Controls->Add(this->label_name);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->ShowIcon = false;
			this->Text = L"Сетевой чат";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &MyForm::MyForm_FormClosed);
			this->Shown += gcnew System::EventHandler(this, &MyForm::MyForm_Shown);
			this->panel_top->ResumeLayout(false);
			this->panel_main->ResumeLayout(false);
			this->panel_main->PerformLayout();
			this->panel_info->ResumeLayout(false);
			this->panel_info->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void MyForm_Shown(System::Object^  sender, System::EventArgs^  e) 
	{
		if (!WinSocket::InitializeLibrary())
		{
			Engine::ErrorMessage(0);
			Application::Exit();
		}
		engine = gcnew Engine;
		face = gcnew Interface(label_info, lb_members, rtb_all_msg, rbt_my_msg);
		engine->SetInterface(face);
	}
	private: System::Void MyForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
	{
		if (engine->IsActive()) engine->Terminate();
		if (!WinSocket::CloseLibrary())
		{
			Engine::ErrorMessage(1);
			Application::Exit();
		}
	}
	private: System::Void tb_name_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (!((e->KeyChar > 47 && e->KeyChar < 59) || (e->KeyChar >= 65 && e->KeyChar <= 122) || e->KeyChar == 8) || e->KeyChar == 92)
			e->Handled = true;
	}
	private: System::Void b_ok_name_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (tb_name->Text->Length == 0) return;
		b_ok_name->Enabled = false;
		tb_name->ReadOnly = true;
		b_create_room->Enabled = true;
		b_connet_room->Enabled = true;
		label_info->Text = "Выберите режим работы программы.";
	}
	private: System::Void b_create_room_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		StartServer();
		if (!engine->Start()) return;

		tb_ip->Text = engine->GetIP();
		tb_port->Text = engine->GetPort();
		panel_info->Enabled = true;
		panel_top->Enabled = false;
		panel_main->Enabled = true;
		b_connect->Text = "Disconnect";
		b_connect->Visible = true;
	}
	private: System::Void b_connet_room_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		panel_top->Enabled = false;
		panel_info->Enabled = true;
		b_connect->Visible = true;
		tb_ip->ReadOnly = false;
		tb_port->ReadOnly = false;
		label_info->Text = "Введите IP и порт сервера для подключения.";
	}
	
	private: System::Void tb_ip_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if ((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8 && e->KeyChar != 46)
			e->Handled = true;
	}
	private: System::Void tb_port_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (((e->KeyChar <= 47 || e->KeyChar >= 59) && e->KeyChar != 8) || (tb_port->Text->Length == 5 && e->KeyChar != 8))
			e->Handled = true;
	}
	private: System::Void b_connect_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if (b_connect->Text == "Connect")
		{
			if (tb_ip->Text->Length == 0 || tb_port->Text->Length == 0)
			{
				Engine::ErrorMessage(8);
				return;
			}
			StartClient();
			if (!engine->Start()) return;
			b_connect->Text = "Disconnect";
			panel_top->Enabled = false;
			panel_main->Enabled = true;
			tb_ip->ReadOnly = true;
			tb_port->ReadOnly = true;
		}
		else
		{
			engine->Terminate();
			b_connect->Text = "Connect";
			tb_ip->Text = "";
			tb_port->Text = "";
			panel_top->Enabled = true;
			panel_main->Enabled = false;
			panel_info->Enabled = false;
			label_info->Text = "Выберите режим работы программы.";
		}
	}

	private: System::Void b_send_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		engine->SendMessage();
	}
};
}