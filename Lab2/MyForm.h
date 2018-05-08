#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
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

	public ref class MyForm : public System::Windows::Forms::Form
	{
		// Common
		ref class WinSocket
		{
			static WSADATA *ws;
			SOCKET main_socket;
			sockaddr_in *adr;
			std::vector<SOCKET> *sockets;
			std::vector<sockaddr_in> *adrresses;
			char* ip;
			u_short port;
			size_t max_client_count;
			size_t client_count;
		public:
			WinSocket()
			{
				this->sockets = new std::vector<SOCKET>();
				this->adrresses = new std::vector<sockaddr_in>();
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
				this->adrresses->clear();
				delete this->adrresses;
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
				if (this->ip) delete[] this->ip;
				this->ip = new char[16];
				strcpy(this->ip, inet_ntoa((in_addr)adr->sin_addr));
				String ^res;
				size_t i = 0;
				while (ip[i] != '\0')
				{
					res += ip[i];
					i++;
				}
				return res;
			}
			size_t GetPort()
			{
				sockaddr_in name;
				int n_l = sizeof(name);
				ZeroMemory(&name, sizeof(name));
				getsockname(this->main_socket, (sockaddr*)&name, &n_l);
				this->port = ntohs(name.sin_port);
				return this->port;
			}
			void Set_IP(String ^ip)
			{
				if (this->ip) delete[] this->ip;
				size_t l = ip->Length;
				this->ip = new char[l + 1];
				this->ip[l] = '\0';
				for (size_t i = 0; i < l; i++) this->ip[i] = (char)ip[i];

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
				hostent* hn;
				hn = gethostbyname(INADDR_ANY);

				ZeroMemory(this->adr, sizeof(this->adr));
				adr->sin_family = AF_INET;
				this->adr->sin_addr.S_un.S_addr = *(DWORD*)hn->h_addr_list[0];
				memset(this->adr->sin_zero, 0, 8);
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
				if ((bind(this->main_socket, (sockaddr*)this->adr, sizeof(this->adr))) == SOCKET_ERROR)
				{
					int k = WSAGetLastError();
					return false;
				}
				return true;
			}
			bool Connect()
			{
				if (SOCKET_ERROR == (connect(main_socket, (sockaddr *)adr, sizeof(adr)))) 
					return false;
				return true;
			}
			bool Disconnect()
			{
				if (SOCKET_ERROR == (shutdown(main_socket, 0))) return false;
				return true;
			}
			bool Listen()
			{
				if (FAILED(listen(this->main_socket, this->max_client_count))) 
					return false;
				return true;
			}
			SOCKET Accept()
			{
				int len = sizeof(sockaddr_in);
				this->adrresses->push_back(sockaddr_in());
				ZeroMemory(&this->adrresses[this->client_count], len);
				this->sockets->push_back(accept(this->main_socket, (sockaddr*)&this->adrresses[this->client_count], &len));
				if (FAILED(this->sockets->back()))
				{
					this->sockets->pop_back();
					this->adrresses->pop_back();
					return SOCKET_ERROR;
				}
				return this->sockets->back();
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
		};
		WinSocket ^Socket;
		bool is_active;
		bool is_server;

		// Server
		Thread^ listen_thr;
		List<Thread^> client_threads;

		bool StartServer()
		{
			Socket = gcnew WinSocket();
			label_info->Text = "Инициализация серверного сокета...";
			if (!Socket->InitializeSocket())
			{
				MessageBox::Show("Не удалось проинициализировать сокет сервера.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error);
				TerminateServer();
				return false;
			}
			Socket->Set_IPAuto();
			Socket->Set_PortAuto();
			label_info->Text = "Биндим сокет на автоматический адресс...";
			if (!Socket->Bind())
			{
				MessageBox::Show("Не удалось забиндить сокет сервера", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error);
				TerminateServer();
				return false;
			}
			tb_ip->Text = Socket->GetIP();
			tb_port->Text = Socket->GetPort().ToString();
			label_info->Text = "Ставим серверный сокет в слушающий режим...";
			if (!Socket->Listen())
			{
				MessageBox::Show("Не удалось поставить сокет сервера на слушающий режим", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error);
				TerminateServer();
				return false;
			}
			label_info->Text = "Запускаем поток обработки клиентских соединений.";
			listen_thr = gcnew Thread(gcnew ThreadStart(this, &MyForm::Listening));
			listen_thr->Start();
			label_info->Text = "Готово!";
			return true;
		}
		void TerminateServer()
		{
			if (listen_thr != nullptr && listen_thr->ThreadState == ThreadState::Running)
				listen_thr->Abort();
			Socket->~WinSocket();
			is_active = false;
			label_info->Text = "Строка состояния";
		}
		void Listening()
		{
			while (true)
			{
				SOCKET s = Socket->Accept();
				if (s == SOCKET_ERROR) continue;
				client_threads.Add(gcnew Thread(gcnew ParameterizedThreadStart(this, &MyForm::ClientProcessing)));
				client_threads[(client_threads.Count - 1)]->Start(s);
			}
		}
		void ClientProcessing(Object^ obj)
		{
			SOCKET sock = (SOCKET)obj;
		}

		// Client
		bool StartClient()
		{
			Socket = gcnew WinSocket();
			
			return true;
		}
		void TerminateClient()
		{
			Socket->~WinSocket();
			is_active = false;
			label_info->Text = "Строка состояния";
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
			this->rtb_all_msg = (gcnew System::Windows::Forms::RichTextBox());
			this->label_info = (gcnew System::Windows::Forms::Label());
			this->b_send = (gcnew System::Windows::Forms::Button());
			this->b_att = (gcnew System::Windows::Forms::Button());
			this->rbt_my_msg = (gcnew System::Windows::Forms::RichTextBox());
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
			// rtb_all_msg
			// 
			this->rtb_all_msg->Location = System::Drawing::Point(11, 3);
			this->rtb_all_msg->Name = L"rtb_all_msg";
			this->rtb_all_msg->ReadOnly = true;
			this->rtb_all_msg->Size = System::Drawing::Size(860, 449);
			this->rtb_all_msg->TabIndex = 4;
			this->rtb_all_msg->Text = L"";
			// 
			// label_info
			// 
			this->label_info->AutoSize = true;
			this->label_info->Location = System::Drawing::Point(12, 625);
			this->label_info->Name = L"label_info";
			this->label_info->Size = System::Drawing::Size(128, 17);
			this->label_info->TabIndex = 3;
			this->label_info->Text = L"Строка состояния";
			// 
			// b_send
			// 
			this->b_send->Location = System::Drawing::Point(776, 458);
			this->b_send->Name = L"b_send";
			this->b_send->Size = System::Drawing::Size(96, 53);
			this->b_send->TabIndex = 2;
			this->b_send->Text = L"Отправить";
			this->b_send->UseVisualStyleBackColor = true;
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
			this->tb_ip->Size = System::Drawing::Size(100, 22);
			this->tb_ip->TabIndex = 4;
			this->tb_ip->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::tb_ip_KeyPress);
			// 
			// tb_port
			// 
			this->tb_port->Location = System::Drawing::Point(64, 57);
			this->tb_port->Name = L"tb_port";
			this->tb_port->ReadOnly = true;
			this->tb_port->Size = System::Drawing::Size(100, 22);
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
			MessageBox::Show("Не удалось проинициализировать библиотеку WinSock. Функция WSAStartup выполнена некорректно.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error);
			Application::Exit();
		}
		is_active = false;
	}
	private: System::Void MyForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
	{
		if (!WinSocket::CloseLibrary())
		{
			MessageBox::Show("Не удалось выгрузить библиотеку WinSock. Функция WSACleanup выполнена некорректно.", "Ошибка WinSock!", MessageBoxButtons::OK, MessageBoxIcon::Error);
			Application::Exit();
		}
		if (is_active)
		{
			if (is_server) TerminateServer();
			else TerminateClient();
		}
	}
	private: System::Void tb_name_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
	{
		if (!((e->KeyChar > 47 && e->KeyChar < 59) || (e->KeyChar >= 65 && e->KeyChar <= 122) || e->KeyChar == 8) || e->KeyChar == 92)
			e->Handled = true;
	}
	private: System::Void b_ok_name_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if (b_ok_name->Text->Length > 0)
		{
			b_ok_name->Enabled = false;
			tb_name->ReadOnly = true;
			b_create_room->Enabled = true;
			b_connet_room->Enabled = true;
		}
	}
	private: System::Void b_create_room_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		is_active = true;
		is_server = true;
		if (!StartServer()) return;

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

		is_server = false;
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
			if (!StartClient()) return;
			b_connect->Text = "Disconnect";
			panel_top->Enabled = false;
			panel_main->Enabled = true;
			is_active = true;
		}
		else
		{
			if (is_server) TerminateServer();
			else TerminateClient();
			b_connect->Text = "Connect";
			tb_ip->Text = "";
			tb_port->Text = "";
			panel_top->Enabled = true;
			panel_main->Enabled = false;
			panel_info->Enabled = false;
		}
	}
};
}